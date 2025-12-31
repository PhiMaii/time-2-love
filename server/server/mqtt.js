const aedes = require("aedes")();
const net = require("net");
const http = require("http");
const ws = require("ws");

/*
MQTT Topics:
- time2love/pair/{pairId}/blink  (published by device when blink detected)
    - published: devices
    - subscribed: server, devices

    QoS 1

    {
    "sender": "devA",
    "ts": 1735500000,
    }

- time2love/pair/{pairId}/event  (timestamp for the next event)
    - published: server
    - subscribed: devices

    retain: true
    QoS 1

    TODO: Maybe use http instead?

- time2love/device/status
    - published: devices
    - subscribed: server

    {deviceId: "devA", status: "online", ts: 1735490000 }

##########

- time2love/device/{deviceId}/config/get
    - published: server
    - subscribed: device

    QoS 1

    { "requestId": "b6c7...", "ts": 1735490000 }

- time2love/device/{deviceId}/config/state
    - published: device
    - subscribed: server

    QoS 1

    {
    "requestId": "b6c7...",
    "ts": 1735490001,
    "config": { "sampleRate": 10, "mode": "eco" },
    "version": 42
    }

- time2love/device/{deviceId}/config/set
    - published: server
    - subscribed: device

    QoS 1

    {
    "requestId": "f1a2...",
    "ts": 1735490100,
    "expectedVersion": 42,
    "config": { "sampleRate": 5 }
    }

- time2love/device/{deviceId}/config/ack
    - published: device
    - subscribed: server

    QoS 1

    {
    "requestId": "f1a2...",
    "ok": true,
    "newVersion": 43
    }



*/

function brokerSend(topic, msg, qos = 0, retain = false) {
    aedes.publish({
        cmd: 'publish',
        qos: qos,
        retain: retain,
        topic,
        payload: Buffer.from(
            typeof msg === 'string' ? msg : JSON.stringify(msg)
        ),
    }, (err) => {
        if (err) console.error('broker publish error', err);
    });
}

function start(devices, push) {

    // MQTT TCP
    const MQTT_PORT = 1883;
    net.createServer(aedes.handle).listen(MQTT_PORT, () => {
        console.log(`MQTT TCP listening on port ${MQTT_PORT}`);
    });

    // MQTT over WebSocket
    const httpServer = http.createServer();
    const wss = new ws.Server({ server: httpServer });

    wss.on("connection", socket => {
        const stream = ws.createWebSocketStream(socket);
        aedes.handle(stream);

        socket.on("close", () => { });
        socket.on("error", err => console.log("WS ERROR:", err.message));
    });

    const WS_PORT = 9001;
    httpServer.listen(WS_PORT, () => {
        console.log(`MQTT WS listening on port ${WS_PORT}`);
    });

    // Handle client connections
    aedes.on("client", client => devices.clientConnect(client));

    // Handle client disconnections
    aedes.on("clientDisconnect", client => devices.clientDisconnect(client));

    // Log subscribes
    aedes.on("subscribe", (subs, client) => {
        subs.forEach(sub => {
            console.log(`📥 SUBSCRIBE clientId=${client.id} topic=${sub.topic}`);
        });
    });

    // Handle published messages
    aedes.on("publish", async (packet, client) => {
        if (!client) return;

        console.log(`📤 PUBLISH clientId=${client.id} QOS=${packet.qos} Retain=${packet.retain} topic=${packet.topic} Payload=${packet.payload}`);

        // Detect blink messages
        const match = packet.topic.match(/^time2love\/pair\/(\d+)\/blink$/);
        if (!match) return;

        const pairId = match[1];

        const message = {
            title: "Blink detected!",
            body: `Pair ${pairId} blinked.`,
            icon: "../config/heart.png",   // 👈 change this

        };

        // Send push notification
        await push.sendNotification(message);
        // savePushSubscriptions(Object.fromEntries(pushSubscriptions));
    });


    return aedes;
}

module.exports = { start };