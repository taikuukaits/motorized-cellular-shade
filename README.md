# motorized-cellular-shade
Motorized cellular shades using ESPHome and cellular shade blinds from SelectBlinds.com

These blinds were among the cheapest (yet still nice) I could find for my blinds. They still ran me $87 per window for 16 1/4" W x 61" H shades. 

This project uses the [Endstop Cover](https://esphome.io/components/cover/endstop/) of ESPHome.

# Getting Started

There are several steps needed to make your own shades. 

1. You will need to build the physical components inside the shade
2. You will need to wire the electronic components
3. You will need to upload the code to the electronics
4. You will need to mount the shade and get the electronics power

This guide will only cover hardwiring the shades to 12V DC. Battery or solar is not currently supported. 

This will require Home Assistant with two add-ons, ESPHome and ESPHome Device Builder.
You will need to adopt the device into Home Assistant yourself.

# Electronics

The ESPHome code assumes the following wiring:
```
D1 to B-1A L9110
D2 to B-2A L9110
D6 to End Stop 1
D5 to End Stop 2
Both End Stops connect to GND.
```
NOTE: It's recommended that D6 represent UP or CLOSED or you will need to modify the YAML file.

This is recommended for power: 
```
5V on D1 mini to Buck Converter Out +
G on D1 mini to Buck Converter Out -
VCC on L9110 to Buck Converter In +
GND on L9110 to Buck Converter In -
Buck Converter In + to 12V Source
Buck Converter In - to 12V Source
```

# ESPHome

The complete YAML file is checked into source under examples, for a short explanation continue reading. This YAML was written with the help of AI.

Each of the two endstops are binary sensors in ESPHome
```
binary_sensor:
  - platform: gpio
    id: endstop_open
    name: "Endstop Open"
    pin:
      number: D6
      mode: INPUT_PULLUP
      inverted: true
    filters:
      - delayed_on: 20ms
      - delayed_off: 20ms

  - platform: gpio
    id: endstop_closed
    name: "Endstop Closed"
    pin:
      number: D5
      mode: INPUT_PULLUP
      inverted: true
    filters:
      - delayed_on: 20ms
      - delayed_off: 20ms
```
They are INPUT_PULLUP because we connect them to GND. They are inverted because when pressed they are shorted to GND (LOW when PRESSED)

The motor is driven by two switches:
```
switch:
  - platform: gpio
    id: motor_in1
    pin: D1
    restore_mode: ALWAYS_OFF

  - platform: gpio
    id: motor_in2
    pin: D2
    restore_mode: ALWAYS_OFF
```
These follow the [drivers table](https://docs.sunfounder.com/projects/3in1-kit-v2/en/latest/components/component_l9110_module.html) to control them, so HIGH on B1 and LOW on B2 sends the B motor in one direction and LOW on B1 and HIGH on B2 sends it in the other direction. LOW on both is stop.


The scripts control the motor, setting these pins as appropriate for the desired action. When we want the motor to open, one is high and the other low.
```
script:
  - id: motor_stop
    then:
      - switch.turn_off: motor_in1
      - switch.turn_off: motor_in2

  # OPEN direction
  - id: motor_open
    then:
      - script.execute: motor_stop
      - switch.turn_on: motor_in1
      - switch.turn_off: motor_in2

  # CLOSE direction
  - id: motor_close
    then:
      - script.execute: motor_stop
      - switch.turn_off: motor_in1
      - switch.turn_on: motor_in2
```

Finally, we leverage the end stop cover from ESPHome. This cover uses time to approximate how open or closed the shade is. And it invokes our script actions to control the motor and uses our endstops for sensing closing and open. 
```
cover:
  - platform: endstop
    name: "Endstop Cover"
    max_duration: 3min

    open_action:
      - switch.turn_on: open_cover_switch
    open_duration: 2min
    open_endstop: end_stop_open

    close_action:
      - switch.turn_on: close_cover_switch
    close_duration: 2min
    close_endstop: end_stop_closed

    stop_action:
      - switch.turn_off: open_cover_switch
      - switch.turn_off: close_cover_switch
```