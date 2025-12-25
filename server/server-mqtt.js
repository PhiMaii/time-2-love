const aedes = require("aedes")();
const net = require("net");
const http = require("http");
const ws = require("ws");
const fs = require("fs");
const path = require("path");

const DEVICE_FILE = path.join(__dirname, "devices.json");

// Helper: load all devices ever registered
function loadDevices() {
    try {
        return JSON.parse(fs.readFileSync(DEVICE_FILE, "utf8"));
    } catch (err) {
        return {}; // empty object if file does not exist
    }
}

// Helper: save devices to file
function saveDevices(devices) {
    fs.writeFileSync(DEVICE_FILE, JSON.stringify(devices, null, 2));
}

// ──────────────────────────────
// Keep track of online devices
// ──────────────────────────────
const onlineDevices = new Set();
const devices = loadDevices(); // { deviceId: { created: timestamp, ... } }

// ──────────────────────────────
// MQTT TCP (ESP)
// ──────────────────────────────
net.createServer(aedes.handle).listen(1883, () => {
    console.log("MQTT TCP listening on port 1883");
});

// ──────────────────────────────
// MQTT WebSocket (Expo / Browser)
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

    socket.on("error", (err) => {
        console.log(`⚠️ WS ERROR from ${clientIP}: ${err.message}`);
    });
});

httpServer.listen(9001, () => {
    console.log("MQTT WS listening on port 9001");
});

// ──────────────────────────────
// Track device connect/disconnect
// ──────────────────────────────
aedes.on("client", (client) => {
    console.log(`🟢 CONNECT clientId=${client.id} transport=${client.conn.transport}`);
    onlineDevices.add(client.id);

    // Ensure device is in JSON store
    if (!devices[client.id]) {
        devices[client.id] = {
            created: Date.now(),
            lastSeen: Date.now(),
        };
        saveDevices(devices);
    } else {
        devices[client.id].lastSeen = Date.now();
        saveDevices(devices);
    }
});

aedes.on("clientDisconnect", (client) => {
    console.log(`🔴 DISCONNECT clientId=${client.id} transport=${client.conn.transport}`);
    onlineDevices.delete(client.id);
});

// Optional: log subscriptions
aedes.on("subscribe", (subs, client) => {
    subs.forEach((sub) => {
        console.log(`📥 SUBSCRIBE clientId=${client.id} topic=${sub.topic}`);
    });
});

// Optional: log publishes
aedes.on("publish", (packet, client) => {
    if (!client) return; // broker publish
    console.log(`📤 PUBLISH clientId=${client.id} topic=${packet.topic}`);
});

// ──────────────────────────────
// HTTP endpoints for phone app
// ──────────────────────────────
const express = require("express");
const bodyParser = require("body-parser");
const app = express();
app.use(bodyParser.json());
app.use((req, res, next) => {
    res.setHeader("Access-Control-Allow-Origin", "*");
    res.setHeader("Access-Control-Allow-Headers", "Content-Type");
    next();
});

// Return list of all devices with online flag
app.get("/devices", (req, res) => {
    console.log("GET /devices");
    const list = Object.entries(devices).map(([id, info]) => ({
        deviceId: id,
        created: info.created,
        lastSeen: info.lastSeen,
        online: onlineDevices.has(id),
    }));
    res.json(list);
});

// Example endpoint to update device settings
app.post("/device/:id/config", (req, res) => {
    console.log(`POST /device/${req.params.id}/config`, req.body);
    const deviceId = req.params.id;
    if (!devices[deviceId]) return res.status(404).json({ error: "Unknown device" });

    // Publish config via MQTT
    const topic = `time2love/device/${deviceId}/config`;
    aedes.publish({ topic, payload: JSON.stringify(req.body), qos: 1 }, () => {
        console.log(`Config sent to ${deviceId}:`, req.body);
        res.json({ ok: true });
    });
});

// Start HTTP server for REST API
const PORT = 3000;
app.listen(PORT, () => {
    console.log(`REST API listening on port ${PORT}`);
});