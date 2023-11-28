// Example backend route to get data from MySQL
const express = require('express');
const mysql = require('mysql');
const cors = require('cors');

const app = express();
app.use(cors()); 

const connection = mysql.createConnection({
  host: 'localhost',
  user: 'root',
  password: '',
  database: 'sys_info',
  port: 3306
});

app.get('/api/data', (req, res) => {
  const query = 'SELECT * FROM information';
  connection.query(query, (err, results) => {
    if (err) throw err;
   // console.log(results);
    res.json(results);
    
  });
});

const port = 5000;
app.listen(port, () => console.log(`Server running on port ${port}`));
