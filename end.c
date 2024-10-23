int currentPlayer = firstPlayer;
int gameOver = 0;
while(!gameOver){
    if(currentPlayer == 1){
        printf("%s's turn", player1);
        if(takeTurn(player1,grid2,difficulty)){
            printf("%s wins! All of %s's ships are sunk.", player1, player2);
            gameOver = 1;
        }
       currentPlayer = 2; // next turn 
    } else {
        printf("%s's turn", player2);
        if(takeTurn(player2,grid1,difficulty)){
            printf("%s wins! All of %s's ships are sunk.\n", player2, player1);
            gameOver = 1;
        }
        currentPlayer = 1; 
    }
    
}
