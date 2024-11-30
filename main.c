#include <stdio.h>
#include <stdlib.h>
#include<stdbool.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include "battleship.h"
#include "moves.h"
#include "helper.h"

int main() {
    Player player1, player2;
    int difficulty, firstPlayer;
    int gameOver = 0;

    // Initialize players
    initializePlayer(&player1, 0); // Human player
    initializePlayer(&player2, 1); // Bot player

    // Get player names
    getPlayerNames(&player1, &player2);

    // Choose tracking difficulty
    difficulty = getDifficultyLevel();

    // Randomly select first player
    firstPlayer = chooseFirstPlayer();
    printf("Player %s will go first!\n", (firstPlayer == 1) ? player1.name : player2.name);

    // Clear screen to preserve ship secrecy
    clearScreen();

    // Place ships for Player 1 (Human)
    printf("%s, place your ships:\n", player1.name);
    placeShips(&player1);
    clearScreen();

    // Place ships for Player 2 (Bot or Human)
    if (!player2.isBot) { // If player2 is human
        printf("%s, place your ships:\n", player2.name);
        placeShips(&player2);
    } else { // If player2 is bot
        placeShipsBot(&player2); // Implement a function to place ships for bot automatically
    }
    clearScreen();

    // Game loop
    int currentPlayer = firstPlayer;
    while (!gameOver) {
        if (currentPlayer == 1) {
            printf("%s's turn:\n", player1.name);
            if (takeTurn(&player1, &player2, difficulty)) {
                printf("All enemy ships have been sunk! %s wins!\n", player1.name);
                gameOver = 1;
            }
            currentPlayer = 2; // Switch turns
        } else {
            printf("%s's turn:\n", player2.name);
            if (takeTurn(&player2, &player1, difficulty)) {
                printf("All enemy ships have been sunk! %s wins!\n", player2.name);
                gameOver = 1;
            }
            currentPlayer = 1; // Switch turns
        }
    }

    return 0;
}