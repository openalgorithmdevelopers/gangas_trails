const mqtt = require('mqtt');
const { Client } = require('pg');

const mqttClient = mqtt.connect('mqtt://localhost:1883');

// QuestDB connection
const db = new Client({
  host: 'localhost',
  port: 8812,
  user: 'admin',
  password: 'quest',
  database: 'qdb'
});

db.connect();

mqttClient.on('connect', () => {
  console.log('MQTT subscriber connected');
  mqttClient.subscribe('gps/data');
});

mqttClient.on('message', async (topic, message) => {
  try {
    const { client_id, timestamp, lat, lng } = JSON.parse(message.toString());

    const query = `
      INSERT INTO gps_data (client_id, lat, lng, ts)
      VALUES ($1, $2, $3, $4)
    `;

    await db.query(query, [client_id, lat, lng, timestamp]);
    console.log('Inserted:', client_id, timestamp);
  } catch (err) {
    console.error('Error:', err.message);
  }
});
