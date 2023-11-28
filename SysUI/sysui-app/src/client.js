// App.js

import React, { useState } from 'react';

const App = () => {
  const [output, setOutput] = useState('');

  const runCpp = async () => {
    try {
    const eventSource = new EventSource('http://localhost:8000/run-cpp');

    eventSource.onmessage = (event) => {
      setOutput((prevOutput) => prevOutput + event.data);
    };

    eventSource.onerror = (error) => {
      console.error('Error with SSE:', error);
      eventSource.close();
    };

    // Cleanup the EventSource when the component unmounts
    return () => {
      eventSource.close();
    };
}
    catch (error) {
        console.error('Error:', error);
      }
  }; // Empty dependency array ensures the effect runs once when the component mounts

  return (
    <div>
      <h1>Client</h1>
      <button onClick={runCpp}>Run client</button>
      <pre>{output}</pre>
    </div>
  );
};

export default App;
