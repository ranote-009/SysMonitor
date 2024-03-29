// backend-client.js
const express = require('express');
const cors = require('cors');
const { spawn } = require('child_process');

const app = express();
const port = 8000;

app.use(cors());
app.use(express.json());

app.get('/run-cpp-client', (req, res) => {
  const command = '/home/abhsihek/SysMonitor/SysMonitor/Client/build/client';

  const childProcess = spawn(command);

  res.setHeader('Content-Type', 'text/event-stream');
  res.setHeader('Cache-Control', 'no-cache');
  res.setHeader('Connection', 'keep-alive');

  let accumulatedData = '';

  childProcess.stdout.on('data', (data) => {
    accumulatedData += data.toString();

    if (accumulatedData.includes('\n')) {
      const chunks = accumulatedData.split('\n');
      accumulatedData = chunks.pop();

      chunks.forEach(chunk => {
        res.write(`data: ${chunk}\n\n`);
      });
    }
  });

  childProcess.stderr.on('data', (data) => {
    res.write(`error: ${data}\n\n`);
  });

  childProcess.on('close', (code) => {
    res.write(`event: close\ndata: Process exited with code ${code}\n\n`);
    res.end();
  });
});

app.post('/stop-cpp-client', (req, res) => {
  // Implement the logic to stop the server using your terminal command
  const stopCommand = 'for pid in $(pgrep -f "/home/abhsihek/SysMonitor/SysMonitor/Client/build/client"); do kill -2 "$pid"; done';

  const childProcess = spawn(stopCommand, { shell: true });

  childProcess.on('close', (code) => {
    res.json({ success: true, code });
  });
});

module.exports = app; // Export the Express app as middleware
