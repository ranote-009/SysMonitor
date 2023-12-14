// backend-mysql.js
const express = require('express');
const mysql = require('mysql');
const cors = require('cors');

const app = express();
app.use(cors());

const connection = mysql.createConnection({
  host: 'localhost',
  user: 'root',
  password: '',
  database: 'system_info',
  port: 3306
});

app.get('/api/data', (req, res) => {
  const query = 'SELECT * FROM systems_information';
  connection.query(query, (err, results) => {
    if (err) throw err;
    res.json(results);
  });
});

module.exports = app; // Export the Express app as middleware
