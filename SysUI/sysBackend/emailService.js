const express = require('express');
const nodemailer = require('nodemailer');
const bodyParser = require('body-parser');
const cors = require('cors');



const app = express();
app.use(cors());
const port = 7000;
app.use(bodyParser.json());

const transporter = nodemailer.createTransport({
  service: 'yahoo',
  auth: {
    user: 'ranoteabhishek@yahoo.com',
    pass: '',
  },
});

app.post('/send-email', async (req, res) => {
  const { subject, text, to } = req.body;

  try {
    const mailOptions = {
      from: 'ranoteabhishek@yahoo.com',
      to: to,
      subject: subject,
      text: text,
    };

    const info = await transporter.sendMail(mailOptions);
    console.log('Email sent:', info.response);

    res.status(200).json({ success: true });
  } catch (error) {
    console.error('Error sending email:', error);
    res.status(500).json({ success: false, error: 'Internal Server Error' });
  }
});

app.listen(port, () => {
  console.log(`Server is running on port ${port}`);
});
