/* NodeMCU Internet Outage Detector Code - https://www.instagram.com/p/CDNDC7rDyOi
 * by: Alex - Super Make Something
 * date: July 29th, 2020
 * license: Creative Commons - Attribution - Non-Commercial.  More information available at: http://creativecommons.org/licenses/by-nc/3.0/
 */

#include <ESP8266WiFi.h>
#include <ESP8266Ping.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

const char* ssid = "SuperMakeSomething";
const char* password = "hunter2";

const IPAddress remote_ip(8,8,8,8); // Remote host (8.8.8.8 is Google.com)

const String ENCODED_USERNAME = "ENCODED_USERNAME";
const String ENCODED_PASSWORD = "ENCODED_PASSWORD";

char server[] = "mail.smtp2go.com"; // The SMTP server

const long utcOffsetInSeconds = -4*60*60; // UTC offset for EDT

//Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

int iterationCounter = 0;

char outages[2][20]; // Array to log last connection and service restoration times

WiFiClient client;

 
void setup() 
{
       Serial.begin(9600);
       delay(10);
               
       Serial.println("Connecting to ");
       Serial.println(ssid); 
 
       WiFi.begin(ssid, password); 
       while (WiFi.status() != WL_CONNECTED) 
          {
            delay(500);
            Serial.print(".");
          }
      Serial.println("");
      Serial.println("WiFi connected");
      Serial.print("IP address:\t");
      Serial.println(WiFi.localIP()); // Send the IP address of the ESP8266 to the computer

      timeClient.begin(); // Initialize time client
      
      pinMode(LED_BUILTIN, OUTPUT);
      digitalWrite(LED_BUILTIN, LOW); // Light up LED when connected to SSID

      sendEmail(0,outages); // Send initialization e-mail
}


void loop() 
{ 
  timeClient.update(); // Get current time     
  Serial.print("Pinging IP:\t");
  Serial.println(remote_ip);
  bool ret = Ping.ping(remote_ip, 10);
  
  if (ret == true)
  {
    Serial.print("Sucessful ping to ");
    Serial.print(remote_ip);
    Serial.print(" on ");

    String outageString = "";
    outageString += timeClient.getFormattedDate();
    
    Serial.println(outageString);
    
    char char_array[20];
    outageString.toCharArray(char_array, 20); // Save formatted date-time to char Array
    for (int i = 0; i<20; i++) // Store last successful connection time
    {
      outages[0][i] = char_array[i];
    }
    
    if (iterationCounter>2880) // Send e-mail approximately every 24 hours
    {
      if (sendEmail(1,outages) == 1) // Successfully sent e-mail, no outages in 24 hours
      {
        iterationCounter = 0;// Reset iterationCounter after successful e-mail
      }
    }

    delay(30000); // Wait for 30 seconds
  }
  
  else // Ping not successful
  {
    while(ret == false) // While ping is unsucessful
    {
      Serial.print("Unable to reach ");
      Serial.print(remote_ip);
      Serial.print(" on ");
      Serial.println(timeClient.getFormattedDate());
      delay(30000); // Wait for 30 seconds
      Serial.println("Trying again...");
      ret = Ping.ping(remote_ip, 10);
    } // Exit while loop when ret == truel

    timeClient.update(); // Get current time
    Serial.print("Connection restored on ");

    String outageString = "";
    outageString += timeClient.getFormattedDate();
    
    Serial.println(outageString);

    Serial.println("Sending status e-mail.");
    
    if (sendEmail(2,outages) == 1) // Successfully sent e-mail, outage
    {
      iterationCounter = 0;// Reset iterationCounter after successful e-mail

      char char_array[20];
      outageString.toCharArray(char_array, 20); // Save formatted date-time to char Array
      for (int i = 0; i<20; i++) // Store last successful connection time
      {
        outages[0][i] = char_array[i];
      }    
    }
   
    delay(30000); // Wait for 30 seconds 
  }

  // DEBUG
  /*
  String outageString = "";
  outageString += timeClient.getFormattedDate();
  char char_array[20];
  outageString.toCharArray(char_array, 20); // Save formatted date-time to char Array
  for (int i = 0; i<20; i++) // Store last successful connection time
  {
    outages[0][i] = char_array[i];
  }
  timeClient.update(); // Get current time
  outageString = "";
  outageString += timeClient.getFormattedDate();
  outageString.toCharArray(char_array, 20); // Save formatted date-time to char Array
  for (int i = 0; i<20; i++) // Store last successful connection time
  {
    outages[1][i] = char_array[i];
  }
  sendEmail(1,outages); // Test no outages e-mail
  sendEmail(2,outages); // Test outages e-mail
  delay(3000000); // hold;   
  */
  
  iterationCounter++; // Increment iterationCounter
}


byte sendEmail(int flag, char outage[2][20])
{
  if (client.connect(server, 2525) == 1)
  {
    Serial.println(F("connected"));
  }
  else
  {
    Serial.println(F("connection failed"));
    return 0;
  }

  if (!emailResp()) // Check if e-mail is sendable throughout method
    return 0;

  Serial.println(F("Sending EHLO"));
  client.println("EHLO www.example.com");

  if (!emailResp())
    return 0;

  Serial.println(F("Sending auth login"));
  client.println("AUTH LOGIN");

  if (!emailResp())
    return 0;

  Serial.println(F("Sending User"));
  client.println(ENCODED_USERNAME); // Your encoded Username

  if (!emailResp())
    return 0;

  Serial.println(F("Sending Password"));
  client.println(ENCODED_PASSWORD);// Your encoded Password

  if (!emailResp())
    return 0;

  Serial.println(F("Sending From"));
  client.println(F("MAIL From: efg@xyz.com")); // Enter Sender Mail Id

  if (!emailResp())
    return 0;

  Serial.println(F("Sending To"));
  client.println(F("RCPT To: abc@xyz.com")); // Enter Receiver Mail Id

  if (!emailResp())
    return 0;

  Serial.println(F("Sending DATA"));
  client.println(F("DATA"));

  if (!emailResp())
    return 0;

  Serial.println(F("Sending email"));
  client.println(F("To:  abc@xyz.com")); // Enter Receiver Mail Id

  // change to your address
  client.println(F("From: efg@xyz.com")); // Enter Sender Mail Id

  if (flag == 0) 
  {
    client.println(F("Subject: INITIALIZED - NodeMCU Internet Outage Monitor\r\n"));
    client.println(F("The NodeMCU internet outage monitor successfully booted.  This is either because of an intentional reboot or a power outage.\n"));
    client.println(F("Continuing to monitor internet status."));
  }
  else if (flag == 1)
  {
    client.println(F("Subject: ALL CLEAR - NodeMCU Internet Outage Summary\r\n"));
    client.println(F("The NodeMCU internet outage monitor has not detected any outages between the following times:\n"));
    for (int i = 0; i<2; i++)
    {
      client.println(outage[i]);
    } 
    client.println(F("\nContinuing to monitor internet status."));
  }
  else
  {
    client.println(F("Subject: OUTAGE DETECTED - NodeMCU Internet Outage Summary\r\n"));
    client.println(F("The NodeMCU internet outage monitor detected an outage between the following times:\n"));
    for (int i = 0; i<2; i++)
    {
      client.println(outage[i]);
    } 
    client.println(F("\nContinuing to monitor internet status."));
  }

  client.println(F("."));
  if (!emailResp())
    return 0;

  Serial.println(F("Sending QUIT"));
  client.println(F("QUIT"));

  if (!emailResp())
    return 0;

  client.stop();
  Serial.println(F("disconnected"));
  return 1;
}


byte emailResp()
{
  byte responseCode;
  byte readByte;
  int loopCount = 0;

  while (!client.available())
  {
    delay(1);
    loopCount++;
    if (loopCount > 20000)
    {
      client.stop();
      Serial.println(F("\r\nTimeout"));
      return 0;
    }
  }

  responseCode = client.peek();
  while (client.available())
  {
    readByte = client.read();
    Serial.write(readByte);
  }

  if (responseCode >= '4')
  {
    return 0;
  }
  
  return 1;
}
