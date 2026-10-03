// I think this will be bad version of the GMP library.

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct{
    int len;
    int num[];// This will be the binary representation
}INT;

char *getNum();
void loopToDivisable(char **pStart, int digit, int *total, int divisor, char **newNumSpot, int firstNum);
int divideByNum(char *num, int divisor);
//INT* add(INT* num1, INT* num2);
INT* convertToBi(char *num);
void printArray(char *arr, int len);
void printBi(INT *bin);

int main()
{
    char *num = getNum();
    if(num){
        INT *bin = convertToBi(num);
        printBi(bin);
        free(num);
    }
    system("pause");
    return 0;
}
// This whill get a string representation of the number.
// time complexity is O(n) where n is the number of digits in the number
char *getNum(){
    int len = 8;
    char *strNum = (char*)malloc(sizeof(char) * (len + 1));
    char temp;
    int i = 0;
    while((temp = getchar()) != '\n'){
        if(i >= len){
            len *= 2;
            strNum = (char*)realloc(strNum, (len + 1) * sizeof(char));
        }
        strNum[i++] = temp;
    }
    strNum[i] = 0;
    //validate the array to see if it's actually a number.
    int realNumBool = 1; // we start out assuming it is a number.
    if((strNum[0] != '-' && strNum[0] < '0') || strNum[0] > '9'){
        realNumBool = 0;
    }
    for(char *pStart = strNum + 1; *pStart != 0 && realNumBool; pStart++){
        if(*pStart < '0' || *pStart > '9'){
            realNumBool = 0;
        }
    }
    if(realNumBool){
        return strNum;
    }else{
        free(strNum);
        return NULL;
    }
}

// helper function for divideByNum.
// reads in digits until it can be divided by divisor
void loopToDivisable(char **pStart, int digit, int *total, int divisor, char **newNumSpot, int firstNum){
    while(*total / divisor == 0 && **pStart != 0){
        digit = *((*pStart)++) - '0';// increments pStart here!!!
        *total = *total * 10 + digit;
        if(firstNum){
            *((*newNumSpot)++) = '0'; // places the placeholder when it can't divide
        }
    }
}

// DANGER!!! This changes num to be it to be the result of num / divisor.
// O(n) where n is the number of digits.
int divideByNum(char *num, int divisor){
    int remainder = 0;
    char *newNumSpot = num; // This is the pointer for where it will place digit
    char *pStart = num;
    /*
    // firstNum is used to skip the first 0 in the division
    // because 12 / 2, the first digit 1 / 2 = 0 and we don't want that at the start.
    // But it's a bool that is true once you get past the first digit.
    */ 
    int firstNum = 0;
    for(; *pStart != 0; newNumSpot++){// goes through the string num
        int digit = *pStart++ - '0';
        int total = digit + remainder * 10;// this is the total if you append the next digit
        loopToDivisable(&pStart, digit, &total, divisor, &newNumSpot, firstNum);
        firstNum = 1;
        *newNumSpot = total / divisor + '0';
        remainder = total % divisor;
    } 
    // End of the string.
    if(pStart > newNumSpot){
        *newNumSpot = 0;
    }
    // printf("REMAINDER: %d\n", remainder);
//    for(char *pStart = num; *pStart != 0; pStart++){
//    	putchar(*pStart);
//	}
//	putchar('\n');
    return remainder;
}

//INT* add(INT* num1, INT* num2){
//    int carry = 0;
//    
//}

// This will convert the number to the binary number
///*
INT* convertToBi(char *num){
	if(num == NULL){
		return NULL;
	}
    int i = 0;
    for(; num[i] != 0; i++);
    int digitsAmt = log(10) * (i + 1) / log(2); // over estimate of base 2 digits
    int length = 16 * (digitsAmt/16 + 1);
    printf("LENGTH: %d, DIGITS: %d\n", length, i);
    INT *bin = (INT*)calloc(sizeof(INT) + (length * sizeof(int)), 1);
    if(bin){
        bin -> len = length;
        char *tellWhenZero = num;
        if(num[0] == '-'){
            (bin -> num)[0] = 1;
            num++;
            tellWhenZero++;
        }
        for(int *pEnd = (bin -> num) + length - 1; length > 0; pEnd--, length--){
        	int remainder = divideByNum(num, 2);
            *pEnd = remainder ^ (bin -> num)[0];
        }
    }
    return bin;
}
//*/
void printArray(char *arr, int len){
    for(int i = 0; i < len; i++){
        putchar(arr[i]);
    }
    putchar('\n');
}

void printBi(INT *bin){
    for(int *pStart = (bin -> num), *pEnd = pStart + (bin -> len); pStart < pEnd; pStart++){
        printf("%d",*pStart);
    }
    putchar('\n');
}
