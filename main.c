#include <stdio.h>
#include "graphics.h"
#include "main.h"
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

// Declare global variables
int sleepTime = 200;
int board[10][10];
int moves[1024];
marker markers[3];

int main(int argc, char **argv)
{
    static robot robot; // Create instance of robot
    // Create instances of 3 markers using a loop
    markers[0].xCoord = 9; markers[0].yCoord = 2; markers[0].holdingMarker = false;
    markers[1].xCoord = 3; markers[1].yCoord = 9; markers[1].holdingMarker = false;
    markers[2].xCoord = 0; markers[2].yCoord = 5; markers[2].holdingMarker = false;

    // Default values if command line args are not given
    robot.xCoord = 2;
    robot.yCoord = 9;
    char *initialDirection = "north";

    if (argc == 4) { // Four args given
        robot.xCoord = atoi(argv[1]); // Get x value
        robot.yCoord = atoi(argv[2]); // Get y value
        initialDirection = argv[3]; // Get direction
    }
    robot.direction = initialDirection[0];
    robot.holdingMarker = 0;

    // Initialisation - draw grid, robot on home square and marker
    int homeSquare[2] = {robot.xCoord, robot.yCoord};
    drawGrid(homeSquare);
    drawRobot(robot);
    drawMarker(markers);

    // Loop through every square to setup board representation
    /*for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 10; ++j) {
            if (i == robot.xCoord && j == robot.yCoord) {
                board[i][j] == 1;
            } else if (i == marker.xCoord && j == marker.yCoord) {
                board[i][j] == 2;
            } else {
                board[i][j] == 0;
            }
        }
    }*/

    // Loop to find marker while recording every move
    int movesCount = 0;
    while (!atMarker(robot, markers)) {
        if (canMoveForward(robot)) {
            forward(&robot, markers);
            moves[movesCount] = 1;
        } else {
            right(&robot, markers);
            moves[movesCount] = 2;
        }
        ++movesCount;
    }
    robot.holdingMarker = true;
    for (int i = 0; i < 3; ++i) {
        markers[i].holdingMarker = true;
    }
    
    // Turn around twice to set robot up for return
    right(&robot, marker);
    right(&robot, marker);

    // Loop to return to home square
    for (int i = movesCount-1; i > -1; --i) {
        switch(moves[i]) {
            case 1: forward(&robot, marker); break;
            case 2: left(&robot, marker); break;
            case 3: right(&robot, marker); break;
        }
    }

    return 0;
}

void drawGrid(int homeSquare[2]) {
    setWindowSize(500, 500);
    background();

    // 11 iterations to draw the vertical and horizontal gridlines
    for (int i = 0; i < 11; ++i) {
        drawLine(50+40*i, 50, 50+40*i, 450); // Vertical gridline
        drawLine(50, 50+40*i, 450, 50+40*i); // Horizontal gridline
    }

    // Draw blue home square
    setColour(blue);
    fillRect(50+homeSquare[0]*40, 50+(9-homeSquare[1])*40, 40, 40);

    foreground();
}

void drawRobot(robot robot) {

    // Calculate coordinates of the triangle for each direction possibility
    int xCoords[3];
    int yCoords[3];
    switch(robot.direction) {
        case 'n':
            xCoords[0] = 50+40*robot.xCoord; xCoords[1] = 90+40*robot.xCoord; xCoords[2] = 70+40*robot.xCoord;
            yCoords[0] = 90+(9-robot.yCoord)*40; yCoords[1] = 90+(9-robot.yCoord)*40; yCoords[2] = 50+(9-robot.yCoord)*40;
            break;
        case 'e':
            xCoords[0] = 50+40*robot.xCoord; xCoords[1] = 50+40*robot.xCoord; xCoords[2] = 90+40*robot.xCoord;
            yCoords[0] = 50+(9-robot.yCoord)*40; yCoords[1] = 90+(9-robot.yCoord)*40; yCoords[2] = 70+(9-robot.yCoord)*40;
            break;
        case 's':
            xCoords[0] = 50+40*robot.xCoord; xCoords[1] = 90+40*robot.xCoord; xCoords[2] = 70+40*robot.xCoord;
            yCoords[0] = 50+(9-robot.yCoord)*40; yCoords[1] = 50+(9-robot.yCoord)*40; yCoords[2] = 90+(9-robot.yCoord)*40;
            break;
        case 'w':
            xCoords[0] = 90+40*robot.xCoord; xCoords[1] = 90+40*robot.xCoord; xCoords[2] = 50+40*robot.xCoord;
            yCoords[0] = 50+(9-robot.yCoord)*40; yCoords[1] = 90+(9-robot.yCoord)*40; yCoords[2] = 70+(9-robot.yCoord)*40;
            break;
        default: // Point robot north in the case of invalid direction input
            robot.direction = 'n';
            xCoords[0] = 50+40*robot.xCoord; xCoords[1] = 90+40*robot.xCoord; xCoords[2] = 70+40*robot.xCoord;
            yCoords[0] = 90+(9-robot.yCoord)*40; yCoords[1] = 90+(9-robot.yCoord)*40; yCoords[2] = 50+(9-robot.yCoord)*40;
            break;
    }

    setColour(green);
    fillPolygon(3, xCoords, yCoords);
}

void drawMarker(marker marker) {
    if (marker.holdingMarker == false) {
        setColour(gray);
        fillRect(50+marker.xCoord*40, 50+(9-marker.yCoord)*40, 40, 40);
    }
}

void forward(robot *robot, marker marker) {
    switch(robot->direction) {
        case 'n': ++robot->yCoord; break;
        case 'e': ++robot->xCoord; break;
        case 's': --robot->yCoord; break;
        case 'w': --robot->xCoord; break;
    }
    foreground();
    clear();
    drawMarker(marker);
    drawRobot(*robot);
    sleep(sleepTime);
}

void left(robot *robot, marker marker) {
    switch(robot->direction) {
        case 'n': robot->direction = 'w'; break;
        case 'e': robot->direction = 'n'; break;
        case 's': robot->direction = 'e'; break;
        case 'w': robot->direction = 's'; break;
    }
    foreground();
    clear();
    drawMarker(marker);
    drawRobot(*robot);
    sleep(sleepTime);
}

void right(robot *robot, marker marker) {
    switch(robot->direction) {
        case 'n': robot->direction = 'e'; break;
        case 'e': robot->direction = 's'; break;
        case 's': robot->direction = 'w'; break;
        case 'w': robot->direction = 'n'; break;
    }
    foreground();
    clear();
    drawMarker(marker);
    drawRobot(*robot);
    sleep(sleepTime);
}

int atMarker(robot robot, marker marker) {
    if (robot.xCoord == marker.xCoord && robot.yCoord == marker.yCoord && marker.holdingMarker == false) {
        return true;
    } else {
        return false;
    }
}

int canMoveForward(robot robot) {
    if (robot.direction == 'n' && robot.yCoord == 9) {
        return false;
    } else if (robot.direction == 'e' && robot.xCoord == 9) {
        return false;
    } else if (robot.direction == 's' && robot.yCoord == 0) {
        return false;
    } else if (robot.direction == 'w' && robot.xCoord == 0) {
        return false;
    } else {
        return true;
    }
}

