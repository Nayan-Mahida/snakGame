#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>

#define WIDTH 60
#define HEIGHT 30
#define DELAY 100000
#define snakeLength 5

struct Snake {
    int x, y;
    struct Snake *next;
};

enum Direction { LEFT, RIGHT, UP, DOWN };

// Global variables
bool gameOver;
enum Direction currDir;
struct Snake *head;

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
        if (gameOver) {
            sleep(2);
            initGame();
        }
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
    struct Snake *currBody;
    currBody = head;
    while (currBody != NULL) {
        mvaddch(currBody->y, currBody->x, 'O');
        currBody = currBody->next;
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
    gameOver = false;
    
    // Initial snake coordinates, at middle of the game screen
    int x = WIDTH / 2;
    int y = HEIGHT / 2;

    struct Snake *currBody;
    currBody = (struct Snake*) malloc(sizeof(struct Snake));
    currBody->next = NULL;
    currBody->x = x - (snakeLength - 1);
    currBody->y = y;

    for (int i = snakeLength - 2; i >= 0; i--) {
        head = (struct Snake*) malloc(sizeof(struct Snake));
        head->next = currBody;
        head->x = x - i;
        head->y = y;
        currBody = head;
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
    struct Snake *newHead;
    newHead = (struct Snake*) malloc(sizeof(struct Snake));
    newHead->next = head;
    newHead->x = head->x;
    newHead->y = head->y;
    head = newHead;    

    switch (currDir) {
        case UP:
            head->y--;
            break;
        case DOWN:
            head->y++;
            break;
        case LEFT:
            head->x--;
            break;
        case RIGHT: 
            head->x++;
            break;
    }

    // if head collide with border, gameover
    if (head->x >= WIDTH - 1 || head->x <= 1 || head->y >= HEIGHT - 1 || head->y <= 1) {
        gameOver = true;
    }

    struct Snake *currBody;
    currBody = head;
    while (currBody->next->next != NULL) {
        // if collide with body, gameover
        if (head->x == currBody->next->x && head->y == currBody->next->y) {
            gameOver = true;
        }
        currBody = currBody->next;
    }
    free(currBody->next);
    currBody->next = NULL;
}
