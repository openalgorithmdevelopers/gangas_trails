const express = require('express');
const { Client } = require('pg');

const app = express();
const PORT = 3000;

// QuestDB connection
const db = new Client({
  host: 'localhost',
  port: 8812,
  user: 'admin',
  password: 'quest',
  database: 'qdb'
});

db.connect();

// Serve static files
app.use(express.static('public'));

// API to get latest GPS position
app.get('/api/latest', async (req, res) => {
  try {
    const result = await db.query(`
      SELECT client_id, lat, lng, ts
      FROM gps_data
      WHERE client_id = 'ganga_tracker_001'  -- <--- ADD THIS FILTER
      ORDER BY ts DESC
      LIMIT 1
    `);
    
    if (result.rows.length === 0) {
        return res.status(404).json({ error: "No data found for Ganga Tracker" });
    }
    res.json(result.rows[0]);
  } catch (err) {
    res.status(500).json({ error: err.message });
  }
});

app.listen(PORT, () =>
  console.log(`Server running → http://localhost:${PORT}`)
);
