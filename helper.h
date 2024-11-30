#ifndef UTILS_H
#define UTILS_H

#include "battleship.h"

void initializeGrid(char grid[GRID_SIZE][GRID_SIZE]);
void displayGrid(char grid[GRID_SIZE][GRID_SIZE], int showShips);
int getDifficultyLevel();
void getPlayerNames(Player *player1, Player *player2);
int chooseFirstPlayer();
void clearScreen();
int isPlacementValid(char grid[GRID_SIZE][GRID_SIZE], int row, int col, int length, char orientation);
void placeShips(Player *player);
int takeTurn(Player *currentPlayer, Player *opponent, int difficulty);
int allShipsSunk(Player *player);
void initializePlayer(Player *player, int botFlag);
void placeShipsBot(Player *player);
void initializeBot(Player *bot);


// Helper functions (if needed)
Ship* findShipAtPosition(Player *player, int row, int col);
int parseCoordinates(char *input, int *row, int *col);
void unlockAdvancedMoves(Player *attacker, Player *defender);
void clearScreen();

#endif
