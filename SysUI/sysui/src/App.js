import { useState } from 'react';
import './App.css';
import Client from './client';
import Server from './server';
import GraphComponent from './graphComponent';
const App=()=> {
  const [showClient, setClient] = useState(false);
  const [showServer, setServer] = useState(false);
  const [showData, setData] = useState(false);

  const handleClient = () => {
    // Update the state to show the component
    setClient(true);
    setServer(false);
    setData(false);
  };
  

  const handleServer = () => {
    // Update the state to show the component
    setClient(false);
    setServer(true);
    setData(false);
  };
  const handleData = () => {
    // Update the state to show the component
    setClient(false);
    setData(true);
    setServer(false);
  };


  return (
    <div >
      <div className="container text-left text-primary-emphasis">
       <div className="row">
       <div className="col-6 ">
        SysMonitor
       </div>
       <div className="col"  onClick={handleClient}>
       Client
       </div>
       <div className="col" onClick={handleServer}>
       Server
      </div>
      <div className="col" onClick={handleData}>
       Data Insights
      </div>
      </div>
      </div>
      {showClient && <Client />}
      {showServer && <Server/>}
      {showData && <GraphComponent/>}
      
      
    </div>
  );
}

export default App;
