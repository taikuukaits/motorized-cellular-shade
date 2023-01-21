# Software Design 

This are the restrictions under which this system is designed:

1. We have a way to count rotations of the shade shaft
2. There is no slippage - meaning we can take the rotation counts as gospel
3. We have no limit switches - meaning everything is relative, no absolutes
4. We can only drive the motor forward or backward - there are no steps or incremental moves - up vs down may be different timing

And we make some assumptions 
1. The shades should not need configuration if power is loss
2. Writing to EEPROM should be minimized - it has a limited number of writes - minimum of 5 years

A note on EEPROM and the 5 years. If we assume worst case it only has 100k writes (though its between 100k and 1 million) and we want the ESP to last for 5 years we can calculate the maximum number of writes: 100,000/(365*5) = 54 times. Plus we can use an EEPROM rotation library to further extend the life. This means we can open and close the shades 28 times a day and it will still last 5 years. Opening and closing 2 times a day (writing to the EEPROM 4 times - is (100000/4)/365 = 68 years which is completely reasonable. 

This still has the problem of what if you lose power during an open and close, the device forgets its location - it only remembers if it is up or if it is down. If the shade is up or down, it can lose power with no issue. When a shade does lose power it will need to be recalibrated. 

## Calibration

The shade needs a few things setup: 

1. It needs to be jogged to the zero position. 
2. It needs to be told it is in the zero position.
3. It needs to be told how many steps it takes to get to the open position.

Note recorded below means broadcast on an MQTT diagnostic channel for operator observablity. 

## Operation

The shade has several systems to make operation easier. 

### The MCS

The motion control subsystem or MCS moves the shade to a particular position - though not precisely. It can be requested to move to given position and it can figure out if that should be a positive or negative move or it can be told to move to a certain position in a certain direction. If the current position extends past the target in the specified direction, the MCS will not move. Generally moving to a specified position makes the most sense. 

The MCS can be thought of as in two states, MOVING or STATIONARY. When moving the MCS will either be moving in a POSITIVE direction or in a NEGATIVE direction. Then the MCS is told to move, it will only do so if it is currently STATIONARY. If it is currently MOVING it can only be told to STOP. Once the MCS has finished moving and after a short delay, it will write the current position to permanent storage. If a change in position is detected while the motor is stationary, that will also be written to permanent storage and recorded - see diagnosing stationary movement.

The MCS has an 'absolute zero' position established during calibration. It does not have to actually be zero, what is important is that the current position and all position requests are relative to this absolute zero. If zero is actually 'absolute zero' that means that the MCS does not need to store it. It already knows what absolute zero is. The open/close subsystem or OCS also uses an assumed zero so that it does not need to track two different values - it assumes the minimum is zero and thus does not need to store it. 

### The OCS

The open/close subsystem can convert arbitrary absolute positions into states that the Home Assistant can understand.\


## Configuration

The first thing that 
There are two MQTT topics used  you need to configure the blind with post install and one command to help you do so. 

`jog`
`zero`
`max`
