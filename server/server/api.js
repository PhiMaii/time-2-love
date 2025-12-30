const express = require("express");
const bodyParser = require("body-parser");
const fs = require("fs");
const path = require("path");

const EVENTS_FILE = path.join(__dirname, "../data/events.json");

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

    app.post("/device/:id/config", (req, res) => {
        const deviceId = req.params.id;

        // TODO: Send config to device
        res.sendStatus(501);

    });

    app.get("/device/:id/config", (req, res) => {
        const deviceId = req.params.id;

        // TODO: Get config from device
        res.sendStatus(501);

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

    // Health check endpoint
    app.get("/health", (req, res) => {
        // You can add extra checks here (db ping, cache ping, etc.)
        // If everything is OK:
        console.log("HEALTH Check OK")
        res.status(200).json({ ok: true, time: new Date().toISOString() });
    });

    app.post("/setEvent/api", (req, res) => {
        const { iso, unix } = req.body || {};

        if (typeof iso !== "string" || typeof unix !== "number" || !Number.isFinite(unix)) {
            return res.status(400).json({ ok: false, error: "Expected { iso: string, unix: number }" });
        }

        console.log("[setEvent] received:", { iso, unix });
        try {
            fs.writeFileSync(EVENTS_FILE, JSON.stringify({ iso, unix, savedAt: Date.now() }, null, 2));
        } catch (err) {
            console.error("[setEvent] failed to save event:", err);
            return res.status(500).json({ ok: false, error: "Failed to save event" });
        }

        res.json({ ok: true, received: { iso, unix } });
    });

    // Serve PWA files
    app.use("/",
        express.static(path.join(__dirname, "../pwa"), {
            index: "index.html",
        }));

    app.use(
        "/setEvent",
        express.static(path.join(__dirname, "../setEvent"), {
            index: "index.html",
        })
    );

    app.use("/ota",
        express.static(path.join(__dirname, "../ota"), {
            index: "index.html",
        }));

    // Start server
    const PORT = 3000;
    app.listen(PORT, () => console.log(`REST API listening on port ${PORT}`));

    console.log("Server URL: http://localhost:3000/ \n");
}

module.exports = { start };
