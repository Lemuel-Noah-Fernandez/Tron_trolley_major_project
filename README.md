# Trontrolley

## The Problem:
In a world where automation is slowly taking over the world, the need for automated guidance in the supermarket setting where the number of items grows, is imminent for consumer convenience. In a bid to increase supermarket efficiency and the customer experience, our project, Tron Trolley, eliminates unneccessary time, and delivers convenience for the customer, by providing a trolley that can guide the customer to the aisle of the item location after an item is inputted into the system. Tron Trolley also acts as an important precursor to an automated shopping trolley - with more time and resources, full automation of the shopping experience could be achieved.

Our TronTrolley displays the trolley's location, the aisle number of the item, and finally the location of that aisle via a pointer. The functionality
is split into the following modules.


## Each module and its function
Our project includes software modules using Python, and hardware modules using HCS12. Due to issues with iic, we have had to simulate the results from the PTU in some modules.

### HCS12 modules:

#### Magnetometer
The functionality of the module is to measure the magnetic field strength. The module returns a binary result (high or low) if the magnetic field is above a certain threshold or not, to indicate if there is a magnet nearby. This module retains the current and previous binary value for use in the location module. This module was developed to use with the PTU, however it is now done in simulation.

#### Accelerometer
This module should read in the raw data from the accelerometers and apply basic filtering (both frequency and magnitude filtering to remove high frequency noise and high magnitude errors in sensor reading). The module should then integrate the signal to obtain the velocity - only the direction is needed, magnituded is unimportant to remove reliance on inaccurate sensor readings. The integration should be completed using the hardware timer to measure time between measurements (dt) and then applying an increment of accel times dt to increment the velocity. To prevent integration drift, the velocity should be reset every time the magnet is read as high. This module was developed to use with the PTU, however it is now done in simulation.

#### Location module
This module should take in the binary responses from the magnetometer and the velocities from the accelerometer. It should then return the location of the trolley with respect to a 1-dimensional grid. The grid resolution is double that of the number of magnet markers. The location is determined using the logic defined below in pseudocode:

if current magnet == high and previous magnet == low and velocity > 0:

    we have moved from between aisle i and aisle i+1 to aisle i+1
  
if current magnet == low and previous magnet == high and velocity > 0:

    we have moved from aisle i to between aisle i and aisle i+1
  
if current magnet == high and previous magnet == low and velocity < 0:

    we have moved from between aisle i and aisle i-1 to aisle i-1
  
if current magnet == low and previous magnet == high and velocity < 0:

    we have moved from aisle i to between aisle i and aisle i-1
  
if current magnet == previous magnet:

    we have not moved

### LCD Display
This module displays the requested item and it's aisle number. This data comes from the serialisation module.

### Seven Seg
This module displays the current aisle number on the seven segs. This data comes from the location module.

#### Servo
This module should take in an angle and rotate the servo/lidar to point at that angle relative to the trolley orientation.
This module has been partially written by Stewart. Some modifications required to allow input to be an angle in degrees relative to "forward" for the trolley.

#### Gyro
Get in raw data from gryo and apply some basic filtering. Integrate once to return the angle rotated (by the servo and trolley combined). This module was developed to use with the PTU, however it is no longer being used.

#### Serialisation module
Takes the position in the grid of the trolley, then sends it to the python script. Receives the angle that the servo should be rotated to point towards the searched item. By default the microcontroller will be in a reading state, but if it reads an 'R' command from python it will temporarily switch to write before going back to read.

### Python modules:

#### Serialisation module
This module unpacks the serialised data sent from the microcontroller to be used for updating the display. It sends the angle of rotation required by the servo. By default the python script will be in a writing state, but if it sends out an 'R' command to the microcontroller it will switch to a read state until the aisle number is read in, then switch back to writing.

#### Search module
This module allows the user to search for items or categories in the supermarket. This requires creation of a supermarket csv (or similar set of data), a search function with error handling (i.e. correcting misspelled searches or identifying when items are not in the supermarket), and returns the aisle the item is in.

#### Mapping module
This module takes in the location data from the serialisation (trolley location) and search (item location) modules. It then displays these locations on a basic map of the Tron Lab using pygame. This module should also calculate the angle of rotation required by the servo.

