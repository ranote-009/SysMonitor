import React, { useEffect, useState } from 'react';
import { LineChart, Line, XAxis, YAxis, CartesianGrid, Tooltip, Legend } from 'recharts';

// hdd_utilization.pop_back();
// // Converting the string to an integer
//   int inthdd = std::stoi(hdd_utilization);
// int ramUsagePercentage = (stoi(ramUsage)*100) / stoi(ramUsage.substr(15,4));


const GraphComponent = () => {
  const [data, setData] = useState([]);

  const fetchData = async () => {
    try {
      const response = await fetch('http://localhost:8000/api/data');
      if (!response.ok) {
        throw new Error(`HTTP error! Status: ${response.status}`);
      }
      const newData = await response.json();
      console.log('Data received:', newData);
      setData(newData);
    } catch (error) {
      console.error('Error fetching data:', error);
    }
  };

  useEffect(() => {
    // Fetch data initially
    fetchData();

    // Fetch data every 5 seconds
    const intervalId = setInterval(() => {
      fetchData();
    }, 5000);

    // Cleanup interval on component unmount
    return () => clearInterval(intervalId);
  }, []);

  const lineColors = ['#8884d8', '#82ca9d', '#ffc658', '#ff7f50', '#6a5acd', '#ff1493'];

  const formatTimestamp = (timestamp) => {
    const date = new Date(timestamp);
    return `${date.getMonth() + 1}/${date.getDate()} ${date.getHours()}:${date.getMinutes()}`;
  };

  return (
    <div>
      <h4>Data Insights</h4>
      <LineChart
        width={800}
        height={600}
        data={data}
        margin={{ top: 5, right: 30, left: 20, bottom: 5 }}
      >
        <CartesianGrid strokeDasharray="3 3" />
        <XAxis dataKey="timestamp" tickFormatter={formatTimestamp} interval="preserveStartEnd"/>
        <YAxis domain={[0, 100]}/>
        <Tooltip />
        <Legend />

        {[...new Set(data.map(item => item.client_id))].map((clientId, index) => (
          <Line
            key={clientId}
            type="monotone"
            dataKey="cpuUsage"
            data={data.filter(item => item.client_id === clientId)}
            name={`Client ${clientId}`}
            stroke={lineColors[index % lineColors.length]}
          />
        ))}
      </LineChart>
      <LineChart
        width={800}
        height={600}
        data={data}
        margin={{ top: 5, right: 30, left: 20, bottom: 5 }}
      >
        <CartesianGrid strokeDasharray="3 3" />
        <XAxis dataKey="timestamp" tickFormatter={formatTimestamp} interval="preserveStartEnd"/>
        <YAxis domain={[0, 100]}/>
        <Tooltip />
        <Legend />

        {[...new Set(data.map(item => item.client_id))].map((clientId, index) => (
          <Line
            key={clientId}
            type="monotone"
            dataKey="ramUsage"
            data={data.filter(item => item.client_id === clientId)}
            name={`Client ${clientId}`}
            stroke={lineColors[index % lineColors.length]}
          />
        ))}
      </LineChart>
      <LineChart
        width={800}
        height={600}
        data={data}
        margin={{ top: 5, right: 30, left: 20, bottom: 5 }}
      >
        <CartesianGrid strokeDasharray="3 3" />
        <XAxis dataKey="timestamp" tickFormatter={formatTimestamp} interval="preserveStartEnd"/>
        <YAxis domain={[0, 100]}/>
        <Tooltip />
        <Legend />

        {[...new Set(data.map(item => item.client_id))].map((clientId, index) => (
          <Line
            key={clientId}
            type="monotone"
            dataKey="hdd_utilization"
            data={data.filter(item => item.client_id === clientId)}
            name={`Client ${clientId}`}
            stroke={lineColors[index % lineColors.length]}
          />
        ))}
      </LineChart>
    </div>
  );
};

export default GraphComponent;
