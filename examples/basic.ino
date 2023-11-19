// Arduino Nano RP2040 with Wifi Prov
#include <WiFiNINA.h>
#include <SPI.h>
#include "wifi_nina_provisioner.h"


WiFiClient client;
int status = WL_IDLE_STATUS;

WiFiServer server(80);

void setup() {
  Serial.begin(9600);
  while (!Serial) {
  }


  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // Stop here if no WiFi module is present.
    while (true)
      ;
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  WifiNinaProvisioner wp;

  // Attempt to retrieve stored credentials
  wp.retrieve_credentials();

  Serial.println("SSID and Pass from memory");
  Serial.println(wp.get_ssid());
  Serial.println(wp.get_pass());

  String ssidString = wp.get_ssid();
  String passString = wp.get_pass();

  // Access Point SSID: wifi-provisioner
  // Access Point Password: 123456789
  // You open a browser to http://192.168.4.1.


  if (ssidString.length() > 0 && passString.length() > 0) {
    Serial.println("Using stored credentials:");

    status = WiFi.begin(ssidString.c_str(), passString.c_str());
    delay(15000);

    if (status != WL_CONNECTED) {
      Serial.println("Failed to connect with stored credentials.");
      // Proceed to provisioning if connection failed
      wp.begin(true);
      wp.store_credentials();
    }
  } else {
    Serial.println("No stored credentials. Starting provisioning...");
    wp.begin(true);
    wp.store_credentials();
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Connected to WiFi");
    printWifiStatus();
  } else {
    Serial.println("Not connected to WiFi");
  }


  server.begin();

  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);

}

void loop() {


  WiFiClient client = server.available();   // listen for incoming clients


  if (client) {                             // if you get a client,
    Serial.println("new client");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
    if (currentLine.length() == 0) {
    // HTTP headers
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.println();

    // HTML content
    client.println("<!DOCTYPE html><html>");
    client.println("<head><title>Arduino Control</title>");
    client.println("<meta name='viewport' content='width=device-width, initial-scale=1.0'>");
    client.println("<style>");
    client.println("body { font-family: Arial, sans-serif; margin: 0; padding: 0; background-color: #121212; color: #fff; }");
    client.println("h2 { text-align: center; }");
    client.println("a { background-color: #007BFF; color: white; padding: 10px 15px; text-decoration: none; border-radius: 5px; display: inline-block; }");
    client.println("a:hover { background-color: #0056b3; }");
    client.println("@media (max-width: 600px) { body { font-size: 18px; } }");
    client.println("</style>");
    client.println("</head>");
    client.println("<body>");
    client.println("<h2>Arduino LED Control</h2>");
    client.println("<div style='text-align: center;'>");
    client.println("<p><a href=\"/H\">Turn the LED on</a></p>");
    client.println("<p><a href=\"/L\">Turn the LED off</a></p>");
    client.println("</div>");
    client.println("</body></html>");

    // The HTTP response ends with another blank line:
    client.println();
    // Break out of the while loop:
    break;
}


          else {      // if you got a newline, then clear currentLine:
            currentLine = "";
          }

        }

        else if (c != '\r') {    // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine

        }

        // Check to see if the client request was "GET /H" or "GET /L":

        if (currentLine.endsWith("GET /H")) {
          digitalWrite(LED_BUILTIN, HIGH);
                    // GET /H turns the LED on

        }

        if (currentLine.endsWith("GET /L")) {
          digitalWrite(LED_BUILTIN, LOW);                // GET /L turns the LED off
         

        }

      }

    }

    // close the connection:
    client.stop();
    Serial.println("client disconnected");

  }




}




void printWifiStatus() {

  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
