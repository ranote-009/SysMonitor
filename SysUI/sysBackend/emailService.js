
// const bodyParser = require('body-parser');
// const cors = require('cors');




// app.use(cors());
// app.use(bodyParser.json());


// app.post('/send-email', async (req, res) => {
//   const { subject, text, to } = req.body;

//   try {
//     const mailOptions = {
//       from: 'ranoteabhishek@yahoo.com',
//       to: to,
//       subject: subject,
//       text: text,
//     };

//     const info = await transporter.sendMail(mailOptions);
//     console.log('Email sent:', info.response);

//     res.status(200).json({ success: true });
//   } catch (error) {
//     console.error('Error sending email:', error);
//     res.status(500).json({ success: false, error: 'Internal Server Error' });
//   }
// });

// app.listen(port, () => {
//   console.log(`Server is running on port ${port}`);
// });


// const nodemailer = require('nodemailer');
// const { google } = require('googleapis');

// // These id's and secrets should come from .env file.
// const CLIENT_ID = '48092452045-f7kgdm3mbknb5749hqjrhlg0u05d4jqh.apps.googleusercontent.com';
// const CLEINT_SECRET = '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';
// const REDIRECT_URI = 'https://developers.google.com/oauthplayground';
// const REFRESH_TOKEN = '1//04ewIBBmiBxaSCgYIARAAGAQSNwF-L9Irwou5-TRvkEN-ZX4vpkR0N0zudrJp1txNuaJA84qmaoW_dJToL4ZFAJOBBymUuCw26z4';

// const oAuth2Client = new google.auth.OAuth2(
//   CLIENT_ID,
//   CLEINT_SECRET,
//   REDIRECT_URI
// );
// oAuth2Client.setCredentials({ refresh_token: REFRESH_TOKEN });

// async function sendMail() {
//   try {
//     const accessToken = await oAuth2Client.getAccessToken();

//     const transport = nodemailer.createTransport({
//       service: 'gmail',
//       auth: {
//         type: 'OAuth2',
//         user: 'ranoteabhishek123@gmail.com',
//         clientId: CLIENT_ID,
//         clientSecret: CLEINT_SECRET,
//         refreshToken: REFRESH_TOKEN,
//         accessToken: accessToken,
//       },
//     });

//     const mailOptions = {
//       from: 'Abhishek <ranoteabhishek123@gmail.com>',
//       to: 'ranoteabhishek123@gmail.com',
//       subject: 'Your Client has a Warning Usage',
//       text: 'Your Client has exceeded the limit usage please check them immediately',
//       html: '<h1>Your Client has exceeded the limit usage please check them immediately</h1>',
//     };

//     const result = await transport.sendMail(mailOptions);
//     return result;
//   } catch (error) {
//     return error;
//   }
// }

// sendMail()
//   .then((result) => console.log('Email sent...', result))
//   .catch((error) => console.log(error.message));


const express = require('express');
const nodemailer = require('nodemailer');
const { google } = require('googleapis');
 const bodyParser = require('body-parser');
const cors = require('cors');






const app = express();
const port = 7000;
 app.use(cors());
app.use(bodyParser.json());

// These id's and secrets should come from .env file.
const CLIENT_ID = '48092452045-f7kgdm3mbknb5749hqjrhlg0u05d4jqh.apps.googleusercontent.com';
const CLEINT_SECRET = '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';
const REDIRECT_URI = 'https://developers.google.com/oauthplayground';
const REFRESH_TOKEN = '1//04ewIBBmiBxaSCgYIARAAGAQSNwF-L9Irwou5-TRvkEN-ZX4vpkR0N0zudrJp1txNuaJA84qmaoW_dJToL4ZFAJOBBymUuCw26z4';

const oAuth2Client = new google.auth.OAuth2(
  CLIENT_ID,
  CLEINT_SECRET,
  REDIRECT_URI
);
oAuth2Client.setCredentials({ refresh_token: REFRESH_TOKEN });

app.post('/send-email', async (req, res) => {
  const { subject, text} = req.body;
  try {
    const accessToken = await oAuth2Client.getAccessToken();

    const transport = nodemailer.createTransport({
      service: 'gmail',
      auth: {
        type: 'OAuth2',
        user: 'ranoteabhishek123@gmail.com',
        clientId: CLIENT_ID,
        clientSecret: CLEINT_SECRET,
        refreshToken: REFRESH_TOKEN,
        accessToken: accessToken,
      },
    });

    const mailOptions = {
      from: 'Abhishek <ranoteabhishek123@gmail.com>',
      to: 'ranoteabhishek123@gmail.com',
      subject: subject,
      text: text,
      html: '<h1>Your Client has exceeded the limit usage. Please check them immediately.</h1>',
    };

    const result = await transport.sendMail(mailOptions);
    console.log('Email sent:', result);

    res.status(200).json({ success: true, message: 'Email sent successfully' });
  } catch (error) {
    console.error('Error sending email:', error);
    res.status(500).json({ success: false, error: 'Internal Server Error' });
  }
});

app.listen(port, () => {
  console.log(`Server is running on port ${port}`);
});
