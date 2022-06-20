//SmartMailBox -- new version
//Author -- Alexandr Waage
//Date -- 20/6/2022

#include <Arduino.h>
#if defined(ESP32)
  #include <WiFi.H>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <ESP_Mail_Client.h>

//Connect to WiFi network
#define WIFI_SSID "ssid" //name of network
#define WIFI_PASSWORD "heslo" //network password

//Some credentials about email
#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT 465

//The sign in credentials
#define AUTHOR_EMAIL "example@gmail.com"
#define AUTHOR_PASSWORD "123456"

//Recipient's email
#define RECIPIENT_EMAIL "recipient@gmail.com"

//The SMTP session object used for email sending
SMTPSession smtp;

//Callback function to get the email sending status
void smtpCallback(SMTP_Status status);

//Function for sending email
void sendEmail(String zprava);

void setup() {
  pinMode(1, INPUT);

  Serial.begin(115200);
  //Connecting to network
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(200);
  }
  //Showing the IP adress
  Serial.println("");
  Serial.println("WiFi is connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  //enable debug with Email library, for no debug use "0"
  smtp.debug(1);

  //set the callback function to get the sending resutls
  smtp.callback(smtpCallback);

  //Declare the session config data
  ESP_Mail_Session session;
  session.server.host_name = SMTP_HOST;
  session.server.port = SMTP_PORT;
  session.login.email = AUTHOR_EMAIL;
  session.login.password = AUTHOR_PASSWORD;
  session.login.user_domain = "";

  //declare the message class
  SMTP_Message message;
  message.sender.name = "SmartMailBox";
  message.sender.email = AUTHOR_EMAIL;
  message.subject = "Your MailBox is full"
  message.addRecipient("Me", RECIPIENT_EMAIL);

  //Connect to server with the session config
  if (!smtp.connect(&session))
    return;
}

void loop(){
  //Variable for sensor controller
  static int sensor = 1; //pin


}

//Function send mail
void sendMail(String zprava){
  
  //sending html message
  String htmlMsg = "<div style=\"color:#2f4468;\"><h1>Your MailBox is full</h1></br><h3>Please pick me up...</h3><p>- Sent from SmartMailBox</p></div>";
  message.html.content = htmlMsg.c_str();
  message.text.charSet = "us-ascii";
  message.html.transfer_encoding = Content_Transfer_Encoding::enc_7bit;

  //Start sending email and close the session
  if (!MailClient.sendMail(&smtp, &message))
    Serial.println("Error sending Email, " + smtp.errorReason());
}

//Callback function to get the Email sending status
void smtpCallback(SMTP_Status status){
  /* Print the current status */
  Serial.println(status.info());

  /* Print the sending result */
  if (status.success()){
    Serial.println("----------------");
    ESP_MAIL_PRINTF("Message sent success: %d\n", status.completedCount());
    ESP_MAIL_PRINTF("Message sent failled: %d\n", status.failedCount());
    Serial.println("----------------\n");
    struct tm dt;

    for (size_t i = 0; i < smtp.sendingResult.size(); i++){
      /* Get the result item */
      SMTP_Result result = smtp.sendingResult.getItem(i);
      time_t ts = (time_t)result.timestamp;
      localtime_r(&ts, &dt);

      ESP_MAIL_PRINTF("Message No: %d\n", i + 1);
      ESP_MAIL_PRINTF("Status: %s\n", result.completed ? "success" : "failed");
      ESP_MAIL_PRINTF("Date/Time: %d/%d/%d %d:%d:%d\n", dt.tm_year + 1900, dt.tm_mon + 1, dt.tm_mday, dt.tm_hour, dt.tm_min, dt.tm_sec);
      ESP_MAIL_PRINTF("Recipient: %s\n", result.recipients);
      ESP_MAIL_PRINTF("Subject: %s\n", result.subject);
    }
    Serial.println("----------------\n");
  }
}