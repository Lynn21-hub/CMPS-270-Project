#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define gridSize 10

char mode[5] = "easy";

void Fire (char grid[gridSize][gridSize], int row , int col){

    if (mode == "easy") 
    {   if (grid[row][col] == 'S'){
            printf("Hit.\n");
            grid[row][col] = 'x';
        }
        else{
            printf("Miss.\n");
            grid[row] [col] = 'o';
        }
    }
    else{
        if (grid[row][col] == 'S'){
            printf("Hit.\n");
            grid[row][col] = 'x';
        }
        else{
            printf("Miss.\n");
            //Do not update the grid in hard mode;
        }
    }
}