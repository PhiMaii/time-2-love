const express = require('express');
const bodyParser = require('body-parser');

const app = express();
app.use(bodyParser.json());

// Hard-coded event date: set this to something you like.
// Use an ISO string or compute epoch seconds directly.
// Example below: event on 2026-01-01 12:00:00 UTC -> compute epoch
const EVENT_EPOCH = Math.floor(new Date('2025-12-19T17:03:00Z').getTime() / 1000);

const devices = new Set();
const blinkFlags = {}; // map: deviceId -> {from: deviceId, ts: epoch}

app.use((req, res, next) => {
    // Simple CORS for devices
    res.setHeader('Access-Control-Allow-Origin', '*');
    res.setHeader('Access-Control-Allow-Headers', 'Content-Type');
    next();
});

// GET event: returns {"event": <unix_seconds>}
app.get('/event', (req, res) => {
    res.json({ event: EVENT_EPOCH });
});

// POST /register { device: "device1" }
// returns { peers: ["device2", ...] }
app.post('/register', (req, res) => {
    const device = req.body && req.body.device;
    if (!device) return res.status(400).json({ error: 'missing device' });

    devices.add(device);

    // return peers (other devices)
    const peers = Array.from(devices).filter(d => d !== device);
    res.json({ peers });
    console.log('Registered device', device, 'peers:', peers);
});

// POST /blink { from: "device1", to: "device2" }
// sets blink flag for "to"
app.post('/blink', (req, res) => {
    const { from, to } = req.body || {};
    if (!from || !to) return res.status(400).json({ error: 'missing from/to' });

    blinkFlags[to] = { from, ts: Math.floor(Date.now() / 1000) };
    console.log(`Blink request from ${from} -> ${to}`);
    res.json({ ok: true });
});

// GET /blink?device=device2
// returns { blink: true, from: "device1" } and clears the flag immediately
app.get('/blink', (req, res) => {
    const device = req.query.device;
    if (!device) return res.status(400).json({ error: 'missing device query' });

    const flag = blinkFlags[device];
    if (flag) {
        // clear on read
        delete blinkFlags[device];
        console.log(`Delivering blink to ${device} from ${flag.from}`);
        res.json({ blink: true, from: flag.from, ts: flag.ts });
    } else {
        res.json({ blink: false });
    }
});

// Simple status endpoint
app.get('/', (req, res) => {
    res.send(`Simple blink server. Event epoch: ${EVENT_EPOCH}\nDevices: ${Array.from(devices).join(',')}`);
});

const port = process.env.PORT || 3000;
app.listen(port, () => {
    console.log(`Server listening on port ${port}`);
    console.log('Event epoch:', EVENT_EPOCH);
});