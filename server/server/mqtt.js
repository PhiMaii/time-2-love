const aedes = require("aedes")();
const net = require("net");
const http = require("http");
const ws = require("ws");

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

        console.log(`📤 PUBLISH clientId=${client.id} topic=${packet.topic}`);

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