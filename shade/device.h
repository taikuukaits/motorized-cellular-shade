#define DEVICE_ID "bay-window-1"
#define DEVICE_NAME "Bay Window 1"
#define SOFTWARE_VERSION "0.0.1"
#define DEFAULT_MAX_STEPS 1000

#define LEFT_MOTOR_PIN_A 0
#define LEFT_MOTOR_PIN_B 0
#define LEFT_MOTOR_PIN_ENCODER 0

#define RIGHT_MOTOR_PIN_A 0
#define RIGHT_MOTOR_PIN_B 0
#define RIGHT_MOTOR_PIN_ENCODER 0

#define AVAILABILITY_TOPIC "homeassistant/" DEVICE_ID "/availability"
#define COVER_DISCOVERY_TOPIC "homeassistant/cover/" DEVICE_ID "/shade/config"
#define COVER_STATE_TOPIC DEVICE_ID "/state"
#define COVER_COMMAND_TOPIC DEVICE_ID "/command"
