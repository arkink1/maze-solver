typedef struct {
    int xCoord;
    int yCoord;
    char direction;
    int holdingMarker;
} robot;

typedef struct {
    int xCoord;
    int yCoord;
    int holdingMarker;
} marker;

void drawGrid();
void drawRobot(robot);
void drawMarker(marker);
void forward(robot *, marker);
void left(robot *, marker);
void right(robot *, marker);
int atMarker(robot, marker);
int canMoveForward(robot);