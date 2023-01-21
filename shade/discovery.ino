void addDeviceInformation(JsonDocument& doc) {
  JsonObject device = doc.createNestedObject("device");
  device["ids"] = DEVICE_ID; //identifiers
  device["sw"] = SOFTWARE_VERSION; //sw_version
  device["name"] = DEVICE_NAME;
}

void sendCoverDiscovery(int position_open) {
  DynamicJsonDocument doc(JSON_DOCUMENT_SIZE);

  doc["avty_t"] = AVAILABILITY_TOPIC;
  doc["cmd_t"] = COVER_COMMAND_TOPIC;

  addDeviceInformation(doc);

  doc["dev_cla"] = "shade"; //device_class
  doc["name"] = DEVICE_NAME " Shade";
  
  doc["stat_t"] = COVER_STATE_TOPIC;
  doc["uniq_id"] = DEVICE_ID "-shade"; //unique_id

  doc["position_open"] = position_open;
  doc["position_topic"] = COVER_POSITION_STATE_TOPIC;
  doc["set_position_topic"] = COVER_POSITION_COMMAND_TOPIC;

  publishJson(COVER_DISCOVERY_TOPIC, doc, DISCOVERY_RETAIN);
}
