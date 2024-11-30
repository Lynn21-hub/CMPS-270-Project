// battleship.h
#include<stdbool.h>
#ifndef BATTLESHIP_H
#define BATTLESHIP_H

#define MAX_NAME_LENGTH 50
#define GRID_SIZE 10
#define TOTAL_SHIPS 4
#define MAX_RADAR_SWEEPS 3
#define MAX_SHIP_SIZE 5 // Maximum size of any ship
#define SMOKE_SCREEN_SIZE 2
#define MAX_PENDING_ATTACKS 100
#define SHIP_COUNT 4
#define WATER '~'
#define SHIP 'S'
#define HIT '*'
#define MISS 'o'
#define SMOKE '1' // Represents a smoke screen
#define NOT_SWEEPED 0
#define SWEEPED_WITH_SHIP 1
#define SWEEPED_NO_SHIP 2

typedef enum { Hunt, Target, lockDirection } Phase;

//ship struct
typedef struct {
    char name[20]; // Fixed-size array to store the ship's name
    int size;
    int hits;
    int positions[MAX_SHIP_SIZE][2]; // Store the positions of the ship parts
    bool sunk; // false = not sunk, true = sunk
}Ship;
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
    int isBot;                                          
    char trackingGrid[GRID_SIZE][GRID_SIZE];
    bool sweptAreas[GRID_SIZE - SMOKE_SCREEN_SIZE + 1][GRID_SIZE - SMOKE_SCREEN_SIZE + 1]; // Tracks swept 2x2 areas
    Phase currentPhase;          // Current phase of the bot
    int lastHitRow;              // Last hit row
    int lastHitCol;              // Last hit column
    int directionRow;            // Direction row for Lock Direction Phase
    int directionCol;            // Direction column for Lock Direction Phase
    int pendingAttacks[MAX_PENDING_ATTACKS][2]; // Queue for pending attacks
    int pendingAttackCount;     // Number of pending attacks
    int turnsSinceLastRadarSweep; // Tracks turns since the bot's last radar sweep
    bool radarUsedAgainstBotLastTurn; // Flag to track Radar Sweep usage against the bot
    int lastHeatmap[GRID_SIZE][GRID_SIZE]; // Last calculated heatmap
    int moveCount; // Total moves made by the player (useful for tracking periodic actions)
    int sweepResults[GRID_SIZE][GRID_SIZE];
} Player;



#endif 
