// React component (App.js)

import React, { useState } from 'react';
import Client from './client.js';
import Server from './server.js';
import GraphComponent from './graphComponent.js';

const App = () => {
  const [output, setOutput] = useState('');

  const runCpp = async () => {
    try {
      const response = await fetch('http://localhost:5000/run-cpp', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
      });

      if (!response.ok) {
        throw new Error(`HTTP error! Status: ${response.status}`);
      }

      const result = await response.json();
      setOutput(result.output);
    } catch (error) {
      console.error('Error:', error);
    }
  };

  return (
    <div>
      <h1>React Frontend</h1>
      <button onClick={runCpp}>Run API</button>
      <p>Output from backend: {output}</p>
      <Client/>
      <Server/>
      <GraphComponent/>
    </div>
  );
};

export default App;
