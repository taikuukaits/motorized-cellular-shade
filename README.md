# motorized-cellular-shade
Motorized cellular shades using ESPHome and cellular shade blinds from SelectBlinds.com

These blinds were among the cheapest (yet still nice) I could find for my blinds. They still ran me $87 per window for 16 1/4" W x 61" H shades. 

This project uses the Endstop Cover of ESPHome to achieve its goals: https://esphome.io/components/cover/endstop/

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

We need the following wired up for ESP Home to work:
```
D1 to (white) B 1A L9110
D2 to (purple) B 2A L9110
D6 to End Stop 1
D5 to End Stop 2
```

The following is required for power: 
```
5V on D1 mini to Buck Converter Out +
G on D1 mini to Buck Converter Out -
VCC on L9110 to Buck Converter In +
GND on L9110 to Buck Converter In -
Buck Converter In + to 12V Source
Buck Converter In - to 12V Source
```

# Sensors in ESPHome
Each of the two endstops are binary sensors in ESPHome
```
binary_sensor:
  - platform: gpio
    device_class: opening
    pin: D6
    id: end_stop_open
    name: End Stop Open
    mode: INPUT_PULLUP
    inverted: true
binary_sensor:
  - platform: gpio
    device_class: opening
    pin: D5
    id: end_stop_closed
    name: End Stop Closed
    mode: INPUT_PULLUP
    inverted: true
```

And our motor is going to be a binary output
```
output:
  - platform: gpio
    pin: D1
    id: motor_1
  - platform: gpio
    pin: D2
    id: motor_2
```

And finally we have the cover itself, this will control the motor pins based on the endstops. 

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
The main idea here is HIGH on D1 and LOW on D2 will spin the motor one way, and LOW on D1 and HIGH on D2 will spin it the other way. LOW on both will stop the motor: https://docs.sunfounder.com/projects/3in1-kit-v2/en/latest/components/component_l9110_module.html
Note: I'm not sure what the difference is between brake and stop. 

The full yaml file is available in the repo.

```
substitutions:
  open_endstop_inverted: "true"
  closed_endstop_inverted: "true"
  failsafe_run_time: "30s"

switch:
  - platform: gpio
    id: motor_in1
    pin: D1
    restore_mode: ALWAYS_OFF

  - platform: gpio
    id: motor_in2
    pin: D2
    restore_mode: ALWAYS_OFF

binary_sensor:
  - platform: gpio
    id: endstop_open_raw
    name: "Endstop Open (raw)"
    pin:
      number: D6
      mode: INPUT_PULLUP
      inverted: ${open_endstop_inverted}
    filters:
      - delayed_on: 20ms
      - delayed_off: 20ms

  - platform: gpio
    id: endstop_closed_raw
    name: "Endstop Closed (raw)"
    pin:
      number: D5
      mode: INPUT_PULLUP
      inverted: ${closed_endstop_inverted}
    filters:
      - delayed_on: 20ms
      - delayed_off: 20ms

  - platform: template
    id: endstop_open
    name: "Endstop Open"
    lambda: |-
      return id(endstop_open_raw).state;
    on_press:
      then:
        - script.execute: motor_stop
        - cover.template.publish:
            id: my_cover
            state: OPEN

  - platform: template
    id: endstop_closed
    name: "Endstop Closed"
    lambda: |-
      return id(endstop_closed_raw).state;
    on_press:
      then:
        - script.execute: motor_stop
        - cover.template.publish:
            id: my_cover
            state: CLOSED
globals:
  - id: moving_dir   # -1 = closing, 0 = stopped, +1 = opening
    type: int
    restore_value: no
    initial_value: "0"

script:
  - id: motor_stop
    mode: restart
    then:
      - switch.turn_off: motor_in1
      - switch.turn_off: motor_in2
      - lambda: |-
          id(moving_dir) = 0;
  - id: motor_open
    mode: restart
    then:
      # never drive open if already at endstop
      - if:
          condition:
            binary_sensor.is_on: endstop_open
          then:
            - script.execute: motor_stop
            - cover.template.publish:
                id: my_cover
                state: OPEN
          else:
            - script.execute: motor_stop
            - switch.turn_on: motor_in1
            - switch.turn_off: motor_in2
            - lambda: |-
                id(moving_dir) = 1;
  - id: motor_close
    mode: restart
    then:
      # never drive closed if already at endstop
      - if:
          condition:
            binary_sensor.is_on: endstop_closed
          then:
            - script.execute: motor_stop
            - cover.template.publish:
                id: my_cover
                state: CLOSED
          else:
            - script.execute: motor_stop
            - switch.turn_off: motor_in1
            - switch.turn_on: motor_in2
            - lambda: |-
                id(moving_dir) = -1;
cover:
  - platform: template
    id: my_cover
    name: "Endstop Cover"
    device_class: garage
    optimistic: false

    open_action:
      - if:
          condition:
            binary_sensor.is_on: endstop_open
          then:
            - cover.template.publish:
                id: my_cover
                state: OPEN
          else:
            - cover.template.publish:
                id: my_cover
                state: OPENING
            - script.execute: motor_open
            # Failsafe stop: prevents endless run if an endstop fails
            - delay: ${failsafe_run_time}
            - if:
                condition:
                  lambda: 'return id(moving_dir) == 1;'
                then:
                  - script.execute: motor_stop
                  - cover.template.publish:
                      id: my_cover
                      state: STOPPED

    close_action:
      - if:
          condition:
            binary_sensor.is_on: endstop_closed
          then:
            - cover.template.publish:
                id: my_cover
                state: CLOSED
          else:
            - cover.template.publish:
                id: my_cover
                state: CLOSING
            - script.execute: motor_close
            - delay: ${failsafe_run_time}
            - if:
                condition:
                  lambda: 'return id(moving_dir) == -1;'
                then:
                  - script.execute: motor_stop
                  - cover.template.publish:
                      id: my_cover
                      state: STOPPED

    stop_action:
      - script.execute: motor_stop
      - cover.template.publish:
          id: my_cover
          state: STOPPED

# Restore state on boot.
on_boot:
  priority: -10
  then:
    - script.execute: motor_stop
    - if:
        condition:
          binary_sensor.is_on: endstop_open
        then:
          - cover.template.publish:
              id: my_cover
              state: OPEN
        else:
          - if:
              condition:
                binary_sensor.is_on: endstop_closed
              then:
                - cover.template.publish:
                    id: my_cover
                    state: CLOSED
              else:
                - cover.template.publish:
                    id: my_cover
                    state: STOPPED

```



