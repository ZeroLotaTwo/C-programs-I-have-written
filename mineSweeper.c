/*
MINESWEEPER
Created by Justis Rothbauer
*/

//#define DEBUG

#define BOARD_HEIGHT 9
#define BOARD_WIDTH 9
// 8 is the max you should do. Or if it's higher it'll break, and lower the randomness sucks.
#define PERCENT_PRECISION 8

#include <stdlib.h>
#include <time.h>
#include <math.h>

#include <windows.h>
#include <conio.h>
#include <string.h>
#include <stdio.h>
typedef struct{
	int numBombs;
	int height, width;
	int tilesLeft;
	int board[]; // This will be technically a two dimensial array, but I have to work with it as one dimesional. For development sounds awful. NOTE: not that bad
}BOARD;

// Used to specify constants to deterMINE things on the board
enum{
	EMPTY = 0, // The program does depend on this value being 0. So I would not recomend changing it.
	MINE = 9,
	MINE_VISUAL = '*',
	NOT_GUESSED_ADDITIVE = 10, // To check if something is not guessed. If the num in bigger than 10 but less than 100 then it hasn't been guessed yet.
	FLAG = 100, // This just adds 100 to the tile and locks it.
	FLAG_VISUAL = '!',
	NOT_GUESSED_VISUAL = '#'
};

BOARD* createBoard(int height, int width, int numBombs);
int* getPointer(BOARD *Board, int row, int col);
int MINECreationBool(int tilesLeft, int numBombsLeft, int precision);
void spawnMINEs(BOARD *BOARD, int numBombs);
void initializeBombCountAroundTiles(BOARD *Board);
void printRawBoard(BOARD *Board);
int *cyclePointer(BOARD *Board, int row, int col, int cycle);
int numBombsAround(BOARD *Board, int currHeight, int currWidth);
void addBomb(BOARD *Board, int row, int col);
void removeBomb(BOARD *Board, int row, int col);
int pointerInList(int *pointer, int **list, int len);
void firstGuess(BOARD *Board, int row, int col);
int takeGuess(BOARD *Board, int row, int col); // returns True if you hit a MINE, returns false if you don't. Subtracts 10 to reveal more of the board
void flag(BOARD *Board, int row, int col);
long long unsigned int randint(long long unsigned int low, long long unsigned int high);
void printRawBoard(BOARD *Board);
void freeBoard(BOARD *Board);
// Section below is funcitons for how the user will interact with the board and play
void display_symbol(int x, int y, const void *display);// For display use a string, I don't know why but for the function to work the arg has to be void.
void moveCursor(BOARD *Board, int *y, int *x, char playerInput);
void display_symbol(int x, int y, const void *display);
void printBoard(BOARD *Board);

int main(void){
	BOARD *Board = createBoard(BOARD_HEIGHT, BOARD_WIDTH, 10);
//	printf("NumBombs: %d\n", numBombsAround(Board, 1, 0));
	printBoard(Board);
//	printRawBoard(Board);
		const char *validInputStr = "wasd\n";
	int x = 0;
	int y = 0;
//	freeBoard(Board);
	int bombFound = 0;
	int firstGuessBool = 1;
	printRawBoard(Board);
	while(!bombFound && (Board -> tilesLeft != 0)){
		char playerInput = _kbhit() ? tolower(getch()) : '?';// This will get and store any key the player inputs instantly.
		if(strchr(validInputStr,playerInput)){
			moveCursor(Board, &y, &x, playerInput);
		}else if(playerInput == 'e'){
			if(firstGuessBool){
				firstGuess(Board, y, x);
				firstGuessBool = 0;
			}
			bombFound = takeGuess(Board, y, x);
			system("cls");
			printBoard(Board);
//			printRawBoard(Board);
		}else if(playerInput == 'f'){
			flag(Board, y, x);
			system("cls");
			printBoard(Board);
//			printRawBoard(Board);
		}// End if valid input
	}// End of game loop
	if(bombFound)
		printf("\nYOU DIED XD\n");
	else
		printf("\nYOU WON!!!\n");
	freeBoard(Board);
	system("pause");
	return 0;
}

// Returns an initialized board with everything set up.
BOARD* createBoard(int height, int width, int numBombs){
	// This section will go around the board and place the MINEs with time complexity of O(n);
	BOARD *Board = (BOARD*)malloc(sizeof(BOARD) + sizeof(int) * (height * width));
	Board -> numBombs = numBombs;
	Board -> height = height;
	Board -> width = width;
	Board -> tilesLeft = height * width - numBombs;
	
	spawnMINEs(Board, numBombs);// This spawns the MINEs and initilizes the board to EMPTY if the is no MINE there.
	
	// This section will will count in the tiles nearby how many MINEs are around that tile.
	initializeBombCountAroundTiles(Board);
	
	return Board;
}

// Works like indexing with Board[row][col]
int* getPointer(BOARD *Board, int row, int col){
	return (Board -> board) + (row * Board -> width + col);
}

int MINECreationBool(int tilesLeft, int numBombsLeft, int precision){
	long long unsigned int increaseAMT = pow(10, precision);
	long long unsigned int MINEToBoardPercent = ((double)(numBombsLeft) / (double)(tilesLeft)) * increaseAMT; //Converts from normal number to percentage
#ifdef DEBUG
	printf("BOMBSLEFT: %d. PERCENT: %I64u. TilesLeft: %d\n", numBombsLeft, MINEToBoardPercent, tilesLeft);
#endif
	if(randint(1, increaseAMT) <= MINEToBoardPercent){
		return 1;
	}
	return 0;
}

// Ian's MINE creation bool
//int MINECreationBool(int tilesLeft, int numBombsLeft, int precision){
//	long long unsigned int increaseAMT = pow(10, precision);
//	double chance = ((double)tilesLeft/(double)(numBombsLeft * numBombsLeft)) * increaseAMT;
//	if(randint(1, increaseAMT) <= chance){
//		return 1;
//	}
//	return 0;
//	
//}

//void spawnMINEs(BOARD *Board, int numBombs){
//	int numBombsLeft = numBombs;
//	int tilesLeft = Board -> height * Board -> width;
//	for(int *pStart = Board -> board, *pEnd = pStart + Board -> height * Board -> width; pStart < pEnd; pStart++){
//		if(MINECreationBool(tilesLeft, numBombsLeft, PERCENT_PRECISION)){
//			*pStart = MINE + NOT_GUESSED_ADDITIVE;
//			numBombsLeft--;
//		}else{	
//			*pStart = EMPTY + NOT_GUESSED_ADDITIVE;
//		}
//	}
//}


void spawnMINEs(BOARD *Board, int numBombs){
	int width = Board -> width;
	int height = Board -> height;
	for(int i = numBombs; i > 0; i--){
		int *spot = getPointer(Board, randint(0, height - 1), randint(0, width - 1));
		while(*spot == MINE + NOT_GUESSED_ADDITIVE){
			spot = getPointer(Board, randint(0, height - 1), randint(0, width - 1));
		}
		*spot = MINE + NOT_GUESSED_ADDITIVE;
	}
	for(int *pStart = Board -> board, *pEnd = pStart + Board -> height * Board -> width; pStart < pEnd; pStart++){
	if(*pStart != MINE + NOT_GUESSED_ADDITIVE){
		*pStart = EMPTY + NOT_GUESSED_ADDITIVE;
		}
	}
	
	
}

// Returns the amount of bombs around the tile
int numBombsAround(BOARD *Board, int row, int col){
	int bombCount = 0;
	short int rowStart = (row - 1 > 0) ? row - 1 : 0;
	short int rowEnd = (row + 1 <= Board -> width) ? row + 1 : Board -> width;
	short int colStart = (col - 1 > 0) ? col - 1 : 0;
	short int colEnd = (col + 1 <= Board -> height) ? col + 1 : Board -> height;
	for(int i = colStart; i <= colEnd; i++){
		for(int j = rowStart; j <= rowEnd; j++){
			int *bombSpot = getPointer(Board, i, j);
			if(*bombSpot == MINE + NOT_GUESSED_ADDITIVE)
				bombCount++;
		}
	}	
//	for(int i = 0; i < 9; i++){
//		int *bombSpot = cyclePointer(Board, row, col, i);
//		if(bombSpot)
//			if(*bombSpot == MINE + NOT_GUESSED_ADDITIVE)
//				bombCount++;
//	}
//	printf("BombCount Col: %d, Row: %d, numBombs: %d\n", col, row, bombCount);
	return bombCount;
}

void initializeBombCountAroundTiles(BOARD *Board){
	int row = 0;
	int col = 0;
	int height = Board -> height;
	int width = Board -> width;
	for(int *pStart = Board -> board, *pEnd = pStart + height * width; pStart < pEnd; pStart++){
//		printf("col: %d, row: %d\n", col, row);
//		printf("pStart: %d\n", *pStart);
		if(*pStart != MINE + NOT_GUESSED_ADDITIVE){
			*pStart += numBombsAround(Board, col, row);
		}
		if(++col == width){
			col = 0;
			row++;
		}
	}
}

// This will cycle through a 3x3 square grid. And return the next pointer. When the pointer is not in the range it returns NULL
// the for cycle use a for loop. go through 0-8. That will cover the 3x3 area. 
// This starts at the top left and reads left to right.
int *cyclePointer(BOARD *Board, int row, int col, int cycle){
	// This starts by getting the pointer in the top left conrer then goes to the pointer spot based on the cycle
	int *bA = getPointer(Board, row - 1, col - 1) + (cycle / 3 * Board -> width) + cycle % 3;
	if((bA < Board -> board) || (bA > Board -> board + Board -> width * Board -> height))// checks to see if it's outside of the array scope
		return NULL;
	if((row == 0 && cycle < 3) || (col == 0 && cycle % 3 == 0) || (col == Board -> width - 1 && cycle % 3 == 2))// this checks to see if it's in the scope of the square
		return NULL;
	return bA;
}

void addBomb(BOARD *Board, int row, int col){	
	for(int i = 0; i < 9; i++){
		int *spot = cyclePointer(Board, row, col, i);
		// if it's in the middle of the 3x3
		if(spot){
			if(i == 4){
				*spot = MINE + NOT_GUESSED_ADDITIVE;
			}else{
				if(*spot != MINE + NOT_GUESSED_ADDITIVE && *spot > NOT_GUESSED_ADDITIVE)
					*spot += 1;
			}		
		}
	}
}

void removeBomb(BOARD *Board, int row, int col){
	for(int i = 0; i < 9; i++){
		int *spot = cyclePointer(Board, row, col, i);
		// if it's in the middle of the 3x3
		if(spot){
			if(i == 4){
				*spot = EMPTY + NOT_GUESSED_ADDITIVE;
			}else{
				if(*spot != MINE + NOT_GUESSED_ADDITIVE && *spot > NOT_GUESSED_ADDITIVE)
					*spot -= 1;
			}		
		}
	}
}


int pointerInList(int *pointer, int **list, int len){
	for(int **pStart = list, **pEnd = list + len; pStart < pEnd; pStart++){
		if(*pStart == pointer){
//			printf("IN\n");
//			sleep(1);
			return 1;
		}
	}
//	printf("OUT\n");
//	sleep(1);
	return 0;
}

// Scuffed and don't touch, I believe it fragile. Unless you know what doing.
// This function is also unsafe. Currenlty if there are only 8 open and the rest are MINEs, it will put one of the bombs out of the boards scope and overright something.
void firstGuess(BOARD *Board, int row, int col){
	system("cls");
	int x = 0;
	int y = 0;
	int width = Board -> width;
//	int height = Board -> height;
	int *pastPointers[9];
	for(int i = 0; i < 9; i++){
		pastPointers[i] = cyclePointer(Board, row, col, i);
	}
	for(int i = 0; i < 9; i++){
		printf("%d\n", *pastPointers[i]);
	}
	int please;
	scanf("%d", &please);
	for(int i = 0; i < 9; i++){
		if(pastPointers[i]){
			if(*pastPointers[i] == MINE + NOT_GUESSED_ADDITIVE){
				while(*getPointer(Board, y, x) == MINE + NOT_GUESSED_ADDITIVE || pointerInList(getPointer(Board, y, x), pastPointers, i + 1)){
					x++;
					if(x >= width){
						y++;
						x = 0;
					}
				}
				removeBomb(Board, row - 1 + i / 3, col - 1 + i % 3);
				addBomb(Board, y, x);
			}
		}
	}
}

// Returns 1 if there is a counter or a MINE, returns 0 if tiles is empty.
// This will update the board and will reveal open up tiles that arn't MINE. 
int takeGuess(BOARD *Board, int row, int col){
	int *guess = getPointer(Board, row, col);
	// Will end recursion if the tile has not been guessed yet. It does not check already checked empty tiles.
	if(*guess == MINE + NOT_GUESSED_ADDITIVE){
		return 1;
	}else if(*guess > NOT_GUESSED_ADDITIVE + EMPTY){
		*guess -= NOT_GUESSED_ADDITIVE;
		(Board -> tilesLeft)--;
		return 0;
	}else if(*guess >= FLAG || (*guess > EMPTY && *guess <= 8)){
		return 0;
	}
	for(int i = 0; i < 9; i++){
		int *spot = cyclePointer(Board, row, col, i);
		if(spot){
			if(*spot == EMPTY + NOT_GUESSED_ADDITIVE){
				*spot -= NOT_GUESSED_ADDITIVE;
				(Board -> tilesLeft)--;
				takeGuess(Board, row - 1 + i / 3, col - 1 + i % 3);	
			}
			if(*spot >= NOT_GUESSED_ADDITIVE && *spot != MINE + NOT_GUESSED_ADDITIVE && *spot < FLAG){
				*spot -= NOT_GUESSED_ADDITIVE;
				(Board -> tilesLeft)--;
			}
		}
	}
	return 0;
}

void flag(BOARD *Board, int row, int col){
	int *spot = getPointer(Board, row, col);
	if(*spot >= FLAG){
		*spot -= FLAG;
		(Board -> numBombs)++;
	}else if(*spot >= NOT_GUESSED_ADDITIVE){
		*spot += FLAG;
		(Board -> numBombs)--;
		
	}
}

// This just isn't a random int anymore this wack.
// COMMENT UPDATE: This function pretty cool and I understand how it works. It gens only a 8 byte int so it just gens the number 8 bits at a time and shifts it to their location in the 64 bit int.
long long unsigned int randint(long long unsigned int low, long long unsigned int high){
	static int firstTime = 1;
	if(firstTime){
		srand(time(NULL));
		firstTime = 0;
	}
    long long unsigned int randomNum = ((unsigned long long int)rand() << 45) | ((unsigned long long int)rand() << 30) | ((unsigned long long int)rand() << 15) | (unsigned long long int)rand();
	return (randomNum % (high - low + 1) + low);
}
// This is a debugging tool to see what the 2d array looks like.
void printRawBoard(BOARD *Board){
	int widthLeft = Board -> width;
	for(int *pStart = Board -> board, *pEnd = pStart + Board -> height * Board -> width; pStart < pEnd; pStart++){
		printf("%3d", *pStart);
		if(--widthLeft == 0){
			widthLeft = Board -> width;
			putchar('\n');
		}
	}
	putchar('\n');
	putchar('\n');
	putchar('\n');
}
void freeBoard(BOARD *Board){
	free(Board);
}

// Section below is funcitons for how the user will interact with the board and play
void display_symbol(int x, int y, const void *display){
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD pos = {x, y};
	SetConsoleCursorPosition(hConsole, pos);
	WriteConsole(hConsole, display, 1, NULL, NULL);
}

void moveCursor(BOARD *Board, int *y, int *x, char playerInput){
	switch(playerInput){
		case 'w':
			*y = *y - 1 * (*y > 0);
		break;
		
		case 'a':
			*x = *x - 1 * (*x > 0);
		break;
		
		case 's':
			*y = *y + 1 * (*y < Board -> height - 1); 
		break;
		
		case 'd':
			*x = *x + 1 * (*x < Board -> width - 1);
		break;
	}// End of switch
	char str[2];// to store what to display
	int value = *getPointer(Board, *y, *x);
	if(value >= FLAG){
		sprintf(str, "%c", FLAG_VISUAL);
	}else if(value >= NOT_GUESSED_ADDITIVE){
		sprintf(str, "%c", NOT_GUESSED_VISUAL);
	}else if(value == MINE){
		sprintf(str, "%c", MINE_VISUAL);
	}else{
		sprintf(str, "%d", value);
	}
	display_symbol(*x * 2, *y, str);
}

void printBoard(BOARD *Board){
	int widthLeft = Board -> width;
	for(int *pStart = Board -> board, *pEnd = pStart + Board -> height * Board -> width; pStart < pEnd; pStart++){
		char str[2];
		if(*pStart >= FLAG){
			sprintf(str, "%c", FLAG_VISUAL);
		}else if(*pStart == MINE){
			sprintf(str, "%c", MINE_VISUAL);
		}else if(*pStart >= NOT_GUESSED_ADDITIVE){
			sprintf(str, "%c", NOT_GUESSED_VISUAL);
		}else{
			sprintf(str, "%1d", *pStart);
		}
		printf("%-2s", str);
		if(--widthLeft == 0){
			widthLeft = Board -> width;
			putchar('\n');
		}
	}
	printf("\n\nBombs Left: %d\n\n", Board -> numBombs);
	printf("You use wasd for movement. E to guess. And F to place flag '!'\n");
	printf("And you select to the left of the cursor\n");
}
