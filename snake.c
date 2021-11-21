#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>

#define WIDTH 60
#define HEIGHT 30
#define DELAY 100000

struct Point {
    int x, y;
};

enum Direction { LEFT, RIGHT, UP, DOWN };

// Global variables
int snakeLength;
bool gameOver;
enum Direction currDir;
struct Point snake[255] = {};

// Function prototypes
void drawScreen();
void initCurses();
void initGame();
void updateDirection(int);
void shiftSnake();

int main(void) {
    initCurses();
    initGame();

    while(TRUE) {
        int keypress = getch();
        flushinp(); // flush all input buffers from getch
        updateDirection(keypress);
        shiftSnake();
        drawScreen();

        // Delay between movements; DELAY = 0.1 second
	    usleep(DELAY); 
    }

    endwin(); // end ncurses
    return 0;
}

void drawScreen() {
    // Draw Border
    move(0, 0); // move cursor to the uppder left corner so border will be draw in the right position
    for (int i = 0; i <= HEIGHT; i++) {
        for (int j = 0; j <= WIDTH; j++) {
            // only print hashtages for firsts and lasts, which creates a border
            if (i == 0 || i == HEIGHT || j == 0 || j == WIDTH)
                addch('#');
            else 
                addch(' ');
        }
        addch('\n');
    }

    // Draw Snake
    for (int i = 0; i < snakeLength; i++) {
        mvaddch(snake[i].y, snake[i].x, 'O');
    }
    
    // move cursor out of the game screen; render
    move(HEIGHT + 1, 0);
    refresh();
}


void initCurses() {
    initscr(); // start ncurses
    keypad(stdscr,TRUE); // enable arrow keys
    timeout(0); // non-blocking read is used, so getchar does not pause and wait for input
}

void initGame() {
    clear(); // make a clean screen
    currDir = RIGHT;
    snakeLength = 5;
    gameOver = false;

    // Initial snake coordinates, at middle of the game screen
    int j = 0;
    for (int i = 0; i < snakeLength; i++) {
        int x = WIDTH / 2;
        int y = HEIGHT / 2;
        struct Point currPoint;
        currPoint.x = x - i;
        currPoint.y = y;

        snake[j++] = currPoint;
    }
}

void updateDirection(int keypress) {
    switch (keypress) {
        case KEY_UP:
            if (currDir == DOWN) {
                // gameover
                break;
            }
            currDir = UP;
            break;
        case KEY_DOWN:
            if (currDir == UP) {
                // gameover
                break;
            }
            currDir = DOWN;
            break;
        case KEY_LEFT:
            if (currDir == RIGHT) {
                // gameover
                break;
            }
            currDir = LEFT;
            break;
        case KEY_RIGHT:
            if (currDir == LEFT) {
                // gameover
                break;
            }
            currDir = RIGHT;
            break;
    }
}

void shiftSnake() {
    struct Point head = snake[0];
    switch (currDir) {
        case UP:
            head.y--;
            break;
        case DOWN:
            head.y++;
            break;
        case LEFT:
            head.x--;
            break;
        case RIGHT: 
            head.x++;
            break;
    }

    for (int i = snakeLength -1; i > 0; i--) {
        snake[i] = snake[i-1];
    }

    snake[0] = head;
}
