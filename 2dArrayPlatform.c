/*
Created by Justis Rothbauer
Started on May 11 2024
And finished version 1.0 at DBT

This is a platformer game that I'm making so using the terminal for the display.
*/

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <stdlib.h>
#include <windows.h>
#include <string.h>
#include <time.h>
//#define DEBUG

#define WINDOW_HEIGHT 16
#define WINDOW_WIDTH 50
#define LEVEL_MOVE_PERCENT 1.0/3.0
#define TICK_RESET 30000

typedef struct {
	int jumpHeight;
	int offset;// This will be used to determine where the player is in the array
	int x,y;// the x coord will mostly be visual, but the y value is important
}PLAYER;

// This will create one section of the level and represents a whole screen
typedef struct {
	int levelSec[WINDOW_HEIGHT][WINDOW_WIDTH];
}LEVEL_SECTION;

typedef struct pNode{
	LEVEL_SECTION *node;
	struct pNode *pPrev, *pNext;
}LEVEL;

void display_symbol(int x, int y, const void *display);// For display use a string, I don't know why but for the function to work the arg has to be void.
LEVEL* createNewLevel();
void addLevelSection(LEVEL *level);
void displayLevel(const LEVEL *level, int offset);
void clearLevel(const LEVEL *level, int offset);
void displayPlayer(const PLAYER *player);
void clearPlayer(const PLAYER *player);
void updatePlayerPosition(PLAYER *player, int *level, char direction, int *tick);// only supports wasd for movement at the moment
void gravity(PLAYER *player, const int *level, int *tick);
int checkInBounds(const PLAYER *player);
int randint(int first, int last);

void devToolDisplay2dArray(LEVEL *level);
int main(void){
//	PLAYER player = {5,0,0,0};// the screen starts at 0
//	char playerInput = '\0';// This initializes player input before it's used
//	char *validInputStr = "wasd";
//	int **level = (int**)malloc(WINDOW_HEIGHT * sizeof(int*));
////	int level[WINDOW_HEIGHT][WINDOW_WIDTH + 1000];// Allocates the memory on the stack for the level
//	for(int j = 0; j < WINDOW_HEIGHT; j++){
//		malloc()
//		for(int i = 0; i < WINDOW_WIDTH + 1000; i++){// Initializes the level
//			level[j][i] = 1;
//		}
//	}
//	int tick = 0;
//	displayLevel(level, player.offset);
//	displayPlayer(&player);
//	sleep(2);
//	while(player.y <= WINDOW_HEIGHT){
//		playerInput = _kbhit() ? getch() : '?';// This will get and store any key the player inputs
//		if(strchr(validInputStr,tolower(playerInput))){
//			if(checkInBounds(&player)){
//				clearPlayer(&player);
//			}
//			clearLevel(level, player.offset - player.x);
//			updatePlayerPosition(&player, level, playerInput, &tick);
//			displayLevel(level, player.offset - player.x);
//			if(checkInBounds(&player)){
//				displayPlayer(&player);
//			}
//		}
//		gravity(&player, level, &tick);
//		
//
//	}
	LEVEL *level = createNewLevel();
	addLevelSection(level);
	displayLevel(level, 0);
	
	PLAYER player = {5,0,0,0};// the screen starts at 0
	char playerInput = '\0';// This initializes player input before it's used
	char *validInputStr = "wasd";
	int tick = 0;
	while(player.y <= WINDOW_HEIGHT){
		playerInput = _kbhit() ? getch() : '?';// This will get and store any key the player inputs
		if(strchr(validInputStr,tolower(playerInput))){
			//if(checkInBounds(&player)){
				clearPlayer(&player);
			//}
			clearLevel(level, player.offset - player.x);
			updatePlayerPosition(&player, level, playerInput, &tick);
			displayLevel(level, player.offset - player.x);
			//if(checkInBounds(&player)){
				displayPlayer(&player);
			//}
		}
//		gravity(&player, level, &tick);
		

	}
	return 0;
}
// This will put a symbol within the bounds you give and x and y both start at 0. Also arg display use "" for the character
void display_symbol(int x, int y, const void *display){
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD pos = {x, y};
	SetConsoleCursorPosition(hConsole, pos);
	WriteConsole(hConsole, display, 1, NULL, NULL);
}
// This will return a pointer the LEVEL linked list. What it makes in one flat section that only has width WINDOW_WIDTH!!!
LEVEL* createNewLevel(){
	LEVEL *level = (LEVEL*)malloc(sizeof(LEVEL));
	level -> node = (LEVEL_SECTION*)malloc(sizeof(LEVEL_SECTION));
	level -> pPrev = NULL;
	level -> pNext = NULL;
// This code goes through the entire level structure except the last row
	int (*bA)[WINDOW_WIDTH] = (level -> node) -> levelSec;
	for(int (*pYEnd)[WINDOW_WIDTH] = bA + WINDOW_HEIGHT - 1; bA < pYEnd; bA++){
		for(int *pStart = *bA, *pXEnd = pStart + WINDOW_WIDTH; pStart < pXEnd; pStart++){
			*pStart = 0;
		}
	}
	// This initilizes the last row to 1 which so far represents platforms, and I plan to have other numbers represent other things.
	for(int *pStart = *(((level -> node) -> levelSec) + WINDOW_HEIGHT - 1), *pEnd = pStart + WINDOW_WIDTH; pStart < pEnd; pStart++){
		*pStart = 1;
	}
	return level;
// For future use
//	int (*bA)[WINDOW_WIDTH] = (level -> node) -> levelSec;
//	for(int (*pYEnd)[WINDOW_WIDTH] = bA + WINDOW_HEIGHT; bA < pYEnd; bA++){
//		for(int *pStart = *bA, *pXEnd = pStart + WINDOW_WIDTH; pStart < pXEnd; pStart++){
//			printf("%d", *pStart);
//		}
//		putchar('\n');
//	}
}
// This will take the spot you are at, and will add it to pNext AND WILL OVERWRITE THE NEXT PART IF NOT CHECKED. Supposed to be used when there is no next part.
void addLevelSection(LEVEL *level){
	// This section adds the new level section to the end of the linked list, because the player moves
	// through the linked list, and pNext should be checked and should be NULL when this is called.
	LEVEL *newNode = (LEVEL*)malloc(sizeof(LEVEL));
	level -> pNext = newNode;
	newNode -> pPrev = level;
	newNode -> pNext = NULL;
	newNode -> node = (LEVEL_SECTION*)malloc(sizeof(LEVEL_SECTION));
	
	// This section actually generates the new section that was allocated above.
	int (*newSec)[WINDOW_WIDTH] = (newNode -> node) -> levelSec;
	for(int (*pYEnd)[WINDOW_WIDTH] = newSec + WINDOW_HEIGHT - 1; newSec < pYEnd; newSec++){
		for(int *pStart = *newSec, *pXEnd = pStart + WINDOW_WIDTH; pStart < pXEnd; pStart++){
			*pStart = 1;
		}
	}
	// This initilizes the last row to 1 which so far represents platforms, and I plan to have other numbers represent other things.
	for(int *pStart = *(((newNode -> node) -> levelSec) + WINDOW_HEIGHT - 1), *pEnd = pStart + WINDOW_WIDTH; pStart < pEnd; pStart++){
		*pStart = 0;
	}
}

// This will loop through the linked list from minimum only level, if there is offset it will read two linked lists.
// FUNCTION WILL BREAK IF OFFSET > WINDOW_WIDTH
void displayLevel(const LEVEL *level, int offset){
	// Below will display from where the player is to the end of the current linked list.
	int (*bA)[WINDOW_WIDTH] = (level -> node) -> levelSec;
	for(int (*pYEnd)[WINDOW_WIDTH] = bA + WINDOW_HEIGHT, y = 0; bA < pYEnd; bA++, y++){
		for(int *pStart = *bA + offset, *pXEnd = *bA + WINDOW_WIDTH, x = 0; pStart < pXEnd; pStart++, x++){
			if(*pStart == 1){
				display_symbol(x, y, "-");
			}
		}
	}
	// If offset > 0 then it will go to the other node in the liked list, and display the first offset collums in the linked list.
	if(offset){
		int (*NextBA)[WINDOW_WIDTH] = ((level -> pNext) -> node) -> levelSec;
	// Below will display from where the player is to the end of the current linked list.
		for(int (*pYEnd)[WINDOW_WIDTH] = NextBA + WINDOW_HEIGHT, y = 0; NextBA < pYEnd; NextBA++, y++){
			for(int *pStart = *NextBA, *pXEnd = pStart + offset, x = WINDOW_WIDTH - offset; pStart < pXEnd; pStart++, x++){
				if(*pStart == 1){
					display_symbol(x, y, "-");
				}
			}
		}
	}
}
// This will loop through the level array, and clear the display from [offset, (offset + WINDOW_WIDTH)]
void clearLevel(const LEVEL *level, int offset){
	// Below will display from where the player is to the end of the current linked list.
	int (*bA)[WINDOW_WIDTH] = (level -> node) -> levelSec;
	for(int (*pYEnd)[WINDOW_WIDTH] = bA + WINDOW_HEIGHT, y = 0; bA < pYEnd; bA++, y++){
		for(int *pStart = *bA + offset, *pXEnd = *bA + WINDOW_WIDTH, x = 0; pStart < pXEnd; pStart++, x++){
			if(*pStart == 1){
				display_symbol(x, y, " ");
			}
		}
	}
	// If offset > 0 then it will go to the other node in the liked list, and display the first offset collums in the linked list.
	if(offset){
		int (*NextBA)[WINDOW_WIDTH] = ((level -> pNext) -> node) -> levelSec;
	// Below will display from where the player is to the end of the current linked list.
		for(int (*pYEnd)[WINDOW_WIDTH] = NextBA + WINDOW_HEIGHT, y = 0; NextBA < pYEnd; NextBA++, y++){
			for(int *pStart = *NextBA, *pXEnd = pStart + offset, x = WINDOW_WIDTH - offset; pStart < pXEnd; pStart++, x++){
				if(*pStart == 1){
					display_symbol(x, y, " ");
				}
			}
		}
	}
}
// Very basic function used for readibility
void displayPlayer(const PLAYER *player){
	display_symbol(player -> x, player -> y, "O");
}
// Same here
void clearPlayer(const PLAYER *player){
	display_symbol(player -> x, player -> y, " ");
}
// This will check to see if the player can move somewhere, then will update the x or y with the corresponding movement the user wanted.
// This also only supports "wad" for the movement at the moment
void updatePlayerPosition(PLAYER *player, LEVEL *level, char direction, int *tick){// only supports wasd for movement at the moment
	int x = player -> x;
	int y = player -> y;
	switch(direction){
//		case 'w':	
//			if(y + 1 == level[player -> offset]){// If the player is thouching the ground
//				player -> y -= player -> jumpHeight;
//				*tick = 0; // Resets the tick so there isn't a chance when player jumps they will immedietly fall
//			}
//		break;
		case 'a':	
			if(x > 0 && level[player -> offset - 1] != y){// if the player is not at screen edge and there is not a block to the left			
				if(x >= WINDOW_WIDTH * LEVEL_MOVE_PERCENT || player -> offset <= (WINDOW_WIDTH * LEVEL_MOVE_PERCENT))//Player can only move to certain percent of level
					player -> x = x - 1;
				if(player -> offset >= (WINDOW_WIDTH * LEVEL_MOVE_PERCENT && player -> offset > (WINDOW_WIDTH * LEVEL_MOVE_PERCENT)))//This part only needed because player starts at the very left.
					player -> offset -= 1;
				}
		break;
		case 'd':	
			if(level[player -> offset + 1] != y){//If there isn't a block to the right
				if(x <= WINDOW_WIDTH * (1 - LEVEL_MOVE_PERCENT))// Player can only move to certain perceont of level
					player -> x = x + 1;
					player -> offset += 1;
				}
		break;
	}
}
// This will make the player fall down to either the ground or to their death. It also will display the player's new spot
void gravity(PLAYER *player, const int *level, int *tick){
	if((*tick)++ == TICK_RESET){
		if((player -> y + 1) != *(level + player -> offset)){
			if(checkInBounds(player)){
				clearPlayer(player);
			}
			player -> y += 1;
			if(checkInBounds(player))
				displayPlayer(player);
			}
		*tick = 0;
	}
}
// returns true if the player is in the bounds, else it will be false
int checkInBounds(const PLAYER *player){
	if(player -> y <= WINDOW_HEIGHT && player -> y >= 0){
		return 1;
	}
	return 0;
}
// Generates a random integer from the range [first, last]
int randint(int first, int last)
{
   static int firstTime = 1;
   int amountOfNumbers;
   if (firstTime == 1){
      /*first time in this function, seed the random number generator */
       firstTime = 0;
	   srand(time(NULL));
   }
   amountOfNumbers = last - first + 1;
   return(rand() % amountOfNumbers + first);
}

void devToolDisplay2dArray(LEVEL *level){
	int (*bA)[WINDOW_WIDTH] = (level -> node) -> levelSec;
	for(int (*pYEnd)[WINDOW_WIDTH] = bA + WINDOW_HEIGHT, y = 0; bA < pYEnd; bA++, y++){
		for(int *pStart = *bA, *pXEnd = pStart + WINDOW_WIDTH, x = 0; pStart < pXEnd; pStart++, x++){
			printf("%d ", *pStart);
			if(*pStart == 1){
			}
		}
		putchar('\n');
	}
	putchar('\n');
	putchar('\n');
}
