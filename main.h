typedef struct {
    int xCoord;
    int yCoord;
    char direction;
    int holdingMarker;
} Robot;

typedef struct {
    int xCoord;
    int yCoord;
    int status;
} marker;

void fileToBoard(char filename[], int numSymbol);
void initialiseBoard();
void drawGrid();
void drawRobot();
void drawMarkers();
void drawForeground();
void forward();
void left();
void right();
int atMarker();
int atHome();
int canMoveForward();
void pickUpMarker();
void dropMarker();
int isCarryingAMarker();
void robotSearch();
void traceBackSteps(int numOfMoves);
int searchedSquareAhead();