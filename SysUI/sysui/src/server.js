import React, { useState } from 'react';
import { useAppContext } from './AuthContext/AuthContext';
const App = () => {
  const {
    serveroutput,
    setserverOutput,
    connection,
    setConnection,
    serverRunning,
    setServerRunning,
    clientsConnected,
    setClientsConnected,
    clientsDisconnected,
    setClientsDisconnected,
  } = useAppContext();
// New state variable

  const runCpp = async () => {
    try {
      const eventSource = new EventSource('http://localhost:8000/run-cpp-server');

      eventSource.onmessage = (event) => {
        const receivedData = event.data;

        // Check for the specific message and update UI accordingly
        if (receivedData.includes('Client connected:')) {
          const match = receivedData.match(/Client connected: (\d+\.\d+\.\d+\.\d+):(\d+)/);

          if (match) {
            const ipAddress = match[1];
            const portNumber = match[2];
            setConnection(`Connection established. IP: ${ipAddress}, Port: ${portNumber}\n`);
            setClientsConnected(prevClients => [...prevClients, `Connection established. IP: ${ipAddress}, Port: ${portNumber}`]);
          }

        } else if (receivedData.includes('client is disconnected:')) {
          const match = receivedData.match(/client is disconnected:\s+(\w{2}:\w{2}:\w{2}:\w{2}:\w{2}:\w{2})\s+(\w{2}:\w{2}:\w{2}:\w{2}:\w{2}:\w{2})/);
        
          if (match) {
            const macAddress1 = match[1];
            const macAddress2 = match[2];
            setConnection(`Connection disconnected. MAC Addresses: ${macAddress1}, ${macAddress2}\n`);
            setClientsDisconnected(prevClients => [...prevClients, `Connection disconnected. MAC Addresses: ${macAddress1} ${macAddress2}`]);
          }
        }
        setserverOutput((prevOutput) => prevOutput + receivedData);
      }

      eventSource.onerror = (error) => {
        console.error('Error with SSE:', error);
        eventSource.close();
        setServerRunning(false); // Update state when an error occurs
      };

      // Cleanup the EventSource when the component unmounts
      return () => {
        eventSource.close();
      };
    } catch (error) {
      console.error('Error:', error);
      setServerRunning(false); // Update state when an error occurs
    }
  };

  const stopCpp = () => {
    fetch('http://localhost:8000/stop-cpp-server', { method: 'POST' })
      .then(response => response.json())
      .then(data => {
        console.log('Server stopped:', data);
        setServerRunning(false); // Update state when the server is stopped
      })
      .catch(error => {
        console.error('Error stopping server:', error);
        setServerRunning(false); // Update state when an error occurs
      });
  };

  return (
    <div>
    {/* <h1>Server</h1> */}
    {/* <button onClick={runCpp}>Run Server</button> */}
     
      <header class="servermasthead">
            <div class="container px-4 px-lg-5 h-100">
              <div
                class="row gx-4 gx-lg-5 h-100 align-items-center justify-content-center text-center"
              >
                <div class="col-lg-8 align-self-end">
                  <h1 class="text-white font-weight-bold fonttext">
                Where Bytes Blossom into Brilliance: Welcome to the Heart of Innovation, Our Server Architecture.
                  </h1>
                  <hr class="divider" />
                </div>
                <div class="col-lg-8 align-self-baseline">
                  <p class="text-white-75 mb-5">
                    Press Button to Start/Stop Server.
                  </p>
                  <button class="btn btn-primary btn-xl" onClick={() => { runCpp(); setServerRunning(true); }} disabled={serverRunning}>
                  Start Server
                 </button>
                 <button class="btn btn-primary btn-xl" onClick={stopCpp} disabled={!serverRunning}>
                  Stop Server
                  </button>
                  {/* <a class="btn btn-primary btn-xl" href="#">Start</a> */}
                  {/* <a class="btn btn-primary btn-xl" href="#">Stop</a> */}
                </div>
              </div>
            </div>
          </header>
          <pre>{connection}</pre>
      <pre>{serveroutput}</pre>
      <div>
       <p>Clients Connected</p>
       <ul>
       {clientsConnected.map((client, index) => (
            <li key={index}>{client}</li>
          ))}
      </ul>
      </div>
      <div>
        <p>Clients Disconnected</p>
        <ul>
          {clientsDisconnected.map((client, index) => (
          <li key={index}>{client}</li>
          ))}
        </ul>
      </div>
    {/* style={{ whiteSpace: 'pre-wrap' }} */}
  </div>
    // <div>
    //   <h1>Server</h1>
    //   <button class="btn btn-primary btn-xl" onClick={() => { runCpp(); setServerRunning(true); }} disabled={serverRunning}>
    //     Start Server
    //   </button>
    //   <button class="btn btn-primary btn-xl" onClick={stopCpp} disabled={!serverRunning}>
    //     Stop Server
    //   </button>
    //   <pre>{connection}</pre>
    //   <pre>{serveroutput}</pre>
    //   <div>
    //     <p>Clients Connected</p>
    //     <ul>
    //       {clientsConnected.map((client, index) => (
    //         <li key={index}>{client}</li>
    //       ))}
    //     </ul>
    //   </div>
    //   <div>
    //     <p>Clients Disconnected</p>
    //     <ul>
    //       {clientsDisconnected.map((client, index) => (
    //         <li key={index}>{client}</li>
    //       ))}
    //     </ul>
    //   </div>
    // </div>
  );
};

export default App;
