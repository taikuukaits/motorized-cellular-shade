#define SOFTWARE_VERSION "0.0.2"

#define AVAILABILITY_TOPIC DEVICE_ID "/availability"

#define COVER_DISCOVERY_TOPIC "homeassistant/cover/" DEVICE_ID "/shade/config"
#define COVER_STATE_TOPIC DEVICE_ID "/shade/state"
#define COVER_COMMAND_TOPIC DEVICE_ID "/shade/command"
#define COVER_POSITION_STATE_TOPIC DEVICE_ID "/shade/position/state"
#define COVER_POSITION_COMMAND_TOPIC DEVICE_ID "/shade/position/command"

#define GREETINGS_TOPIC "motorized-cellular-shades/greetings"
#define CALIBRATE_POSITION_TOPIC DEVICE_ID "/calibrate/position"
#define CALIBRATE_CLOSED_POSITION_TOPIC DEVICE_ID "/calibrate/closed"
#define CALIBRATE_JOG_TOPIC DEVICE_ID "/calibrate/jog"

#define JSON_DOCUMENT_SIZE 1024
#define MQTT_PACKET_SIZE 512
#define DISCOVERY_RETAIN false

#define HELLO_PAYLOAD "__hello__"
#define ANNOUNCE_PAYLOAD "__announce__"
