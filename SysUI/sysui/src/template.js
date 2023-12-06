import React  from 'react';
import './styles.css';
import { useState } from 'react';
import Client from './client'
 
const App = () => {
const [showComponent, setShowComponent] = useState(false);

  const handleLinkClick = () => {
    // Update the state to show the component
    setShowComponent(true);
  };


  return (
    <div className="bg-gray-400 font-sans leading-normal tracking-normal">
      {/* Nav */}
      <nav className="bg-gray-800 p-2 mt-0 w-full">
        <div className="container mx-auto flex flex-wrap items-center">
          <div className="flex w-full md:w-1/2 justify-center md:justify-start text-white font-extrabold">
            <a className="text-white no-underline hover:text-white hover:no-underline" >
              <span className="text-2xl pl-2">
                SysMonitor
              </span>
            </a>
          </div>
          <div className="flex w-full pt-2 content-center justify-between md:w-1/2 md:justify-end">
            <ul className="list-reset flex justify-between flex-1 md:flex-none items-center">
              <li className="mr-3">
                <a className="inline-block py-2 px-4 text-white no-underline" onClick={handleLinkClick} >
                  Client 
                </a>
              </li>
              <li className="mr-3">
                <a
                  className="inline-block text-gray-600 no-underline hover:text-gray-200 hover:text-underline py-2 px-4"
                  
                >
                  Server
                </a>
              </li>
              <li className="mr-3">
                <a
                  className="inline-block text-gray-600 no-underline hover:text-gray-200 hover:text-underline py-2 px-4"
                  
                >
                  link
                </a>
              </li>
              <li className="mr-3">
                <a
                  className="inline-block text-gray-600 no-underline hover:text-gray-200 hover:text-underline py-2 px-4"
                  
                >
                  link
                </a>
              </li>
            </ul>
          </div>
        </div>
      </nav>

      {/* Container */}
      <div className="bg-white h-screen">
        <div className="container mx-auto pt-24 md:pt-16 px-6">
          <p className="py-4">
            {showComponent && <Client/>}
          </p>
        </div>
      </div>
    </div>
  );
};

export default App;
