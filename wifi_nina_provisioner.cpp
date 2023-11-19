#include "wifi_nina_provisioner.h"
#include <string>
#include <vector>


// Constructor with default parameters
WifiNinaProvisioner::WifiNinaProvisioner(const char* _ap_ssid, const char* _ap_pass)
    : ap_ssid(_ap_ssid), ap_pass(_ap_pass), status(WL_IDLE_STATUS), server(new WiFiServer(DEFAULT_WEB_SERVER_PORT)) {
    init();
}


void WifiNinaProvisioner::init() {
  status = WL_IDLE_STATUS;
  server = new WiFiServer(DEFAULT_WEB_SERVER_PORT);

  for (int i = 0; i < MAX_NETWORKS; i++) {
    networks[i] = "";
  }
}

String WifiNinaProvisioner::get_ssid() {
  return ssid;
}

String WifiNinaProvisioner::get_pass() {
  return pass;
}

String WifiNinaProvisioner::get_auth_key() {
  return auth_key;
}

void WifiNinaProvisioner::begin(bool connect) {
  scan_networks();
  Serial.println("");
  create_ap();
  Serial.println("");
  start_web_server();
  if (connect) {
    Serial.println("");
    connect_to_network();
  }
}

void WifiNinaProvisioner::scan_networks() {
  Serial.println("Scanning wifi networks...");

  int n_ssid = WiFi.scanNetworks();

  if (n_ssid == -1) {
    Serial.println("  Couldn't get a wifi connection");
  } else {
    Serial.println("  Network(s) found:");
    for (int i = 0; i < n_ssid && i < MAX_NETWORKS; i++) {
      networks[i] = WiFi.SSID(i);
      Serial.println("  - " + networks[i]);
    }
  }
}



// Improved Error Handling
bool WifiNinaProvisioner::create_ap() {
    Serial.println("Creating access point...");
    status = WiFi.beginAP(ap_ssid, ap_pass);

    if (status != WL_AP_LISTENING) {
        Serial.println("Creating access point failed.");
        return false;
    }
    delay(DELAY_AP_CREATION);
    Serial.println("Success.");
    return true;
}

void WifiNinaProvisioner::start_web_server() {
  bool brk = false;

  Serial.println("Starting Web Server...");
  server->begin();
  Serial.println("  Success.");
  print_ap_status();

  Serial.println("\n  Waiting for clients...");
  while (true) {
    WiFiClient client = server->available();

    if (client) {
      Serial.println("  New client connected.\n");
      String currentLine = "";
      String request = "";
      while (client.connected()) {
        if (client.available()) {
          char c = client.read();
          Serial.write(c);
          if (c == '\n') {
            request += currentLine + "\n";
            if (currentLine.length() == 0) { // Second empty line in a row => Process the request
              if (request.startsWith("GET /connect")) {
                get_network_credentials(request);
                print_html_connecting(client);
                brk = true;
              } else {
                Serial.println("  Display list of networks.");
                print_html_networks(client);
              }
              break;
            }
            else {
              currentLine = "";
            }
          }
          else if (c != '\r') {
            currentLine += c;
          }
        }
      }
      // close the connection:
      client.stop();
      Serial.println("\n  Client disconnected.");
      if (brk) {
        break;
      }
    }
  }
}


// Separating Concerns: HTML Generation
void WifiNinaProvisioner::print_html_networks(WiFiClient client) {
    generate_html(client);
}

void WifiNinaProvisioner::generate_html(WiFiClient& client) {
    // HTML generation code

    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.println();

    // HTML content with CSS for styling
    client.println("<!DOCTYPE html><html>");
    client.println("<head><title>WiFi Setup</title>");
    client.println("<meta name='viewport' content='width=device-width, initial-scale=1'>");
    client.println("<style>");
    client.println("body { font-family: Arial, sans-serif; margin: 0; padding: 0; background-color: #f2f2f2; color: #333; }");
    client.println("h2 { text-align: center; }");
    client.println("form { max-width: 300px; margin: 50px auto; padding: 20px; background: #fff; border-radius: 10px; }");
    client.println("label { margin-top: 10px; display: block; }");
    client.println("input[type='text'], select { font-size: 16px; width: 100%; padding: 10px; margin: 8px 0; display: inline-block; border: 1px solid #ccc; border-radius: 4px; box-sizing: border-box; }");
    client.println("input[type='submit'] { font-size: 16px; width: 100%; background-color: #4CAF50; color: white; padding: 14px 20px; margin: 8px 0; border: none; border-radius: 4px; cursor: pointer; }");
    client.println("input[type='submit']:hover { background-color: #45a049; }");
    client.println("</style>");
    client.println("</head>");

    client.println("<body>");

    client.println("<h2>Specify Network & Credentials</h2>");
    client.println("<form action='/connect' method='get'>");
    client.println("<label for='network'>Network:</label>");
    client.println("<select name='network' id='network'>");
    for (int i = 0; i < MAX_NETWORKS; i++) {
        if (networks[i] == "") {
            break;
        }
        client.print("<option value='" + networks[i] + "'>" + networks[i] + "</option>");
    }
    client.println("</select><br>");

    client.println("<label for='password'>Password:</label>");
    client.println("<input type='text' id='password' name='password'><br>");

    client.println("<label for='auth_key'>Auth/API Key:</label>");
    client.println("<input type='text' id='auth_key' name='auth_key'><br>");

    client.println("<input type='submit' value='Connect'>");
    client.println("</form>");
    client.println("</body></html>");

    // The HTTP response ends with another blank line:
    client.println();

}



void WifiNinaProvisioner::print_html_connecting(WiFiClient client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println();
  client.print("We will now connect to " + ssid + ".");
  client.println();
}



void WifiNinaProvisioner::get_network_credentials(String request) {
  // Extract the first line of the request
  request = request.substring(0, request.indexOf("\n"));

  // Helper function to extract a value from the request
  auto extractValue = [](const String& request, const String& key) {
    int start = request.indexOf(key);
    if (start == -1) return String(""); // Key not found
    start += key.length(); // Move start position past the key
    int end = request.indexOf('&', start);
    if (end == -1) end = request.length(); // Handle case where it's the last parameter
    return request.substring(start, end);
  };

  // Extract each parameter
  ssid = extractValue(request, "network=");
  pass = extractValue(request, "password=");
  auth_key = extractValue(request, "auth_key=");

  Serial.println("AP access information");
  Serial.println("  - SSID: " + ssid);
  Serial.println("  - PASS: " + pass);
  Serial.println("  - AUTH KEY: " + auth_key);
}


void WifiNinaProvisioner::connect_to_network() {
  Serial.println("Connecting to selected network...");

  while (status != WL_CONNECTED) {
    Serial.print("  Attempting to connect to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid.c_str(), pass.c_str());
    delay(DELAY_CONNECTION_ATTEMPTS);
  }
  print_network_status();
}

void WifiNinaProvisioner::store_credentials() {

  WiFiStorageFile ssid_file = WiFiStorage.open(SSID_FILE);
  WiFiStorageFile pass_file = WiFiStorage.open(PASS_FILE);
  WiFiStorageFile auth_key_file = WiFiStorage.open(AUTH_KEY_FILE);

if (auth_key_file) {
    auth_key_file.erase();
  }


  if (ssid_file) {
    ssid_file.erase();
  }

  if (pass_file) {
    pass_file.erase();
  }

  

  Serial.println("Saving to memory");
  auth_key_file.write(auth_key.c_str(), auth_key.length());
  Serial.println(ssid.c_str());

// This part is important!
   
    int bytesWritten = ssid_file.write(ssid.c_str(), ssid.length() + 1); // +1 for null terminator
    if (bytesWritten != ssid.length() + 1) {
      Serial.println("Failed to write complete SSID");
    }
    ssid_file.close();

  Serial.println(ssid.length());
  pass_file.write(pass.c_str(), pass.length());

}

void WifiNinaProvisioner::erase_credentials() {
  WiFiStorageFile ssid_file = WiFiStorage.open(SSID_FILE);
  WiFiStorageFile pass_file = WiFiStorage.open(PASS_FILE);
  WiFiStorageFile auth_key_file = WiFiStorage.open(AUTH_KEY_FILE);

  if (ssid_file) {
    ssid_file.erase();
  }
  ssid = "";

  if (pass_file) {
    pass_file.erase();
  }
  pass = "";

  if (auth_key_file) {
    auth_key_file.erase();
  }
  auth_key = "";
}

void WifiNinaProvisioner::retrieve_credentials() {
  WiFiStorageFile ssid_file = WiFiStorage.open(SSID_FILE);
  WiFiStorageFile pass_file = WiFiStorage.open(PASS_FILE);
  WiFiStorageFile auth_key_file = WiFiStorage.open(AUTH_KEY_FILE);


  char buf[128] = {0}; // Initialize buffer to zeros

  int l;


 if (ssid_file) {
    ssid_file.seek(0);
    int bytesRead = ssid_file.read(buf, sizeof(buf) - 1); // Leave space for null terminator
    buf[bytesRead] = '\0'; // Explicitly null-terminate the string
    ssid = buf;
    Serial.print("Buffer contains: ");
    Serial.println(buf); // This should print the entire SSID
  }



if (pass_file) {
  pass_file.seek(0);
  memset(buf, 0, sizeof(buf)); // Clear buffer
  l = pass_file.read(buf, sizeof(buf) - 1); // Read data, leave space for null terminator
  pass = buf; // Convert to String

}


  if (auth_key_file) {
    auth_key_file.seek(0);
    while (auth_key_file.available()) {
      l = auth_key_file.read(buf, 128);
      delay(2000);
    }
    auth_key = buf;
    auth_key = auth_key.substring(0, l);
  }
}

void WifiNinaProvisioner::print_ap_status() {
  Serial.print("  SSID: ");
  Serial.println(WiFi.SSID());

  IPAddress ip = WiFi.localIP();
  Serial.print("  Open a browser to http://");
  Serial.print(ip);
  Serial.println(" and choose the network you want to connect to.");
}

void WifiNinaProvisioner::print_network_status() {
  Serial.print("  You're connected to ");
  Serial.println(WiFi.SSID());

  IPAddress ip = WiFi.localIP();
  Serial.print("  IP Address: ");
  Serial.println(ip);

  long rssi = WiFi.RSSI();
  Serial.print("  Signal strength (RSSI): ");
  Serial.println(rssi);
}
