#ifndef MOVES_H
#define MOVES_H

#include "battleship.h"

// Function prototypes
int fire(Player *attacker, Player *defender, int trackingDifficulty);
void radar(Player *attacker, Player *defender);
void smokeScreen(Player *player);
void artillery(Player *attacker, Player *defender);
void torpedo(Player *attacker, Player *defender);

// Helper functions (if needed)
Ship* findShipAtPosition(Player *player, int row, int col);
int parseCoordinates(char *input, int *row, int *col);
void unlockAdvancedMoves(Player *attacker, Player *defender);

#endif // MOVES_H
