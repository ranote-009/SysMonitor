const express = require('express');
const cors = require('cors');
const { spawn } = require('child_process');

const app = express();
const port = 9000;

app.use(cors()); // Add this line to enable CORS
app.use(express.json());

app.get('/run-cpp', (req, res) => {
  const command = '/home/abhsihek/SysMonitor/SysMonitor/Server/build/server';

  // Use spawn to create a child process
  const childProcess = spawn(command);

  // Set up SSE
  res.setHeader('Content-Type', 'text/event-stream');
  res.setHeader('Cache-Control', 'no-cache');
  res.setHeader('Connection', 'keep-alive');

  // Send output to the client
//   childProcess.stdout.on('data', (data) => {
//     res.write(`data: ${data}\n\n`);
//   });


let accumulatedData = '';

  // Send output to the client
  childProcess.stdout.on('data', (data) => {
    accumulatedData += data.toString();
    
    // Check for newline character to send a chunk
    if (accumulatedData.includes('\n')) {
      const chunks = accumulatedData.split('\n');
      accumulatedData = chunks.pop(); // Save incomplete line for the next iteration

      chunks.forEach(chunk => {
        res.write(`data: ${chunk}\n\n`);
      });
    }
  });
  // Send errors to the client
  childProcess.stderr.on('data', (data) => {
    res.write(`error: ${data}\n\n`);
  });

  // Send the process exit code when the process ends
  childProcess.on('close', (code) => {
    res.write(`event: close\ndata: Process exited with code ${code}\n\n`);
    res.end();
  });
});

app.listen(port, () => {
  console.log(`Server is running at http://localhost:${port}`);
});
