void addDeviceInformation(JsonDocument& doc) {
  JsonObject device = doc.createNestedObject("device");
  device["identifiers"] = DEVICE_ID;
  device["sw_version"] = SOFTWARE_VERSION;
  device["name"] = DEVICE_NAME;
}

void sendCoverDiscovery(int position_open) {
  DynamicJsonDocument doc(JSON_DOCUMENT_SIZE);

  doc["avty_t"] = AVAILABILITY_TOPIC;
  doc["cmd_t"] = COVER_COMMAND_TOPIC;

  addDeviceInformation(doc);

  doc["device_class"] = "shade";
  doc["name"] = DEVICE_NAME " Shade";
  
  doc["stat_t"] = COVER_STATE_TOPIC;
  doc["unique_id"] = DEVICE_ID "-shade";

  doc["position_open"] = position_open;
  doc["position_topic"] = COVER_POSITION_STATE_TOPIC;
  doc["set_position_topic"] = COVER_POSITION_COMMAND_TOPIC;

  publishJson(COVER_DISCOVERY_TOPIC, doc, DISCOVERY_RETAIN);
}

void sendMaxStepsDiscovery() {
  DynamicJsonDocument doc(JSON_DOCUMENT_SIZE);

  doc["avty_t"] = AVAILABILITY_TOPIC;
  doc["cmd_t"] = MAX_STEPS_COMMAND_TOPIC;

  addDeviceInformation(doc);

  doc["entity_category"] = "config";
  doc["name"] = DEVICE_NAME " Max Steps";
  
  doc["stat_t"] = MAX_STEPS_STATE_TOPIC;
  doc["unique_id"] = DEVICE_ID "-max-steps";

  publishJson(MAX_STEPS_DISCOVERY_TOPIC, doc, DISCOVERY_RETAIN);
}
