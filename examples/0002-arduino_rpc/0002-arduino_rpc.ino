// This sketch demonstrates connecting and receiving RPC calls from 
// ThingsBoard using ThingsBoard SDK.
//
// Hardware:
//  - Arduino Uno
//  - ESP8266 connected to Arduino Uno

#include "ThingsBoard.h"

#include <WiFi.h>


#define WIFI_AP             "YOUR_WIFI_AP"
#define WIFI_PASSWORD       "YOUR_WIFI_PASSWORD"

// See https://thingsboard.io/docs/getting-started-guides/helloworld/
// to understand how to obtain an access token
#define TOKEN               "YOUR_ACCESS_TOKEN"
#define THINGSBOARD_SERVER  "demo.thingsboard.io"

// Initialize the Ethernet client object
WiFiClient espClient;
// Initialize ThingsBoard instance
ThingsBoard tb(espClient);

void setup() {
  // initialize serial for debugging
  Serial.begin(9600);
}
// Processes function for RPC call "example_set_temperature"
// RPC_Data is a JSON variant, that can be queried using operator[]
// See https://arduinojson.org/v5/api/jsonvariant/subscript/ for more details
RPC_Response processTemperatureChange(const RPC_Data &data)
{
  Serial.println("Received the set temperature RPC method");

  // Process data

  float example_temperature = data["temp"];

  Serial.print("Example temperature: ");
  Serial.println(example_temperature);

  // Just an response example
  return RPC_Response("example_response", 42);
}

// Processes function for RPC call "example_set_switch"
// RPC_Data is a JSON variant, that can be queried using operator[]
// See https://arduinojson.org/v5/api/jsonvariant/subscript/ for more details
RPC_Response processSwitchChange(const RPC_Data &data)
{
  Serial.println("Received the set switch method");

  // Process data

  bool switch_state = data["switch"];

  Serial.print("Example switch state: ");
  Serial.println(switch_state);

  // Just an response example
  return RPC_Response("example_response", 22.02);
}

std::vector<RPC_Callback> callbacks = {
  { "example_set_temperature",    processTemperatureChange },
  { "example_set_switch",         processSwitchChange }
};

void loop() {
  delay(100);

  if (WiFi.status() == WL_IDLE_STATUS) {
    // Still connecting
    return;
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connecting to AP ...");
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(WIFI_AP);
    // Connect to WPA/WPA2 network
    WiFi.begin(WIFI_AP, WIFI_PASSWORD);
    return;
  }

  if (!tb.connected()) {
    // Connect to the ThingsBoard
    Serial.print("Connecting to: ");
    Serial.print(THINGSBOARD_SERVER);
    Serial.print(" with token ");
    Serial.println(TOKEN);
    if (!tb.connect(THINGSBOARD_SERVER, TOKEN)) {
      Serial.println("Failed to connect, retrying ...");
      return;
    }
  }

  if (!tb.RPC_Subscribed()) {
    Serial.println("Subscribing for RPC...");

    // Perform a subscription. All consequent data processing will happen in
    // processTemperatureChange() and processSwitchChange() functions,
    // as denoted by callbacks[] array.
    if (!tb.RPC_Subscribe(callbacks)) {
      Serial.println("Failed to subscribe for RPC");
      return;
    }

    Serial.println("Subscribe done");
  }

  Serial.println("Waiting for data...");
  tb.loop();
}