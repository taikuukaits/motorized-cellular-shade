void publishJson(const char* topic, const JsonDocument& doc) {
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

  if (client.publish(topic, buffer, false)) {
    Serial.println("Message send OK.");
  } else {
    Serial.println("Message send FAILED.");
  }
}
