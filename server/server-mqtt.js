const aedes = require("aedes")();
const net = require("net");
const http = require("http");
const ws = require("ws");
const fs = require("fs");
const path = require("path");
const webPush = require("web-push");
const express = require("express");
const bodyParser = require("body-parser");

// ──────────────────────────────
// FILE STORAGE
// ──────────────────────────────

const DEVICE_FILE = path.join(__dirname, "devices.json");
const PUSH_FILE = path.join(__dirname, "pushSubscriptions.json");

// Load stored devices
function loadDevices() {
    try {
        return JSON.parse(fs.readFileSync(DEVICE_FILE, "utf8"));
    } catch {
        return {};
    }
}

// Save devices
function saveDevices(devices) {
    fs.writeFileSync(DEVICE_FILE, JSON.stringify(devices, null, 2));
}

// Load push subscriptions
function loadPushSubscriptions() {
    try {
        return JSON.parse(fs.readFileSync(PUSH_FILE, "utf8"));
    } catch {
        return {};
    }
}

// Save push subscriptions
function savePushSubscriptions(subs) {
    fs.writeFileSync(PUSH_FILE, JSON.stringify(subs, null, 2));
}

// ──────────────────────────────
// VAPID
// ──────────────────────────────

// const vapidKeys = webPush.generateVAPIDKeys();
const vapidKeys = require("./vapid.json");

console.log("VAPID Public Key:", vapidKeys.publicKey);
console.log("VAPID Private Key:", vapidKeys.privateKey);

webPush.setVapidDetails(
    "mailto:you@example.com",
    vapidKeys.publicKey,
    vapidKeys.privateKey
);

// ──────────────────────────────
// PUSH SUBSCRIPTIONS (persistent)
// ──────────────────────────────

const pushSubscriptions = new Map(
    Object.entries(loadPushSubscriptions())
);

// ──────────────────────────────
// DEVICE TRACKING
// ──────────────────────────────

const onlineDevices = new Set();
const devices = loadDevices();

// ──────────────────────────────
// MQTT TCP (ESP)
// ──────────────────────────────

net.createServer(aedes.handle).listen(1883, () => {
    console.log("MQTT TCP listening on port 1883");
});

// ──────────────────────────────
// MQTT WebSocket
// ──────────────────────────────

const httpServer = http.createServer();
const wss = new ws.Server({ server: httpServer });

wss.on("connection", (socket, req) => {
    const clientIP = req.socket.remoteAddress;
    console.log(`🌐 WS CONNECT from ${clientIP}`);

    const stream = ws.createWebSocketStream(socket);
    aedes.handle(stream);

    socket.on("close", () => {
        console.log(`🌐 WS DISCONNECT from ${clientIP}`);
    });

    socket.on("error", err => {
        console.log(`⚠️ WS ERROR from ${clientIP}: ${err.message}`);
    });
});

httpServer.listen(9001, () => {
    console.log("MQTT WS listening on port 9001");
});

// ──────────────────────────────
// MQTT EVENTS
// ──────────────────────────────

aedes.on("client", client => {
    console.log(`🟢 CONNECT clientId=${client.id}`);
    onlineDevices.add(client.id);

    if (!devices[client.id]) {
        devices[client.id] = {
            created: Date.now(),
            lastSeen: Date.now()
        };
    } else {
        devices[client.id].lastSeen = Date.now();
    }
    saveDevices(devices);
});

aedes.on("clientDisconnect", client => {
    console.log(`🔴 DISCONNECT clientId=${client.id}`);
    onlineDevices.delete(client.id);
});

aedes.on("subscribe", (subs, client) => {
    subs.forEach(sub => {
        console.log(`📥 SUBSCRIBE clientId=${client.id} topic=${sub.topic}`);
    });
});

// ──────────────────────────────
// MQTT → PUSH BRIDGE
// ──────────────────────────────

aedes.on("publish", async (packet, client) => {
    if (!client) return;

    console.log(`📤 PUBLISH clientId=${client.id} topic=${packet.topic}`);

    const match = packet.topic.match(/^time2love\/pair\/(\d+)\/blink$/);
    if (!match) return;

    const pairId = match[1];

    const message = {
        title: "Blink detected!",
        body: `Pair ${pairId} blinked.`
    };

    for (const [endpoint, sub] of pushSubscriptions) {
        try {
            await webPush.sendNotification(sub, JSON.stringify(message));
            sub.lastUsed = Date.now();
        } catch (err) {
            if (err.statusCode === 404 || err.statusCode === 410) {
                console.log("🧹 Removing expired push:", endpoint);
                pushSubscriptions.delete(endpoint);
            } else {
                console.error("❌ Push error:", err.message);
            }
        }
    }

    savePushSubscriptions(Object.fromEntries(pushSubscriptions));
});

// ──────────────────────────────
// REST API
// ──────────────────────────────

const app = express();
app.use(bodyParser.json());
app.use((req, res, next) => {
    res.setHeader("Access-Control-Allow-Origin", "*");
    res.setHeader("Access-Control-Allow-Headers", "Content-Type");
    next();
});

app.get("/devices", (req, res) => {
    const list = Object.entries(devices).map(([id, info]) => ({
        deviceId: id,
        created: info.created,
        lastSeen: info.lastSeen,
        online: onlineDevices.has(id)
    }));
    res.json(list);
});

app.post("/device/:id/config", (req, res) => {
    const deviceId = req.params.id;
    if (!devices[deviceId]) return res.status(404).json({ error: "Unknown device" });

    const topic = `time2love/device/${deviceId}/config`;
    aedes.publish({ topic, payload: JSON.stringify(req.body), qos: 1 }, () => {
        res.json({ ok: true });
    });
});

// Register push
app.post("/registerPush", (req, res) => {
    const sub = req.body;
    if (!sub?.endpoint) return res.status(400).end();

    pushSubscriptions.set(sub.endpoint, {
        ...sub,
        created: Date.now(),
        lastUsed: Date.now()
    });

    savePushSubscriptions(Object.fromEntries(pushSubscriptions));
    console.log("✅ Push registered:", sub.endpoint);
    res.sendStatus(200);
});

// Unregister push
app.post("/unregisterPush", (req, res) => {
    const sub = req.body;
    if (!sub?.endpoint) return res.status(400).end();

    pushSubscriptions.delete(sub.endpoint);
    savePushSubscriptions(Object.fromEntries(pushSubscriptions));
    console.log("🗑️ Push unregistered:", sub.endpoint);
    res.sendStatus(200);
});

app.get("/vapidPublicKey", (req, res) => {
    res.json({ publicKey: vapidKeys.publicKey });
});

app.use("/", express.static(path.join(__dirname, "pwa")));

// ──────────────────────────────
// START REST SERVER
// ──────────────────────────────

const PORT = 3000;
app.listen(PORT, () => {
    console.log(`REST API listening on port ${PORT}`);
});