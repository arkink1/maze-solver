#include <stdio.h>
#include "graphics.h"
#include "main.h"
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

// Declare global variables
static Robot robot;
int sleepTime = 40;
int homeSquare[2];
int board[10][10];
int numOfMarkers;
int numMarkersReturned = 0;
marker markers[10]; // Maximum of 10 markers
int moves[1024];

int main(int argc, char **argv) {
    // Initialise robot with values given or set default values
    if (argc == 4) {
        robot.xCoord = atoi(argv[1]); robot.yCoord = atoi(argv[2]); robot.direction = argv[3][0];
    } else {
        robot.xCoord = 9; robot.yCoord = 2; robot.direction = 'n';
    }
    homeSquare[0] = robot.xCoord; homeSquare[1] = robot.yCoord;
    initialiseBoard();
    drawGrid();
    drawForeground();
    robotSearch();
    return 0;
}

void fileToBoard(char filename[], int numSymbol) {
    FILE *file = fopen(filename, "r");
    if (file != NULL) {
        int num, numX, count = 0;
        while (fscanf(file, "%d", &num) == 1) {
            if (count % 2 == 0) {
                numX = num;
            } else {
                // Only add marker/block if not home square
                if (numX != homeSquare[0] || num != homeSquare[1]) {
                    board[numX][num] = numSymbol;
                } else {
                    count -= 2; // Decrease count for numOfMarkers calculation
                }
            }
            ++count;
            // Break after 10 markers added
            if (!strcmp(filename, "markers.txt") && count > 20) {
                break;
            }
        }
        // Set the number of markers
        if (!strcmp(filename, "markers.txt")) {
            numOfMarkers = count / 2;
        }       
    }
    fclose(file);
}

void initialiseBoard() {
    // Initialise values on board matrix
    fileToBoard("blocks.txt", 3); // Blocks = 3
    fileToBoard("markers.txt", 2); // Markers = 2
    board[homeSquare[0]][homeSquare[1]] = 1; // Home square = 1
    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 10; ++j) {
            if (board[i][j] != 1 && board[i][j] != 2 && board[i][j] != 3) {
                board[i][j] = 4; // Default, free square = 4
            }
        }
    }
}

void drawGrid() {
    setWindowSize(500, 500);
    background();
    // 11 iterations to draw the vertical and horizontal gridlines
    for (int i = 0; i < 11; ++i) {
        drawLine(50+40*i, 50, 50+40*i, 450);
        drawLine(50, 50+40*i, 450, 50+40*i);
    }
    int nMark = 0;
    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 10; ++j) {
            switch(board[i][j]) {
                // 1: home square, 2: marker, 3: block
                case 1: setColour(blue); fillRect(50+40*i, 50+40*(9-j), 40, 40); board[i][j] = 4; break;
                case 2: markers[nMark].xCoord = i; markers[nMark].yCoord = j; markers[nMark].status = 0; ++nMark; break; // Initialise each marker
                case 3: setColour(black); fillRect(50+40*i, 50+40*(9-j), 40, 40); break;
            }
        }
    }
    foreground();
}

void drawRobot() {
    // Calculate coordinates of the triangle for each direction possibility
    int xCoords[3];
    int yCoords[3];
    switch(robot.direction) {
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
    if (isCarryingAMarker()) { // Gray outline on robot if carrying a marker
        setColour(gray);
        drawLine(xCoords[0], yCoords[0], xCoords[1], yCoords[1]);
        drawLine(xCoords[1], yCoords[1], xCoords[2], yCoords[2]);
        drawLine(xCoords[2], yCoords[2], xCoords[0], yCoords[0]);
    }
}

void drawMarkers() {
    setColour(gray);
    for (int i = 0; i < numOfMarkers; ++i) {
        if (markers[i].status == 0) {
            // If marker returned to home square, draw small marker (so home square can still be identified)
            if (markers[i].xCoord == homeSquare[0] && markers[i].yCoord == homeSquare[1]) {
                fillRect(60+40*markers[i].xCoord, 60+40*(9-markers[i].yCoord), 20, 20);
                char numMarkerString[2] = "0";
                sprintf(numMarkerString, "%d", numMarkersReturned);
                setColour(black);
                if (numMarkersReturned < 10) {
                    drawString(numMarkerString, 67+40*markers[i].xCoord, 75+40*(9-markers[i].yCoord));
                } else {
                    drawString(numMarkerString, 62+40*markers[i].xCoord, 75+40*(9-markers[i].yCoord));
                }
                setColour(gray);
            } else {
                fillRect(50+40*markers[i].xCoord, 50+40*(9-markers[i].yCoord), 40, 40);
            }
        }
    }
}

void drawForeground() {
    foreground();
    clear();
    drawMarkers();
    drawRobot();
}

void forward() {
    sleep(sleepTime);
    switch(robot.direction) {
        case 'n': ++robot.yCoord; break;
        case 'e': ++robot.xCoord; break;
        case 's': --robot.yCoord; break;
        case 'w': --robot.xCoord; break;
    }
    drawForeground();
}

void left() {
    sleep(sleepTime);
    switch(robot.direction) {
        case 'n': robot.direction = 'w'; break;
        case 'e': robot.direction = 'n'; break;
        case 's': robot.direction = 'e'; break;
        case 'w': robot.direction = 's'; break;
    }
    drawForeground();
}

void right() {
    sleep(sleepTime);
    switch(robot.direction) {
        case 'n': robot.direction = 'e'; break;
        case 'e': robot.direction = 's'; break;
        case 's': robot.direction = 'w'; break;
        case 'w': robot.direction = 'n'; break;
    }
    drawForeground();
}

int atMarker() {
    for (int i = 0; i < numOfMarkers; ++i) {
        if (robot.xCoord == markers[i].xCoord && robot.yCoord == markers[i].yCoord && markers[i].status == 0) {
            return true;
        }
    }
    return false;
}

int atHome() {
    if (robot.xCoord == homeSquare[0] && robot.yCoord == homeSquare[1]) {
        return true;
    } else {
        return false;
    }
}

int canMoveForward() {
    if (robot.direction == 'n' && (robot.yCoord == 9 || board[robot.xCoord][robot.yCoord+1] == 3)) {
        return false;
    } else if (robot.direction == 'e' && (robot.xCoord == 9 || board[robot.xCoord+1][robot.yCoord] == 3)) {
        return false;
    } else if (robot.direction == 's' && (robot.yCoord == 0 || board[robot.xCoord][robot.yCoord-1] == 3)) {
        return false;
    } else if (robot.direction == 'w' && (robot.xCoord == 0 || board[robot.xCoord-1][robot.yCoord] == 3)) {
        return false;
    } else {
        return true;
    }
}

void pickUpMarker() {
    for (int i = 0; i < numOfMarkers; ++i) {
        if (robot.xCoord == markers[i].xCoord && robot.yCoord == markers[i].yCoord && markers[i].status == 0) {
            markers[i].status = 1;
            break;
        }
    }
    robot.holdingMarker = true;
    drawForeground();
}

void dropMarker() {
    for (int i = 0; i < numOfMarkers; ++i) {
        if (markers[i].status == 1) {
            markers[i].xCoord = robot.xCoord;
            markers[i].yCoord = robot.yCoord;
            markers[i].status = 0;
            break;
        }
    }
    robot.holdingMarker = false;
    drawForeground();
}

int isCarryingAMarker() {
    return robot.holdingMarker;
}

void robotSearch() {
    int numOfMoves = 0, markersLeft = numOfMarkers;
    int stats[4] = {0, 0, 0, 0};
    int moveToMake[2] = {0, 0};
    // Algorithm involves moving to the least visited square adjacent to the robot until a marker is reached
    // Loop until all markers are returned to home square
    while (markersLeft != 0) {
        if (atMarker() && !atHome()) {
            pickUpMarker();
            board[robot.xCoord][robot.yCoord] = 4; // If marker has been visited, set value to that of a normal free square
            traceBackSteps(numOfMoves);
            dropMarker();
            --markersLeft;
            ++numMarkersReturned;
            numOfMoves = 0;
        } else {
            // Turn robot and move forward depending on the value the function returns
            switch(nextMove(stats, moveToMake)) {
                case 0:
                    forward(); moves[numOfMoves] = 1; ++numOfMoves;
                    break;
                case 1: 
                    right(); moves[numOfMoves] = 2; ++numOfMoves;
                    forward(); moves[numOfMoves] = 1; ++numOfMoves;
                    break;
                case 2: 
                    left(); moves[numOfMoves] = 3; ++numOfMoves;
                    forward(); moves[numOfMoves] = 1; ++numOfMoves;
                    break;
                case 3:
                    left(); moves[numOfMoves] = 3; ++numOfMoves;
                    left(); moves[numOfMoves] = 3; ++numOfMoves;
                    forward(); moves[numOfMoves] = 1; ++numOfMoves;
                    break;
            }
            // Increment the value of the square (keep track how many times the square has been visited for comparison)
            ++board[robot.xCoord][robot.yCoord];
        }
    }
}

int nextMove(int stats[4], int moveToMake[2]) {
    stats[0] = 100; stats[1] = 100; stats[2] = 100; stats[3] = 100;
    // Rotate robot 360 degrees and check the board matrix value of each square ahead
    if (canMoveForward()) { stats[0] = searchedSquareAhead(); }
    right();
    if (canMoveForward()) { stats[1] = searchedSquareAhead(); }
    left();
    left();
    if (canMoveForward()) { stats[2] = searchedSquareAhead(); }
    left();
    if (canMoveForward()) { stats[3] = searchedSquareAhead(); }
    left();
    left();
    moveToMake[0] = 0; moveToMake[1] = 0;
    // Loop through values of squares, return square with least value
    for (int m = 0; m < 4; ++m) {
        if (m == 0) { moveToMake[0] = m; moveToMake[1] = stats[m]; }
        else {
            if ((stats[m] != 0) && (stats[m] < moveToMake[1])) { moveToMake[0] = m; moveToMake[1] = stats[m]; }
        }
    }
    return moveToMake[0];
}

void traceBackSteps(int numOfMoves) {
    // Turn robot 180 degrees and do the opposite of each move previously made
    left();
    left();
    for (int i = numOfMoves; i > -1; --i) {
        switch(moves[i]) {
            case 1: forward(); break;
            case 2: left(); break;
            case 3: right(); break;
            default: break;
        }
        moves[i] = 0;
    }
}

int searchedSquareAhead() {
    // Return value of square in front of robot
    if (robot.direction == 'n' && (board[robot.xCoord][robot.yCoord+1] >= 4)) {
        return board[robot.xCoord][robot.yCoord+1];
    } else if (robot.direction == 'e' && (board[robot.xCoord+1][robot.yCoord] >= 4)) {
        return board[robot.xCoord+1][robot.yCoord];
    } else if (robot.direction == 's' && (board[robot.xCoord][robot.yCoord-1] >= 4)) {
        return board[robot.xCoord][robot.yCoord-1];
    } else if (robot.direction == 'w' && (board[robot.xCoord-1][robot.yCoord] >= 4)) {
        return board[robot.xCoord-1][robot.yCoord];
    } else { // Return 4 by default
        return 4;
    }
}