void publishJson(const char* topic, const JsonDocument& doc, bool retain) {
  Serial.print("Sending json document to ");
  Serial.println(topic);
  
  char buffer[MQTT_PACKET_SIZE];
  size_t n = serializeJson(doc, buffer);

  Serial.print("Message size is ");
  Serial.print(n);
  Serial.print(" and buffer size ");
  Serial.print(client.getBufferSize());

  if (n < client.getBufferSize()) {
    Serial.println(" which is OK.");  
  } else {
    Serial.println(" which is WRONG.");  
  }

  if (client.publish(topic, buffer, retain)) {
    Serial.println("Message send OK.");
  } else {
    Serial.println("Message send FAILED.");
    Serial.println(client.state());
  }
}

void publishInt(const char* topic, int value, bool retain) {
  char cstr[16];
  itoa(value, cstr, 10);
  
  if (client.publish(topic, cstr, retain)) {
    Serial.println("Message send OK.");
  } else {
    Serial.println("Message send FAILED.");
  }
}
