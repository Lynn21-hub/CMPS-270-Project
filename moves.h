#ifndef MOVES_H
#define MOVES_H

#include "battleship.h"

// Human player functions
int fire(Player *attacker, Player *defender, int trackingDifficulty);
void radar(Player *attacker, Player *defender);
void smokeScreen(Player *player) ;
void artillery(Player *attacker, Player *defender, int trackingDifficulty);
void torpedo(Player *attacker, Player *defender, int trackingDifficulty);


//bot functions 
void calculateHeatmap(Player *bot, Player *defender, int heatmap[GRID_SIZE][GRID_SIZE]);
void chooseBestTarget(int heatmap[GRID_SIZE][GRID_SIZE], int *row, int *col, Player *bot);
void addAdjacentCells(Player *bot, int row, int col);
int botFire(Player *bot, Player *defender);
void determineDirection(Player *bot, Player *defender);
void adjustHeatmapOnMiss(Player *bot, int row, int col, Player *defender);

void botRadar(Player *bot, Player *defender);
void botSmokeScreen(Player *bot);
void botArtillery(Player *bot, Player *defender, int trackingDifficulty);
void botTorpedo(Player *bot, Player *defender, int trackingDifficulty);


#endif // MOVES_H
