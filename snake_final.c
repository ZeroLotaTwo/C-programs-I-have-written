/*
Trying to make snake program
*/

#include<stdio.h>
#include<conio.h>
#include <stdlib.h>
#include <windows.h>

#define LEFT 1
#define RIGHT 3
#define UP 2
#define DOWN 4
#define FOOD 5
#define SCREEN_SIZE 25

//int mechanics();
void firstTimeDisplay();
int getRand();
void snakeHeadMovement(int *x, int *y, int direction, int snakeMem[][SCREEN_SIZE]);
void snakeBodyMovement(int *x, int *y, int snakeMem[][SCREEN_SIZE]);
int foodGen(int *x, int *y, int snakeMem[][SCREEN_SIZE], int food_amount);
int playerInput ();
void display_symbol(int x, int y, const void *display);

int main(){
	int x,y,direction,a,b,r1,r2,win,food_amount,tick_rate;
	char key;
	int snakeMem[SCREEN_SIZE][SCREEN_SIZE] = {{0}};
	x=y=SCREEN_SIZE/3;
	a=SCREEN_SIZE/3-1;//(a,b) are cord variables for the the second snake
	b=SCREEN_SIZE/3;
	win=((SCREEN_SIZE-1)*(SCREEN_SIZE-1))-2;
	food_amount=1;
	tick_rate=0;
	direction=RIGHT;
	snakeMem[b][a]=RIGHT;
	r1=SCREEN_SIZE/2;
	r2=SCREEN_SIZE/2;
	snakeMem[r2][r1]=FOOD;
	firstTimeDisplay();
	while((!(snakeMem[y][x]) || (snakeMem[y][x]==5)) && ((x!=0) && (x!=SCREEN_SIZE) && (y!=0) && (y!=SCREEN_SIZE))&&win>=food_amount){
		key = _kbhit() ? _getch() : 0;
		if (key == 'w' || key == 'a' || key == 's' || key == 'd'){
			direction=playerInput(key);
		}
		if (tick_rate>=5000){
			snakeHeadMovement(&x, &y, direction, snakeMem);
			if(!(snakeMem[y][x]==FOOD)){
				snakeBodyMovement(&a, &b, snakeMem);
	    	}else if(snakeMem[y][x]==5){
	    		food_amount = foodGen(&r1, &r2, snakeMem, food_amount);
		 }
			//the end of it the second snake thing
			display_symbol(x, y, "O");
	    	tick_rate=0;
		}
		tick_rate++;
	}
	system("cls");
	printf("You got a length of %d\n", food_amount);
	printf("THANKS FOR PLAYING!!!\n");
	system("pause");
	return 0;
}
void snakeHeadMovement(int *x, int *y, int direction, int snakeMem[][SCREEN_SIZE]){
	if (direction==UP){
			snakeMem[*y][*x]=direction;
			(*y)--;
			}else if (direction==LEFT){
				snakeMem[*y][*x]=direction;
				(*x)--;
			}else if (direction==DOWN){
				snakeMem[*y][*x]=direction;
				(*y)++;
			}else if(direction==RIGHT){
				snakeMem[*y][*x]=RIGHT;
				(*x)++;
			}
	
	
}

void snakeBodyMovement(int *x, int *y, int snakeMem[][SCREEN_SIZE]){
	if(snakeMem[*y][*x]==UP){
					snakeMem[*y][*x]=0;
					(*y)--;
					display_symbol(*x, *y, " ");
				}else if(snakeMem[*y][*x]==DOWN){
					snakeMem[*y][*x]=0;
					(*y)++;
					display_symbol(*x, *y, " ");
				}else if(snakeMem[*y][*x]==RIGHT){
					snakeMem[*y][*x]=0;
					(*x)++;
					display_symbol(*x, *y, " ");
				}else if(snakeMem[*y][*x]==LEFT){
					snakeMem[*y][*x]=0;
					(*x)--;
					display_symbol(*x, *y, " ");
		    	}

	
}
int foodGen(int *x, int *y, int snakeMem[][SCREEN_SIZE], int food_amount){
	int win = ((SCREEN_SIZE-1)*(SCREEN_SIZE-1))-2;
	snakeMem[*y][*x]=0;
	do{ //I took a short cut instead of figuring out a smart way of getting the food to not spawn on the snake, instead I used brute force to find a spot for the food.
		*x=getRand(1,SCREEN_SIZE-1);// 1 and SCREEN_SIZE-1 keeps it from touching the borders
		*y=getRand(1,SCREEN_SIZE-1);
	}while(win>=food_amount&&snakeMem[*y][*x]);//this checks if the food spawns on the snake, and the win condition is so it doese not get stuck at the end of snake
	snakeMem[*y][*x]=FOOD;
	display_symbol(*x, *y, "*");
	return ++food_amount;
}

void firstTimeDisplay(void){
	int i,j;
    static int firstTime2 = 1;
	if (firstTime2 == 1){
       firstTime2 = 0;
       for(i=0;i<SCREEN_SIZE;i++){
       		display_symbol(i, 0, "-");
	   }
	   for(j=0;j<SCREEN_SIZE;j++){
	   		display_symbol(0, j, "|");
		}
		for(i=0;i<SCREEN_SIZE;i++){
			display_symbol(i, SCREEN_SIZE, "-");
	   }
	   for(j=0;j<SCREEN_SIZE;j++){
	   		display_symbol(SCREEN_SIZE, j, "|");
		}
		display_symbol(SCREEN_SIZE, j, "|");
	}
		display_symbol(SCREEN_SIZE/2, SCREEN_SIZE/2, "*");
}

void display_symbol(int x, int y, const void *display){
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD pos = {x, y};
	SetConsoleCursorPosition(hConsole, pos);
	WriteConsole(hConsole, display, 1, NULL, NULL);
}
int getRand(int first, int last)
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

int playerInput (int key){
	int direction;
	if (key=='w'){
			direction=UP;
		}else if (key=='a'){
			direction=LEFT;
		}else if (key=='s'){
			direction=DOWN;
		}else if(key=='d'){
			direction=RIGHT;
		}
	return (direction);
}

	
