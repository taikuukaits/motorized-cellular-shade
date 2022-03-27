#define DEVICE_ID "test-window-1"
#define DEVICE_NAME "Test Window 1"
#define DEFAULT_MAX_STEPS 1000

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

#define MAX_STEPS_DISCOVERY_TOPIC "homeassistant/number/" DEVICE_ID "/max-steps/config"
#define MAX_STEPS_STATE_TOPIC DEVICE_ID "/max-steps/state"
#define MAX_STEPS_COMMAND_TOPIC DEVICE_ID "/max-steps/command"

#define JSON_DOCUMENT_SIZE 1024
#define MQTT_PACKET_SIZE 512
#define DISCOVERY_RETAIN false
