# Remote Control Robot

## General Overview
Project included building and programming a robot that was controlled by sending commands via a wireless gamepad to a computer that was mounted on to the robot. The computer on the robot had a wireless receiver connected to a USB port that was linked with the gamepad. When the gamepad sent a command, the computer would receive the command and transmit it to a module via serial communication that was responsible for the carrying out the designated commands. 
  
The two modules that are present are the drive module and the sensor module. The drive module was responsible for engaging and halting the motors. It maintained a heartbeat with the computer that would shut the motors off if a command was not received after a set time. The sensor module would constantly check distance sensors that were placed around the robot. If any object came within a specified distance of the robot, the module would transmit a message to halt all motors until there was no longer an object present. This module design also allows for continual integration of systems onto the robot, without potentially sacrificing the responsiveness of the system.

Continued work sees me adding a compass as well as a camera. While the robot is able to be programmed and drive autonomously for a period of time, it does not have full autonomy. The compass module would allow the robot to maneuver on its own based on its coordinates. The camera will be used to allow for the tracking of an object, such as a red ball. 

## Technology Used
- Python
- C++
- Arduino

## Robot Pictures
<img align="left" alt="Robot Front" width="250px" src="https://i.imgur.com/RO5btTN.jpg"/>
<img align="left" alt="Robot Front" width="250px" src="https://i.imgur.com/PRL1Y2G.jpg"/>
<img align="left" alt="Robot Top" width="500px" src="https://i.imgur.com/R7QoZjD.jpg"/>
