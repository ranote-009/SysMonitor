// backend.js

const express = require('express');
const cors = require('cors');
const { exec } = require('child_process');
const app = express();
const port = 5000;

app.use(cors()); // Add this line to enable CORS
app.use(express.json());

app.post('/run-cpp', (req, res) => {
  // Command to run your C++ executable (without input parameters)
  const command = './test';

  // Execute the command
  exec(command, (error, stdout, stderr) => {
    if (error) {
      console.error(`Error executing C++ executable: ${error.message}`);
      res.status(500).send('Internal Server Error');
      return;
    }

    // Send the output back to the frontend
    res.json({ output: stdout });
  });
});

app.listen(port, () => {
  console.log(`Server is running at http://localhost:${port}`);
});
