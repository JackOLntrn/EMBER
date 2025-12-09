# EMBER
## Overview
The EMBER project makes use of the **MLX90640** thermal camera to detect fires on a pan/tilt motor mechanism. Once a fire is detected, the system points the thermal camera/hose nozzle at the detected fire and activates a solenoid to spray water at the detected fire. The system is supplied with water from a garden hose. This repository includes all of the relavant files for the project. 
| Directory | Contents|
| --- | --- |
| [EMBER/src](src/) | Contains the header and cpp files developed for the project |
| [EMBER/hardware/board](hardware/board) | Contains the schematic and board files used to develop a custom PCB used for the project. Additionally, gerber files are included which were used during ordering the board |
| [EMBER/hardware/mech](hardware/mech) | Contains the 3D model files of parts developed for the project |
| [EMBER/docs](docs/) | Contains doxygen generated documents for code documentation |
| [EMBER/platformio files](platformio%20files/) | Contains useful platformio files including platformio.ini which includes libraries used for project |

# Hardware
See board documentation [here](hardware/board/README.md)

See mechanical component documentation [here](hardware/mech/README.md)




