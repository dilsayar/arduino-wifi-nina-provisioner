/*
  wifi_nina_provisioner.h - Library for passing Wifi credentials to Arduino Nano RP2040 and other similar devices
  Created by Hüseyin Demirtaş, November 19, 2023.
  Personel website: https://huseyindemirtas.net/
  Inspired by Guillaume Deflaux's code at https://gitlab.com/gdeflaux/wifi-nina-provisioning
*/


#ifndef _WIFI_NINA_PROVISIONER_
#define _WIFI_NINA_PROVISIONER_

#include <WiFiNINA.h>
#include <array>

constexpr auto DEFAULT_SSID = "wifi-provisioner";
constexpr auto DEFAULT_PASS = "123456789";
constexpr int DEFAULT_WEB_SERVER_PORT = 80;
constexpr size_t MAX_NETWORKS = 20;

#define DELAY_AP_CREATION         10000
#define DELAY_CONNECTION_ATTEMPTS 10000
#define SSID_FILE                 "/fs/ssid"
#define PASS_FILE                 "/fs/pass"
#define AUTH_KEY_FILE             "/fs/auth_key"

class WifiNinaProvisioner {
  private:
    const char* ap_ssid;
    const char* ap_pass;
    String      ssid;
    String      pass;
    String      auth_key;
    int         status;
    WiFiServer  *server;
    String      networks[MAX_NETWORKS];


  public:
    explicit WifiNinaProvisioner(const char* _ap_ssid = DEFAULT_SSID, const char* _ap_pass = DEFAULT_PASS);
    void begin(bool connect);
    String get_ssid();
    String get_pass();
    String get_auth_key();
    void store_credentials();
    void retrieve_credentials();
    void erase_credentials();
    void generate_html(WiFiClient& client); // Corrected declaration



  private:
    void init();
    void scan_networks();
    bool create_ap();
    void start_web_server();
    void connect_to_network();

    void print_html_networks(WiFiClient client);
    void print_html_connecting(WiFiClient client);
    void get_network_credentials(String request);
  

    void print_ap_status();
    void print_network_status();
};

#endif
