const mqtt = require("./mqtt");
const push = require("./push");
const devices = require("./devices");
const api = require("./api");

// Load persistent data
devices.load();
push.load();

// Start MQTT broker
const aedes = mqtt.start(devices, push);

// Start REST API and inject dependencies
api.start(push, devices, aedes);