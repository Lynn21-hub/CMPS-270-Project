void torpedo( char opponentGrid[GRID_SIZE][GRID_SIZE], char target, int* sink){
    int hit = 0;

    if(isalpha(target)){
        int col = toupper(target) - 'A'; // col to index 
        for(int i=0;i<GRID_SIZE;i++){
            if(opponentGrid[i][col] == 'S'){
                opponentGrid[i][col] = 'X'; // hit 
                hit = 1;
                (*sink)++;
            } else if(opponentGrid[i][col] == '~'){
                opponentGrid[i][col] = 'O'; // miss
            }
        }
        if(hit)
            printf("Torpedo attack! hit on column %c\n", target);
        else
            printf("Topedo Attack Missed! on column %c\n", target);

    } else if(isdigit(target)){
            int row = target - '1'; // row to index 
            for(int i=0;i<GRID_SIZE;i++){
                if(opponentGrid[row][i] == 'S'){
                    opponentGrid[row][i] = 'X';
                    hit = 1;
                    (*sink)++;
                } else if(opponentGrid[row][i] == '~'){
                    opponentGrid[row][i] = 'O';
                }

            }
            if(hit)
                printf("Torpedo attack! hit on row %c\n", target);
            else 
                printf("Topedo Attack Missed! on column %c\n", target);
    }

}
void unlockTorpedo(int shipSunk, char opponentGrid[GRID_SIZE][GRID_SIZE]){
    if(shipSunk == 3){
        printf("Torpedo Unlocked! Use in next turn\n");
        char target;
        printf("Enter a target column or row: ");
        scanf(" %c", &target);

        int sink = 0;
        torpedo(opponentGrid, target,&sink);
    }
}
