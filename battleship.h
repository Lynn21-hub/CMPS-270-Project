// battleship.h
#ifndef BATTLESHIP_H
#define BATTLESHIP_H

#define MAX_NAME_LENGTH 50
#define GRID_SIZE 10
#define TOTAL_SHIPS 4
#define MAX_SHIP_SIZE 5 // Maximum size of any ship
#define WATER '~'
#define SHIP 'S'
#define HIT '*'
#define MISS 'o'

//ship struct
typedef struct {
    char name[20]; // Fixed-size array to store the ship's name
    int size;
    int hits;
    int positions[MAX_SHIP_SIZE][2]; // Store the positions of the ship parts
    int sunk; // 0 = not sunk, 1 = sunk
} Ship;

//player struct
typedef struct {
    char name[MAX_NAME_LENGTH];
    char grid[GRID_SIZE][GRID_SIZE];
    Ship fleet[TOTAL_SHIPS];
    int radarSweepsUsed;
    int smokeScreensAvailable;
    int artilleryAvailable;
    int torpedoAvailable;
    int shipsRemaining;
    int smokeScreenGrid[GRID_SIZE][GRID_SIZE]; // 1 if smoke screen is active on the cell
    int shipsSunk;          // Number of ships the player has sunk
    int smokeScreensUsed;   // Number of smoke screens the player has used
} Player;



#endif
