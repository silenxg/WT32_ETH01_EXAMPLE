# WT32-ETH01 board

You need Ethernet client that sends data to GraphQL API of the monitoring server with a query.

## Installation
Clone the repo and use platformio extension of VSCode to compile and upload the code.

## Libraries
* SoftTimer library to schedule the task of reading the state of the relays every 60 seconds.
* ArduinoJson library to create the JSON object that will be sent to the server.
* HTTPClient library to send the POST request to the server.
* ETH library to initialize the ethernet connection.
* WiFi library to initialize the WiFi connection.

## Library Dependencies
```text
https://github.com/prampec/arduino-softtimer.git#NoPci
arkhipenko/TaskScheduler@^3.7.0
bblanchon/ArduinoJson@^7.0.4
khoih-prog/WebServer_WT32_ETH01@^1.5.1
arduino-libraries/Ethernet@^2.0.2
```

## Contributing

Pull requests are welcome. For major changes, please open an issue first
to discuss what you would like to change.

Please make sure to update tests as appropriate.
