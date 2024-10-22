#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define gridSize 10

void RadarSweep (char grid[gridSize][gridSize], int row, int col){
    int found =0;
    for (int i= row; i<row +2 ; i++){
        for (int j = col; j< col +2; j++){
            if (i<gridSize && j < gridSize && grid[i][j] == 'S'){
                found =1;
                break;
            }
        }
        if (found) break;
    }

    if (found){
        printf("Enemy ships found.\n");
    }else{
        printf("No enemy ships found.\n");
    }
    
}