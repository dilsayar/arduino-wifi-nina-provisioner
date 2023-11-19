
# Wifi Nina Provisioner Library

The Wifi Nina Provisioner library offers a streamlined and user-friendly process for setting up and managing WiFi connections on Arduino devices compatible with Wifi Nina. This library is ideal for projects requiring remote WiFi network configuration, particularly in IoT applications.

## Easily Configure Your Wifi Credentials
Instead of hardcoding your Wifi credentials in your code, you can use this library to pass network credentials to your Arduino Nano RP2040 or other similar devices. 

## Standard Provisioning Workflow

This library simplifies the WiFi setup process into a few easy steps:

1. **Access Point Initialization**: The Arduino device is configured as an access point and runs a web server.
2. **Connection Establishment**: Connect to this access point using a computer or mobile device.
3. **Web Interface Access**: Navigate to the web interface at `http://192.168.4.1` using any web browser.
4. **Credential Submission**: Enter the necessary information:
   - The WiFi network name (SSID) for the Arduino to connect to.
   - The corresponding password for the WiFi network.
   - An Auth/API key, if integrating with IoT cloud services like [Blynk](https://www.blynk.io).
5. **Network Connection Attempt**: The Arduino tries to establish a connection with the specified WiFi network.

### Basic Usage

Here's how you can initiate the standard workflow:

```cpp
#include "wifi_nina_provisioner.h"

WifiNinaProvisioning wp;
wp.begin(true); // Automatically connects to the provided network
```

By default, the access point credentials are:
- **SSID**: wifi-provisioner
- **Password**: 123456789

## Customization Options

### Custom Access Point Credentials

To customize the SSID and password for the access point, pass them as parameters to the constructor:

```cpp
#include "wifi_nina_provisioner.h"

WifiNinaProvisioning wp("my_custom_ssid", "my_secure_password");
wp.begin(true);
```

**Note**: The password should be at least 8 characters long for security reasons.

### Manual WiFi Network Connection

If automatic connection to the WiFi network is not desired, use `wp.begin(false)`. This is useful for scenarios where you need to manage the connection process manually or integrate with other IoT libraries. Retrieve the provided credentials using `get_ssid()`, `get_pass()`, and `get_auth_key()`.

## Persistent Credential Storage

To enhance user experience, especially after device reboots or resets, the library supports persistent storage of WiFi credentials:

- **Storing Credentials**: After initial setup, use `store_credentials()` to save the user-provided credentials in WifiNina's permanent storage.
- **Retrieving Credentials**: Use `retrieve_credentials()` to load previously stored credentials. Access them with `get_ssid()`, `get_pass()`, and `get_auth_key()`.
- **Erasing Credentials**: Remove stored credentials anytime using `erase_credentials()`.

## Example and Demonstration

Refer to the included _basic_ example sketch to explore a comprehensive demonstration of the library's capabilities. This example provides insights into implementing the library in your Arduino projects for effective WiFi management.


## Thanks
This project has been adapted from [Guillaume Deflaux's code](https://gitlab.com/gdeflaux/wifi-nina-provisioning). I updated the UI, refactored the code and fixed some bugs related to retrieval and saving of credentials. 
