const fs = require("fs");
const path = require("path");
const webPush = require("web-push");
const PUSH_FILE = path.join(__dirname, "../data/pushSubscriptions.json");
const vapidKeys = require("../config/vapid.json");

// Configure web-push with VAPID keys
webPush.setVapidDetails("mailto:admin@phimai.net", vapidKeys.publicKey, vapidKeys.privateKey);

const subscriptions = new Map();

// Load push subscriptions from JSON file
function load() {
    try {
        const data = fs.readFileSync(PUSH_FILE, "utf8");
        const obj = JSON.parse(data);
        for (const [endpoint, sub] of Object.entries(obj)) {
            subscriptions.set(endpoint, sub);
        }
    } catch {
        // empty
    }
}

// Save push subscriptions to JSON file
function save() {
    fs.writeFileSync(PUSH_FILE, JSON.stringify(Object.fromEntries(subscriptions), null, 2));
}

// Register a new push subscription
function register(sub) {
    if (!sub?.endpoint) return false;
    subscriptions.set(sub.endpoint, { ...sub, created: Date.now(), lastUsed: Date.now() });
    save();
    console.log("Push registered:", sub.endpoint);
    return true;
}

// Unregister a push subscription
function unregister(sub) {
    if (!sub?.endpoint) return false;
    subscriptions.delete(sub.endpoint);
    save();
    console.log("Push unregistered:", sub.endpoint);
    return true;
}

// Send a push notification to all registered subscriptions
async function sendNotification(message) {
    console.log(`🔔 Sending push notification`);

    for (const [endpoint, sub] of subscriptions) {
        try {
            await webPush.sendNotification(sub, JSON.stringify(message));
            sub.lastUsed = Date.now();
        } catch (err) {
            if (err.statusCode === 404 || err.statusCode === 410) {
                subscriptions.delete(endpoint);
                console.log("🧹 Removing expired push:", endpoint);
            } else {
                console.error("❌ Push error:", err.message);
            }
        }
    }
    save();
}

module.exports = { load, save, register, unregister, sendNotification };