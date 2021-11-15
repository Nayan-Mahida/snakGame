#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define DELAY 100000
#define TIMEOUT 10 

// Create the type direction-type, with the possible values, LEFT, RIGHT, UP, DOWN
typedef enum 
{
	LEFT, RIGHT, UP, DOWN
} direction_type;