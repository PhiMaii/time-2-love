const aedes = require("aedes")();
const net = require("net");
const http = require("http");
const ws = require("ws");

function start(devices, push) {
    // MQTT TCP
    net.createServer(aedes.handle).listen(1883, () => {
        console.log("MQTT TCP listening on port 1883");
    });

    // MQTT WebSocket
    const httpServer = http.createServer();
    const wss = new ws.Server({ server: httpServer });

    wss.on("connection", socket => {
        const stream = ws.createWebSocketStream(socket);
        aedes.handle(stream);

        socket.on("close", () => { });
        socket.on("error", err => console.log("WS ERROR:", err.message));
    });

    httpServer.listen(9001, () => {
        console.log("MQTT WS listening on port 9001");
    });

    // Device tracking
    aedes.on("client", client => devices.clientConnect(client));
    aedes.on("clientDisconnect", client => devices.clientDisconnect(client));

    // Log subscribes/publishes
    aedes.on("subscribe", (subs, client) => {
        subs.forEach(sub => {
            console.log(`📥 SUBSCRIBE clientId=${client.id} topic=${sub.topic}`);
        });
    });

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

        await push.sendNotification(message);
        // savePushSubscriptions(Object.fromEntries(pushSubscriptions));
    });


    return aedes;
}

module.exports = { start };