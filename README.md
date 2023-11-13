# COMP0002 C Programming Coursework
All 6 stages of the coursework have been completed.

## Compiling and running the program
To compile the program, run the following command:  
`gcc -o main main.c graphics.c -lm`

To run the program, run the following command:  
`./main | java -jar drawapp-2.0.jar`  
Or, if you want to specify a starting point and direction for the robot, enter the x and y coordinates from 0-9 and the direction (north, east, south, west) separated by spaces. For example:  
`./main 4 4 west | java -jar drawapp-2.0.jar`  
This will initialise the home square to (4, 4) on the 0-9 scale.

## Changing the coordinates of blocks and markers
To change the coordinates of blocks and markers, change the values in `blocks.txt` and `markers.txt` files. One set of coordinates can be entered on one line, with the x and y coordinate separated by a space. For example:  
`1 2`  
`0 9`  
`5 7`  
The coordinates are on a scale from 0-9.

## About the program
The window size is set to 500x500 with a 10x10 board centered inside it. The robot is a green triangle and is initialised on the blue home square. The black squares are blocks and the gray squares are markers. When the robot is carrying a marker, it will have a gray outline and when a marker is delivered to the home square, the blue square will have a smaller gray square inside it.

The robot moves to the square adjacent to it that it has visited the least previously. It does this until it finds a marker and then traces back its steps to the home square. This continues until every marker has been returned. It keeps track of how many times it has visited a square by adjusting the value of the square in the board matrix. Thus, it does not find the best path, but will eventually find all the markers that are not closed off by blocks.

To change the speed of the robot, the variable `sleepTime` can be adjusted on line 11 of `main.c`:  
`int sleepTime = 50;`