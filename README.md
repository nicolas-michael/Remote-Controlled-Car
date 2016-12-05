# Remote-Controlled-Car
###CSS427 Embedded Systems Final Project
Remote controlled car with collision prevention build using an Arduino Mega with communication using the Zigbee (802.15.4) wireless protocol.
 

###Terminal supports to following commands:
| Command | Action |
| ------- | ------ |
| A, D, W, S, Q, R | Left, Right, Forward, Backward, Forward-left, Forward-right |
| temp | retrieve temperature of car |
| prox | retrieve proximity data from near by objects |
| comp | retrieve direction from the compass sensor |
| accel | retrieve accelerometer data |
| mode (periodic / demand) | setting mode to periodic returns sensor data every DELAY seconds, set using the delay command |
| delay (sensor) | sets the delay time between sensor data automated retreival |
| PINMODE (#) | toggles the pin # HIGH/LOW |

