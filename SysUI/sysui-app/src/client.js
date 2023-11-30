// App.js

import React, { useState } from 'react';

const App = () => {
  const [output, setOutput] = useState('');
  // Inside your React component
const handleWordDetection = (inputString) => {
  const targetWord = 'IS MORE THAN 90%'; // Replace with the word you're looking for

  if (inputString.includes(targetWord)) {
    const emailSubject = 'Target Word Found';
    const emailText = `The word "${targetWord}" was found in the string: ${inputString}`;

    // Make an HTTP request to your server
    fetch('http://localhost:7000/send-email', {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
      },
      body: JSON.stringify({
        subject: emailSubject,
        text: emailText,
        to: 'ranoteabhishek@yahoo.com', // Replace with the recipient's email
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
      {handleWordDetection(output)}
    </div>
  );
};

export default App;
