import React, { useState } from 'react';

const App = () => {
  const [output, setOutput] = useState('');
  const [connection, setConnection] = useState('');


  const runCpp = async () => {
    try {
    const eventSource = new EventSource('http://localhost:9000/run-cpp');

    eventSource.onmessage = (event) => {
        const receivedData = event.data;

      // Check for the specific message and update UI accordingly
      if (receivedData.includes('Client connected:')) {
        const match = receivedData.match(/Client connected: (\d+\.\d+\.\d+\.\d+):(\d+)/);

        if (match) {
          const ipAddress = match[1];
          const portNumber = match[2];
          setConnection(`Connection established. IP: ${ipAddress}, Port: ${portNumber}\n`);
        }
       
      } 
        setOutput((prevOutput) => prevOutput + receivedData);
    }
    

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
      <h1>Server</h1>
      <button onClick={runCpp}>Run Server</button>
      <pre>{connection}</pre>
      <pre >{output}</pre>
      {/* style={{ whiteSpace: 'pre-wrap' }} */}
    </div>
  );
};

export default App;
