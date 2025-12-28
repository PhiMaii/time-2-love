const express = require("express");
const bodyParser = require("body-parser");
const path = require("path");

function start(push, devices, aedes) {
    const app = express();

    app.use(bodyParser.json());

    // CORS headers
    app.use((req, res, next) => {
        res.setHeader("Access-Control-Allow-Origin", "*");
        res.setHeader("Access-Control-Allow-Headers", "Content-Type");
        next();
    });

    // Get a list of devices
    app.get("/devices", (req, res) => {
        res.json(devices.list());
    });

    // TODO: Send config to device
    app.post("/device/:id/config", (req, res) => {
        const deviceId = req.params.id;
        const list = devices.list().map(d => d.deviceId);
        if (!list.includes(deviceId)) return res.status(404).json({ error: "Unknown device" });

        const topic = `time2love/device/${deviceId}/config`;
        aedes.publish({ topic, payload: JSON.stringify(req.body), qos: 1 }, () => {
            res.json({ ok: true });
        });
    });

    // Register a device for push notifications
    app.post("/registerPush", (req, res) => {
        if (push.register(req.body)) res.sendStatus(200);
        else res.status(400).json({ error: "Invalid subscription" });
    });

    // Unregister a device from push notifications
    app.post("/unregisterPush", (req, res) => {
        if (push.unregister(req.body)) res.sendStatus(200);
        else res.status(400).json({ error: "Invalid subscription" });
    });

    // Get VAPID public key
    app.get("/vapidPublicKey", (req, res) => {
        res.json({ publicKey: require("../config/vapid.json").publicKey });
    });

    // Serve PWA files
    app.use("/", express.static(path.join(__dirname, "../pwa")));

    // Start server
    const PORT = 3000;
    app.listen(PORT, () => console.log(`REST API listening on port ${PORT}`));
}

module.exports = { start };