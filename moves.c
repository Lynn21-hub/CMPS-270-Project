#include "moves.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

//fucntion to unlock advanced moves like torpedo and artillary 
void unlockAdvancedMoves(Player *attacker, Player *defender) {
    int shipsSunk = TOTAL_SHIPS - defender->shipsRemaining;

    if (shipsSunk == 1 && !attacker->artilleryAvailable) {
        attacker->artilleryAvailable = 1;
        printf("Artillery strike unlocked for your next turn!\n");
    }

    if (shipsSunk == 3 && !attacker->torpedoAvailable) {
        attacker->torpedoAvailable = 1;
        printf("Torpedo strike unlocked for your next turn!\n");
    }
}
//function to identify which ship was hit 
Ship* findShipAtPosition(Player *player, int row, int col) {
    for (int i = 0; i < TOTAL_SHIPS; i++) {
        Ship *ship = &player->fleet[i];
        for (int j = 0; j < ship->size; j++) {
            if (ship->positions[j][0] == row && ship->positions[j][1] == col) {
                return ship;
            }
        }
    }
    return NULL;
}
//this function handles inout validation 
int parseCoordinates(char *input, int *row, int *col) {
    if (strlen(input) < 2 || strlen(input) > 3) return 0;
    char colChar = toupper(input[0]);
    int rowNum = atoi(&input[1]);
    *col = colChar - 'A';
    *row = rowNum - 1;
    if (*col < 0 || *col >= GRID_SIZE || *row < 0 || *row >= GRID_SIZE) return 0;
    return 1;
}

// Function to clear the screen (simulate it with newlines)
void clearScreen() {
    #ifdef _WIN32
        system("cls"); // Windows command to clear screen
    #else
        system("clear"); // Unix/Linux command to clear screen
    #endif
}



//fire function 
int fire(Player *attacker, Player *defender, int trackingDifficulty) {
    char input[10];
    int row, col;

    // 1. Prompt for target coordinates
    printf("%s, enter the coordinates to fire at (e.g., B3): ", attacker->name);
    scanf("%s", input);

    // 2. Parse and validate coordinates
    if (!parseCoordinates(input, &row, &col)) {
        printf("Invalid coordinates. You lose your turn.\n");
        return 0; // End turn due to invalid input
    }

    // 3. Check if the cell has already been targeted
    char cell = defender->grid[row][col];
    if (cell == HIT || cell == MISS) {
        printf("You've already fired at this location. You lose your turn.\n");
        return 0; // End turn
    }

    // 4. Check for smoke screen
    if (defender->smokeScreenGrid[row][col]) {
        printf("Your shot was obstructed by a smoke screen. You miss!\n");
        // In Easy mode, you might still mark the cell as MISS
        if (trackingDifficulty == 1) {
            defender->grid[row][col] = MISS;
        }
        return 0; // Turn ends due to smoke screen obstruction
    }

    // 5. Determine hit or miss
    if (cell == SHIP) {
        // Handle hit
        printf("Hit!\n");
        defender->grid[row][col] = HIT;

        // 6. Update ship status
        Ship *hitShip = findShipAtPosition(defender, row, col);
        if (hitShip != NULL) {
            hitShip->hits++;
            if (hitShip->hits == hitShip->size && !hitShip->sunk) {
                hitShip->sunk = 1;
                defender->shipsRemaining--;
                printf("You have sunk the %s!\n", hitShip->name);

                // 7. Increment ships sunk by attacker
                attacker->shipsSunk++;

                // 8. Unlock advanced moves if applicable
                unlockAdvancedMoves(attacker, defender);
            }
        } else {
            // This should not happen; handle unexpected cases
            printf("Error: Could not find ship at the hit position.\n");
        }
    } else if (cell == WATER) {
        // Handle miss
        printf("Miss!\n");
        if (trackingDifficulty == 1) {
            defender->grid[row][col] = MISS; // Mark miss only in Easy mode
        }
    } else {
        // Handle any unexpected cell values
        printf("Error: Unexpected cell value '%c' at (%d, %d)\n", cell, row, col);
    }

    // 7. Check for game over
    if (defender->shipsRemaining == 0) {
        printf("All enemy ships have been sunk!\n");
        return 1; // Game over
    }

    return 0; // Game continues
}




     
void radar(Player *attacker, Player *defender) {
    // 1. Check radar sweeps remaining
    if (attacker->radarSweepsUsed >= 3) {
        printf("You have used all your radar sweeps. You lose your turn.\n");
        return;
    }

    // 2. Prompt for top-left coordinate
    char input[10];
    int row, col;
    printf("%s, enter the top-left coordinate for the radar sweep (e.g., B3): ", attacker->name);
    scanf("%s", input);

    // 3. Parse and validate coordinates
    if (!parseCoordinates(input, &row, &col)) {
        printf("Invalid coordinates. You lose your turn.\n");
        return; // Do not increment radarSweepsUsed on invalid input
    }

    // Ensure 2x2 area is within grid boundaries
    if (row < 0 || row >= GRID_SIZE - 1 || col < 0 || col >= GRID_SIZE - 1) {
        printf("Radar sweep area is out of bounds. You lose your turn.\n");
        return; // Do not increment radarSweepsUsed on invalid input
    }

    // 4. Check for smoke screens
    int smokeScreened = 0; // Correctly declared before loops

    for (int i = row; i <= row + 1; i++) {
        for (int j = col; j <= col + 1; j++) {
            if (defender->smokeScreenGrid[i][j]) {
                smokeScreened = 1;
                break; // Breaks out of the inner loop
            }
        }
        if (smokeScreened) {
            break; // Breaks out of the outer loop
        }
    }

    // 5. Scan the 2x2 area
    int enemyShipsFound = 0; // Declare before loops

    if (!smokeScreened) {
        for (int i = row; i <= row + 1; i++) {
            for (int j = col; j <= col + 1; j++) {
                if (defender->grid[i][j] == SHIP) {
                    enemyShipsFound = 1;
                    break; // Breaks out of the inner loop
                }
            }
            if (enemyShipsFound) {
                break; // Breaks out of the outer loop
            }
        }
    }

    // 6. Inform the attacker
    if (smokeScreened) {
        printf("No enemy ships found.\n");
    } else if (enemyShipsFound) {
        printf("Enemy ships found.\n");
    } else {
        printf("No enemy ships found.\n");
    }

    // 7. Update radar sweeps used
    attacker->radarSweepsUsed++;
}




void smokeScreen(Player *player) {
    // 1. Calculate allowed smoke screens
    int allowedSmokeScreens = player->shipsSunk; // Ships sunk by the player
    if (player->smokeScreensUsed >= allowedSmokeScreens) {
        printf("You have used all your smoke screens allowed. You lose your turn.\n");
        return;
    }

    // 2. Prompt for top-left coordinate
    char input[10];
    int row, col;
    printf("%s, enter the top-left coordinate for the smoke screen (e.g., B3): ", player->name);
    scanf("%s", input);

    // 3. Parse and validate coordinates
    if (!parseCoordinates(input, &row, &col)) {
        printf("Invalid coordinates. You lose your turn.\n");
        return;
    }

    // Ensure 2x2 area is within grid boundaries
    if (row < 0 || row >= GRID_SIZE - 1 || col < 0 || col >= GRID_SIZE - 1) {
        printf("Smoke screen area is out of bounds. You lose your turn.\n");
        return;
    }

    // 4. Update the smoke screen grid
    for (int i = row; i <= row + 1; i++) {
        for (int j = col; j <= col + 1; j++) {
            player->smokeScreenGrid[i][j] = 1; // Mark cell as smoke-screened
        }
    }

    // Increment smoke screens used
    player->smokeScreensUsed++;

    // 5. Clear the screen to preserve secrecy
    clearScreen();

    // 6. Inform the player
    printf("Smoke screen deployed successfully.\n");

    // Optionally, inform them of how many smoke screens they have left
    int smokeScreensLeft = allowedSmokeScreens - player->smokeScreensUsed;
    printf("You have %d smoke screen(s) left.\n", smokeScreensLeft);
}



// Dummy implementation of artillery
void artillery(Player *attacker, Player *defender) {
    printf("Artillery move not implemented yet.\n");
}

// Dummy implementation of torpedo
void torpedo(Player *attacker, Player *defender) {
    printf("Torpedo move not implemented yet.\n");
}


