import React, { useState } from 'react';
import { useAppContext } from './AuthContext/AuthContext';
const App = () => {
  const { clientoutput, setclientOutput } = useAppContext();
  const {clientRunning, setClientRunning} = useAppContext();
 

  const runCpp = async () => {
    try {
      const eventSource = new EventSource('http://localhost:8000/run-cpp-client');

      eventSource.onmessage = (event) => {
        setclientOutput((prevOutput) => prevOutput + event.data);
      };

      eventSource.onerror = (error) => {
        console.error('Error with SSE:', error);
        eventSource.close();
        setClientRunning(false);
      };

      return () => {
        eventSource.close();
      };
    } catch (error) {
      console.error('Error:', error);
      setClientRunning(false);
    }
  };

 // Change the endpoint in your client code
const stopCpp = () => {
  fetch('http://localhost:8000/stop-cpp-client', { method: 'POST' })
    .then(response => response.json())
    .then(data => {
      console.log('Client stopped:', data);
      setClientRunning(false);
    })
    .catch(error => console.error('Error stopping client:', error));
};


  const handleWordDetection = (inputString) => {
    const targetWord = 'IS MORE THAN 90%';

    if (inputString.includes(targetWord)) {
      const emailSubject = 'Your Client has a Warning Usage';
      const emailText = `The word "${targetWord}" was found`;

      fetch('http://localhost:7000/send-email', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify({
          subject: emailSubject,
          text: emailText,
        }),
      })
        .then((response) => response.json())
        .then((data) => {
          if (data.success) {
            console.log('Email sent successfully');
          } else {
            console.error('Failed to send email');
          }
        })
        .catch((error) => {
          console.error('Error sending email:', error);
        });
    }
  };

  return (
    <div>
    {/* <h1>Client</h1> */}
  
  {/* <!-- Masthead--> */}
  <header class="servermasthead">
    <div class="container px-4 px-lg-5 h-100">
      <div
        class="row gx-4 gx-lg-5 h-100 align-items-center justify-content-center text-center"
      >
        <div class="col-lg-8 align-self-end">
          <h1 class="text-white font-weight-bold">
         Where Bytes Blossom into Brilliance: Welcome to the Heart of Innovation, Our Server Architecture.
          </h1>
          <hr class="divider" />
        </div>
        <div class="col-lg-8 align-self-baseline">
          <p class="text-white-75 mb-5">
            Press Button to Start/Stop Client.
          </p>
         {clientRunning ? (
           <button class="btn btn-primary btn-xl"  onClick={stopCpp}>Stop Client</button>
              ) : (
           <button class="btn btn-primary btn-xl" onClick={() => { runCpp(); setClientRunning(true); }}>Run Client</button>
            )}
          {/* <button onClick={runCpp}>Run client</button> */}
          {/* <a class="btn btn-primary btn-xl" href="#">Start</a> */}
          {/* <a class="btn btn-primary btn-xl" href="#">Stop</a> */}
        </div>
      </div>
    </div>
  </header>

  
  <pre>{clientoutput}</pre>
  {handleWordDetection(clientoutput)}
  </div>
    // <div>
    //   <h1>Client</h1>
    //   {clientRunning ? (
    //     <button onClick={stopCpp}>Stop Client</button>
    //   ) : (
    //     <button onClick={() => { runCpp(); setClientRunning(true); }}>Run Client</button>
    //   )}
    //   <pre>{clientoutput}</pre>
    //   {handleWordDetection(clientoutput)}
    // </div>
  );
};

export default App;
