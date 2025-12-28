const fs = require("fs");
const path = require("path");
const DEVICE_FILE = path.join(__dirname, "../data/devices.json");

const devices = {};         // deviceId -> info
// const onlineDevices = new Set();

function load() {
    try {
        const data = fs.readFileSync(DEVICE_FILE, "utf8");
        Object.assign(devices, JSON.parse(data));
    } catch {
        // empty
    }
}

function save() {
    fs.writeFileSync(DEVICE_FILE, JSON.stringify(devices, null, 2));
}

function clientConnect(client) {
    // onlineDevices.add(client.id);
    if (!devices[client.id]) {
        devices[client.id] = { created: Date.now(), lastSeen: Date.now(), online: true };
    } else {
        devices[client.id].lastSeen = Date.now();
        devices[client.id].online = true;
    }
    save();
    console.log(`🟢 CONNECT clientId=${client.id}`);
}

function clientDisconnect(client) {
    // onlineDevices.delete(client.id);
    if (devices[client.id]) {
        devices[client.id].online = false;
        save();
    }
    console.log(`🔴 DISCONNECT clientId=${client.id}`);
}

function list() {
    return Object.entries(devices).map(([id, info]) => ({
        deviceId: id,
        created: info.created,
        lastSeen: info.lastSeen,
        online: info.online
    }));
}

module.exports = { load, save, clientConnect, clientDisconnect, list };