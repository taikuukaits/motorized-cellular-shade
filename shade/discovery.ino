void sendDiscovery() {
    sendCoverDiscovery();
}

void sendCoverDiscovery() {
  String topic = COVER_DISCOVERY_TOPIC;

  DynamicJsonDocument doc(1024);
  char buffer[256];

  doc["avty_t"] = AVAILABILITY_TOPIC;
  doc["command_topic"] = COVER_COMMAND_TOPIC;
  doc["cmd_t"] = DEVICE_ID "/command";

  JsonObject device = doc.createNestedObject();
  device["identifiers"] = DEVICE_ID;
  device["sw_version"] = SOFTWARE_VERSION;
  device["name"] = DEVICE_NAME;

  doc["device_class"] = "shade";
  doc["name"] = DEVICE_NAME " Shade";
  
  doc["stat_t"] = COVER_STATE_TOPIC;
  doc["unique_id"] = DEVICE_ID "-shade";

  size_t n = serializeJson(doc, buffer);
  client.publish(topic.c_str(), buffer, n);
}
