import { useState } from 'react';
import './App.css';
import Client from './client';
import Server from './server';
import GraphComponent from './graphComponent';
const App=()=> {
  const [showClient, setClient] = useState(false);
  const [showServer, setServer] = useState(true);
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
    <>
    <nav
      class="navbar navbar-expand-lg navbar-light fixed-top py-3"
      id="mainNav"
    >
      <div class="container px-4 px-lg-5">
        <a class="navbar-brand" href="#page-top">SysMonitor</a>
        <button
          class="navbar-toggler navbar-toggler-right"
          type="button"
          data-bs-toggle="collapse"
          data-bs-target="#navbarResponsive"
          aria-controls="navbarResponsive"
          aria-expanded="false"
          aria-label="Toggle navigation"
        >
          <span class="navbar-toggler-icon"></span>
        </button>
        <div class="collapse navbar-collapse" id="navbarResponsive">
          <ul class="navbar-nav ms-auto my-2 my-lg-0">
            {/* <li class="nav-item"><a class="nav-link" href="#about">SYSmonitor</a></li>  */}
            <li class="nav-item  navbutton" onClick={handleServer}>
              {/* <a class="nav-link" href="/startbootstrap-creative-gh-pages/server.html">Server</a> */}
              Server
            </li>
            <li class="nav-item navbutton" onClick={handleClient}>
              {/* <a class="nav-link" href="/startbootstrap-creative-gh-pages/client.html"> */}
                Client
                {/* </a> */}
            </li>
            <li class="nav-item navbutton" onClick={handleData}>
              Data Insights
            </li>
          </ul>
        </div>
      </div>
    </nav>
    {showClient && <Client />}
       {showServer && <Server/>}
       {showData && <GraphComponent/>}
    
   
   
    <footer class="bg-light py-5" id="data">
      <div class="container px-4 px-lg-5">
        <div class="small text-center text-muted">
          Copyright &copy; 2023 - CSGO
        </div>
      </div>
    </footer>
    </>
  );
}

export default App;
