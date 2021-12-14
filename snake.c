// ChenYang Lin, Yiu Wong, Nayan Mahida, Elijah Jones
// 12/10/21
// Final Project - Snake Game

#include <curses.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define WIDTH 60
#define HEIGHT 20
#define SPEED 300000
#define LENGTH 3

// linked list of snake
struct Snake {
    int x, y;
    struct Snake *next;
};
// linked list of trophy
struct Trophy
{
    int x, y;
};

// Enum directions
enum Direction { LEFT, RIGHT, UP, DOWN };

// Global variables
int speed, snakeLength, numNewBodies, numTrophy, trophyExpireTime;
float speedRate = 0.98;
bool gameOver = false, exitGame = false;
enum Direction currDir;
struct Snake *head;
struct Trophy trophy;
time_t trophySpawnTime, now;

// Function prototypes
void initGame();
void updateDirection(int);
void shiftSnake();
void drawScreen();
void spawnTrophy();
void gameOverScreen();

// Main Function
int main(void) {
    initGame();

    while(!exitGame) {
        if (gameOver) {
            gameOverScreen("You lose...\n");
        }
        int keypress = getch();
        updateDirection(keypress);
        shiftSnake();
        drawScreen();

        // Delay between movements 
	    usleep(speed); 
    }

    endwin(); // end curses
    return 0;
}

// Elijah Jones
void initGame() {
    clear(); // make a clean screen

    // Init curses
    initscr(); // start ncurses
    keypad(stdscr,TRUE); // enable arrow keys
    timeout(0); // non-blocking read is used, so getchar does not pause and wait for input
    noecho();
    curs_set(0); // disable cursor on terminal, but it doesn't work on google clould platform ternmianl...
    raw(); // line buffering is disabled

    // Init Snake status
    currDir = RIGHT;
    gameOver = false;
    speed = SPEED;
    snakeLength = LENGTH;
    
    // Initial snake coordinates, at middle of the game screen
    int x = WIDTH / 2;
    int y = HEIGHT / 2;

    struct Snake *currBody;
    currBody = (struct Snake*) malloc(sizeof(struct Snake));
    currBody->next = NULL;
    currBody->x = x - (LENGTH - 1);
    currBody->y = y;

    for (int i = LENGTH - 2; i >= 0; i--) {
        head = (struct Snake*) malloc(sizeof(struct Snake));
        head->next = currBody;
        head->x = x - i;
        head->y = y;
        currBody = head;
    }    

    spawnTrophy(); 
}

// Nayan Mahida
void updateDirection(int keypress) {
    switch (keypress) {
        case KEY_UP:
            if (currDir == DOWN) {
                gameOverScreen("You lose (attempts to reverse the snake's direction)...\n");
                break;
            }
            currDir = UP;
            break;
        case KEY_DOWN:
            if (currDir == UP) {
                gameOverScreen("You lose (attempts to reverse the snake's direction)...\n");
                break;
            }
            currDir = DOWN;
            break;
        case KEY_LEFT:
            if (currDir == RIGHT) {
                gameOverScreen("You lose (attempts to reverse the snake's direction)...\n");
                break;
            }
            currDir = LEFT;
            break;
        case KEY_RIGHT:
            if (currDir == LEFT) {
                gameOverScreen("You lose (attempts to reverse the snake's direction)...\n");
                break;
            }
            currDir = RIGHT;
            break;
        case 'x':       
        case 'X':
            exitGame = true;
    }
}

// Yiu Wong
void shiftSnake() {
    // Create a new snake head based on the user input for direction
    struct Snake *newHead;
    newHead = (struct Snake*) malloc(sizeof(struct Snake));
    newHead->next = head;
    newHead->x = head->x;
    newHead->y = head->y;
    head = newHead;    

    if (currDir == UP)
        head->y--;
    else if (currDir == DOWN)
        head->y++;
    else if (currDir == LEFT)
        head->x--;
    else if (currDir == RIGHT)
        head->x++;
        

    // if head collide with border, gameover
    if (head->x >= WIDTH || head->x <= 0 || head->y >= HEIGHT || head->y <= 0) {
        gameOver = true;
        gameOverScreen("You lose (you hit a wall)...\n");
    }

    // Check if snake collides with trophy
    if (head->x == trophy.x && head->y == trophy.y) {
        numNewBodies += numTrophy;
        snakeLength += numTrophy;
        spawnTrophy();        
    }

    // snake shift, and if head collide with body, gameover
    struct Snake *currBody;
    currBody = head;
    while (currBody->next->next != NULL) {
        // if collide with body, gameover
        if (currBody->next->x == head->x && currBody->next->y == head->y) {
            gameOver = true;
            gameOverScreen("You lose (you ate youself)...\n");
        }
        currBody = currBody->next;
    }

    // if there is no new body left to add, remove tail
    if (numNewBodies <= 0) {
        mvaddch(currBody->next->y, currBody->next->x, ' '); // clear that body from screen
        free(currBody->next);
        currBody->next = NULL;
    }
    else {
        numNewBodies--;
        speed = (int)(speed * speedRate);
    }
}

// Nayan Mahida
void drawScreen() {
    // Draw Border
    move(0, 0); // move cursor to the uppder left corner so border will be draw in the right position
    for (int i = 0; i <= HEIGHT; i++) {
        for (int j = 0; j <= WIDTH; j++) {
            // only print hashtages for firsts and lasts, which creates a border
            if (i == 0 || i == HEIGHT || j == 0 || j == WIDTH)
                mvaddch(i, j, '#');
        }
        addch('\n');
    }
    
    // Draw Snake
    struct Snake *currBody;
    currBody = head;
    mvaddch(currBody->y, currBody->x, 'O');
    while (currBody->next != NULL) {
        mvaddch(currBody->next->y, currBody->next->x, 'O');
        currBody = currBody->next;
    }

    // ChenYang Lin contributed to the following sections
    // Update trophy - if an trophy expired, remove it and spawn a new one.
    now = time(NULL);
    int timeSpent = (now - trophySpawnTime);   
    int remainingTime = trophyExpireTime - timeSpent;
    if (remainingTime <= 0) {
        mvaddch(trophy.y, trophy.x, ' '); // clear current trophy from the screen
        remainingTime = 0;
        spawnTrophy();
    } 

    // Render Game Status
    // trophy info
    mvprintw(0, WIDTH + 2, "Trophy expires in: %d seconds", remainingTime);
    // game progress
    float halfPerimeter = HEIGHT + WIDTH;
    float length = snakeLength;
    float progress = (length / halfPerimeter) * 100;
    mvprintw(4, WIDTH + 2, "Progress: %d %%", (int)progress);
    if ((int)progress >= 100) {
        gameOverScreen("You won!!!\n");
    }

    
    // render
    move(HEIGHT + 1, 0); // move cursor out of the game screen because curs_set(0); does not work in GCP terminal
    refresh(); // render screen, screen buffer -> real screen
}

// ChenYang Lin
// spawns random trophy at random location on the screen
void spawnTrophy() {
    bool overlapping = true;
    // Check if trophy spawn on the location already taken by snake
    while(overlapping) {
        overlapping = false;
        
        trophy.x = random() % (WIDTH - 1) + 1;
        trophy.y = random() % (HEIGHT - 1) + 1;

        struct Snake *currBody;
        currBody = head;
        if (trophy.x == currBody->x && trophy.y == currBody->y) {
            overlapping = true;
        }
        while (currBody->next != NULL) {
            // if trophy collides with body, discard and generate a new trophy
            if (trophy.x == currBody->next->x && trophy.y == currBody->next->y) {
                overlapping = true; 
            }
            currBody = currBody->next;
        }
    }
    numTrophy = rand() % 9 + 1;
    trophyExpireTime = rand() % 9 + 1;
    trophySpawnTime = time(NULL);
    char numTrophyChar = numTrophy + '0';  
    mvaddch(trophy.y, trophy.x, numTrophyChar);    
}

void gameOverScreen(char* result) {
    clear();
    mvprintw(8, 45, result);
    mvprintw(10, 40, "Game Over! Hit any key to close game");
    refresh();
    sleep(3);
    getchar();
    endwin();
    exit(0);
}

