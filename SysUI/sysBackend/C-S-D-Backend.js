// index.js
const express = require('express');
const clientApp = require('./Client-Backend.js');
const serverApp = require('./Server-Backend.js');
const mysqlApp = require('./Database-Backend.js');

const app = express();
const port = 8000;

// Use the client, server, and MySQL apps as middleware
app.use(clientApp);
app.use(serverApp);
app.use(mysqlApp);

app.listen(port, () => {
  console.log(`Combined server is running at http://localhost:${port}`);
});
