import React, { useEffect, useState } from 'react';
import { LineChart, Line, XAxis, YAxis, CartesianGrid, Tooltip, Legend } from 'recharts';

const GraphComponent = () => {
  const [data, setData] = useState([]);

  useEffect(() => {
    const fetchData = async () => {
      try {
        const response = await fetch('http://localhost:5000/api/data');
        if (!response.ok) {
          throw new Error(`HTTP error! Status: ${response.status}`);
        }
        const data = await response.json();
        console.log('Data received:', data);
        setData(data);
      } catch (error) {
        console.error('Error fetching data:', error);
      }
    };

    fetchData();
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
    </div>
  );
};

export default GraphComponent;
