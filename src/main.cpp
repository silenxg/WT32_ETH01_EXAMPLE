/**
 * @file main.cpp
 * @author Spyros Gogos (admsilen@gmail.com)
 * @brief This is an example of a simple ethernet client that sends data to the GraphQL API of the monitoring server.
 * we are using the WT32-ETH01 board. It is based on the ESP32 module and has an ethernet port and WiFi.
 * We are using the SoftTimer library to schedule the task of reading the state of the relays every 60 seconds.
 * We are using the ArduinoJson library to create the JSON object that will be sent to the server.
 * We are using the HTTPClient library to send the POST request to the server.
 * We are using the ETH library to initialize the ethernet connection.
 * We are using the WiFi library to initialize the WiFi connection.
 * We are reading the state of 5 relays and sending the data to the server.
 * @version 1.0
 * @date 2024-10-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */

//We set the ETH PHY type to LAN8720 and the pins for the MDC and MDIO before we include the libraries.
#define ETH_PHY_TYPE        ETH_PHY_LAN8720
#define ETH_PHY_MDC         23
#define ETH_PHY_MDIO        18
#define ETH_CLK_MODE        ETH_CLOCK_GPIO0_IN

//We include the necessary libraries.
#include <Arduino.h>
#include <ETH.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <SPI.h>
#include <SoftTimer.h>
#include <ArduinoJson.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

// Mac configuration
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
// IPAddress configuration
IPAddress ip(192, 168, 1, 8);
// IPAddress gateway
IPAddress gateway(192, 168, 1, 1);
// IPAddress subnet
IPAddress subnet(255, 255, 255, 0);

//We define the client and the host.
HTTPClient http;
String host = "http://<ip address>:<port>/graphql";

void readState(int pin, int sensorid);
//Function to initialize the ethernet connection.
void ETH_init()
{
  Serial.println("Connecting to ethernet...");
  ETH.begin(ETH_PHY_ADDR, ETH_PHY_POWER, ETH_PHY_MDC, ETH_PHY_MDIO, ETH_PHY_TYPE, ETH_CLK_MODE);
  ETH.macAddress(mac);
  ETH.config(ip, gateway , subnet);
}
//Task to read the state of the relays every 60 seconds.
Task readRelays(60000, [](Task *me)
                { readState(IO39, 1), readState(IO36, 2), readState(IO15, 3), readState(IO14, 4), readState(IO12, 5);});

void setup()
{
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); // disable brownout detector for module
  Serial.begin(115200);
  while (!Serial && millis() < 5000)
    ;
  ETH_init();

  // Wait for Ethernet connection
  while (!ETH.linkUp())
  {
    delay(100);
    Serial.println("Waiting for Ethernet connection...");
  }
  Serial.print("Connected! IP address: ");
  Serial.println(ETH.localIP());

  pinMode(IO39, INPUT_PULLUP);
  pinMode(IO36, INPUT_PULLUP);
  pinMode(IO15, INPUT_PULLDOWN);
  pinMode(IO14, INPUT_PULLDOWN);
  pinMode(IO12, INPUT_PULLDOWN);

  // Add the task to the timer
  SoftTimer.add(&readRelays);
}

void readState(int pin, int sensorid)
{

  int state = digitalRead(pin);

  // Create a JSON object
  JsonDocument doc;

  // Add graphql query in the JSON object
  doc["query"] = "mutation CreateEntry($entryInput: EntryCreate!) { createEntry(entryInput: $entryInput) { _id address sensor value }}";
  JsonObject variables_entryInput = doc["variables"]["entryInput"].to<JsonObject>();
  variables_entryInput["address"] = "0013a20040050000";
  variables_entryInput["sensor"] = sensorid;
  variables_entryInput["value"] = state;
  doc.shrinkToFit(); // optional

  // Convert JSON object to string
  String queryString;

  // Serialize the JSON object to a string
  serializeJson(doc, queryString);

  // Print the JSON object to the Serial monitor
  Serial.println(queryString);

  // Send the POST request
  if (ETH.linkUp())
  {
    Serial.println("Connecting to GraphQL API...");
    int conn = http.begin(host);
    if (conn)
    {
      Serial.println("Connection establised");
    }
    else
    {
      Serial.println("Connection error");
    }
    http.addHeader("Content-Type", "application/json");

    int httpResponseCode = http.POST(queryString);

    // Check the HTTP response code
    if (httpResponseCode == 200)
    {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);
    }
    else
    {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }
    // End the connection
    http.end();
  }
  else
  {
    Serial.println("Error in Eth connection");
  }
}