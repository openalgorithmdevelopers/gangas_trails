const mqtt = require('mqtt');
const client = mqtt.connect('mqtt://broker.emqx.io:1883');
const path = [
  // Devprayag (Start - Confluence of Alaknanda & Bhagirathi)
  [30.1460, 78.5975],
  [30.1380, 78.5880],
  [30.1250, 78.5750],
  [30.1150, 78.5650],
  [30.1020, 78.5580],
  [30.0880, 78.5480],
  [30.0780, 78.5410],
  
  // Vyas Ghat area
  [30.0750, 78.5350],
  [30.0720, 78.5250],
  [30.0700, 78.5150],
  
  // Kaudiyala (Major Rafting Point)
  [30.0680, 78.5050],
  [30.0650, 78.4900],
  [30.0620, 78.4750],
  [30.0600, 78.4600],
  
  // Byasi
  [30.0600, 78.4450],
  [30.0650, 78.4350],
  [30.0780, 78.4280],
  
  // Marine Drive stretch
  [30.0880, 78.4250],
  [30.1050, 78.4180],
  [30.1200, 78.4080],
  
  // Shivpuri
  [30.1350, 78.3880],
  [30.1420, 78.3750],
  [30.1380, 78.3650],
  
  // Brahmpuri
  [30.1330, 78.3550],
  [30.1300, 78.3450],
  
  // Lakshman Jhula / Tapovan
  [30.1230, 78.3300],
  [30.1180, 78.3220],
  [30.1120, 78.3150],
  
  // Rishikesh (Triveni Ghat / End)
  [30.1030, 78.2950]
];

let index = 0;

client.on('connect', () => {
  console.log('Ganga GPS simulator started');

  setInterval(() => {
    if (index >= path.length) {
      console.log('End of river segment reached');
      return;
    }

    const [lat, lng] = path[index];

    const msg = {
      client_id: 'ganga_tracker_001',
      timestamp: new Date().toISOString(),
      lat,
      lng
    };

    client.publish('gps/data', JSON.stringify(msg));
    console.log('Published:', msg);

    index++;
  }, 5000);
});
