# NodeMCU Internet Outage Detector
by: Alex - Super Make Something
date: July 29, 2020

NodeMCU code and setup instructions for the internet outage detector shown [here](https://www.instagram.com/p/CDNDC7rDyOi).  
If you need to pick up a NodeMCU for this build, please consider showing your support for this project by purchasing one through this Amazon Affiliate link: https://amzn.to/2X7RK5m

## Introduction
With COVID-19 making telework the new norm, having access to the internet is extremely important.  Unfortunately, some ISPs are better than others, which may eventually make you file a complaint with an organization like the FCC to try to resolve your connectivity issues and improve your internet service.  Having data about outages helps you to make a better case that your ISP is not delivering the services you are paying for, which is why I created a tiny internet outage detector using a NodeMCU microcontroller.

The code simply pings Google approximately every 30 seconds (using the delay() function), and fetches the real-world time that it was able to last connect to the site from an NTP server.  If internet connectivity is interrupted, the last time that the NodeMCU was able to sucessfully ping Google is logged, after which the NodeMCU will continue to try to reach Google until it is able to.  Once the NodeMCU successfully pings Google again, it sends an e-mail with the last successful connection time and the time that internet connectivity was restored to the recipient specified in the code through the SMTP2GO service.  (Instructions about how to set up a free SMTP2GO account and how to encode your username and password to the correct format to use with the code can be found (here)[https://www.electronicshub.org/send-an-email-using-esp8266/].)  If no outage occurs within approximately 24 hours (2880 loops through the code, once every ~30 seconds), the NodeMCU will send an e-mail to the user letting them know that there were no outages within that timeframe.

## Installation instructions and required libraries
To program the NodeMCU using the Arduino IDE, the following link must be added to the "Additional Boards Manager URLs" field under the "Preferences" Menu: http://arduino.esp8266.com/stable/package_esp8266com_index.json.  After this, install `esp8266` platform support using the "Boards Manager" in the Arduino IDE.  (Installation instructions (here)[https://www.instructables.com/id/Quick-Start-to-Nodemcu-ESP8266-on-Arduino-IDE/].)

The code uses the following libraries:
- (ESP8266 Arduino Core)[https://arduino-esp8266.readthedocs.io/en/latest/index.html]: Installed via the "Library Manager" in the Arduino IDE. Installation instructions (here)[https://arduino-esp8266.readthedocs.io/en/latest/installing.html].
- (ESP8266Ping)[https://github.com/dancol90/ESP8266Ping]: Place the unzipped folder into the `Arduino -> libraries` directory.
- (NTPClient)[https://github.com/taranais/NTPClient]: Unzip the file, rename the folder `NTPClient-master` to `NTPClient` and place it the `Arduino -> libraries` directory.

## Configuration instructions
Replace the following items in the code to match your setup:
- `const char* ssid`: the SSID of your WiFi network.
- `const char* password`: your WiFi network password.
- `const String url`: the website to ping.
- `const String ENCODED_USERNAME`: encoded SMTP2GO username.
- `const String ENCODED_PASSWORD`: encoded SMTP2GO password.
- Recipient e-mail address in lines 216 and 228.
- Sender e-mail address in lines 210 and 231.
 
