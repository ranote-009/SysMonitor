import React, { createContext, useContext, useState } from 'react';

// Create a context with a default value
const AppContext = createContext();

// Create a provider component
export const AppProvider = ({ children }) => {
  const [clientoutput, setclientOutput] = useState('');
  const [serveroutput, setserverOutput] = useState('');
  const [connection, setConnection] = useState('');
  const [serverRunning, setServerRunning] = useState(false);
  const [clientRunning, setClientRunning] = useState(false);
  const [clientsConnected, setClientsConnected] = useState([]);
  const [clientsDisconnected, setClientsDisconnected] = useState([]);


  const contextValue = {
    clientoutput,
    setclientOutput,
    serveroutput,
    setserverOutput,
    connection,
    setConnection,
    serverRunning,
    setServerRunning,
    clientRunning, 
    setClientRunning,
    clientsConnected,
    setClientsConnected,
    clientsDisconnected,
    setClientsDisconnected,
  };

  return <AppContext.Provider value={contextValue}>{children}</AppContext.Provider>;
};

// Create a custom hook to consume the context
export const useAppContext = () => {
  const context = useContext(AppContext);
  if (!context) {
    throw new Error('useAppContext must be used within an AppProvider');
  }
  return context;
};
