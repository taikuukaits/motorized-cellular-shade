void connect() {
   wifiConnect();
   mqttConnect();
   arduinoOtaConnect();
}

void wifiConnect(){
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  Serial.print("Connecting to Wi-Fi...");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" ok");

  WiFi.setAutoReconnect(true);
  WiFi.persistent(false);

  Serial.println("Successfully connected to " WIFI_SSID);
}

void mqttConnect() {
  client.setServer(MQTT_SERVER, MQTT_PORT);
  Serial.print("Connecting to MQTT...");

  while (!client.connected()) {
    if (client.connect(DEVICE_ID, MQTT_USER, MQTT_PASS, AVAILABILITY_TOPIC, 2, true, "offline")) {
      Serial.println(" ok");
    } else {
      delay(2000);
      Serial.print(".");
    }
  }

  Serial.println("Successfully connected to " MQTT_SERVER);
}

void arduinoOtaConnect(){
  ArduinoOTA.setHostname(DEVICE_ID " v" SOFTWARE_VERSION);
  
  ArduinoOTA.onStart([]() {
    Serial.println("Start updating...");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
}
