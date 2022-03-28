#define DEVICE_ID "test-window-1"
#define DEVICE_NAME "Test Window 1"

#define SOFTWARE_VERSION "0.0.1"

#define LEFT_MOTOR_PIN_A 0
#define LEFT_MOTOR_PIN_B 0
#define LEFT_MOTOR_PIN_ENCODER 0

#define RIGHT_MOTOR_PIN_A 0
#define RIGHT_MOTOR_PIN_B 0
#define RIGHT_MOTOR_PIN_ENCODER 0

#define AVAILABILITY_TOPIC DEVICE_ID "/availability"

#define COVER_DISCOVERY_TOPIC "homeassistant/cover/" DEVICE_ID "/shade/config"
#define COVER_STATE_TOPIC DEVICE_ID "/shade/state"
#define COVER_COMMAND_TOPIC DEVICE_ID "/shade/command"
#define COVER_POSITION_STATE_TOPIC DEVICE_ID "/shade/position/state"
#define COVER_POSITION_COMMAND_TOPIC DEVICE_ID "/shade/position/command"

#define CALIBRATE_HELLO_TOPIC "motorized-cellular-shades/calibrate/hello"
#define CALIBRATE_MAX_STEPS_TOPIC DEVICE_ID "/calibrate/max-steps"
#define CALIBRATE_POSITION_TOPIC "/calibrate/position"
#define CALIBRATE_LEFT_JOG_TOPIC DEVICE_ID "/calibrate/left-jog"
#define CALIBRATE_RIGHT_JOG_TOPIC DEVICE_ID "/calibrate/right-jog"
#define CALIBRATE_JOG_TOPIC DEVICE_ID "/calibrate/jog"

#define JSON_DOCUMENT_SIZE 1024
#define MQTT_PACKET_SIZE 512
#define DISCOVERY_RETAIN false
