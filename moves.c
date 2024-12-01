#include "moves.h"
#include "helper.h"
#include "battleship.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include<stdbool.h>

//Requires:
// - Valid, non-null pointers "attacker" and "defender" of type "Player" structure
// - "trackingDifficulty" must be a valid integer 
//Effects:
// - Prompts player for target coordinates and validates them
// - Checks if the cell with input coordinates has already been targeted
// - If fire target is valid : the function updates the "defender" grid to HIT or MISS
// -                           the function updates the "attacker" grid also accordingly
// - Returns 1 if game is over, 0 otherwise
int fire(Player *attacker, Player *defender, int trackingDifficulty) {
    char input[10];
    int row, col;

    // 1. Prompt for target coordinates
   // Prompt for target coordinates
    printf("%s, enter the coordinates to fire at (e.g., B3): ", attacker->name);
    scanf("%s", input);
    // Clear input buffer
    int c;
    while ((c = getchar()) != '\n' && c != EOF);

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

    // 5. Determine hit or miss
    if (cell == SHIP) {
        // Handle hit
        printf("Hit!\n");
        defender->grid[row][col] = HIT;
        attacker->trackingGrid[row][col] = HIT;

        // 6. Update ship status
        Ship *hitShip = findShipAtPosition(defender, row, col);
        if (hitShip != NULL) {
            hitShip->hits++;
            if (hitShip->hits == hitShip->size && !hitShip->sunk) {
                hitShip->sunk = true;
                defender->shipsRemaining--;
                printf("You have sunk the %s!\n", hitShip->name);

                // Increment ships sunk by attacker
                attacker->shipsSunk++;

                // Update Smoke Screens Available
                attacker->smokeScreensAvailable = attacker->shipsSunk - attacker->smokeScreensUsed;
                
                // Unlock advanced moves if applicable
                unlockAdvancedMoves(attacker, defender);
            }
        } else {
            // This should not happen; handle unexpected cases
            printf("Error: Could not find ship at the hit position.\n");
        }
    }
    else if (cell == WATER) {
        // Handle miss
        printf("Miss!\n");
        if (trackingDifficulty == 1) {
            defender->grid[row][col] = MISS; // Mark miss only in Easy mode
            attacker->trackingGrid[row][col] = MISS;
        }
    }
    else {
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



//Requires:
// - Valid, non-null pointers "attacker" and "defender" of type "Player" structure
//Effects:
// - Checks for number of remaining allowed radar sweep moves
// - Prompts player for valid coordinates
// - Validates coordinates
// - Informs attacker, by printing messages, of the presence of ships in area of coordinates
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
    // 4. Check for smoke screens
    bool smokePresent = false;
    for (int i = row; i <= row + 1 && !smokePresent; i++) {
        for (int j = col; j <= col + 1 && !smokePresent; j++) {
            if (defender->smokeScreenGrid[i][j] == SMOKE) {
                smokePresent = true;
            }
        }
    }

    // 5. Scan the 2x2 area
    bool shipsFound = false;

    if (!smokePresent) {
        for (int i = row; i <= row + 1 && !shipsFound; i++) {
            for (int j = col; j <= col + 1 && !shipsFound; j++) {
                if (defender->grid[i][j] == SHIP) {
                    shipsFound = true;
                }
            }
        }
    }

    // 6. Inform the attacker
    if (smokePresent) {
        printf("No enemy ships found.\n"); // Radar sweep obstructed by smoke screen
    } else if (shipsFound) {
        printf("Enemy ships found.\n");
    } else {
        printf("No enemy ships found.\n");
    }

    // 7. Update radar sweeps used
    attacker->radarSweepsUsed++;
    // 8. Set the flag to indicate that the bot was scanned
    defender->radarUsedAgainstBotLastTurn = true;
}


//Requires: Valid, non-null pointer "player" of type "Player" structure
//Effets:
// - Prompts the player for target coordinates
// - Validates coordinates
// - If the limit of smoke screen is exceeded, the player loses their turn
// - If the area chosen is already smoke covered, the player loses their turn
// - If coordinated are valid with no other violations: the function marks the area with smoke, where ships become undetectable by radar sweeps.

void smokeScreen(Player *player) {
    char input[10];
    int row, col;
   
   
    // Check if deploying the smoke screen exceeds the allowed limit
   if(player->smokeScreensAvailable <= 0) {
    printf("Smoke Screen limit exceeded. You lose your turn.\n");
    return;
}
     // Prompt for top-left coordinate
    printf("%s, enter the top-left coordinate for the Smoke Screen (e.g., B3): ", player->name);
    scanf("%s", input);
    // Clear input buffer
    int c;
    while ((c = getchar()) != '\n' && c != EOF);

    // Parse and validate coordinates
    if (!parseCoordinates(input, &row, &col)) {
        printf("Invalid coordinates for Smoke Screen. You lose your turn.\n");
        return;
    }

    // Check if the area is already covered by another smoke screen
    bool overlap = false;
    for(int i = row; i < row + SMOKE_SCREEN_SIZE && !overlap; i++) {
        for(int j = col; j < col + SMOKE_SCREEN_SIZE && !overlap; j++) {
            if(player->smokeScreenGrid[i][j] == SMOKE) {
                overlap = true;
            }
        }
    }
    if(overlap) {
        printf("Smoke Screen area already covered. You lose your turn.\n");
        return;
    }

    // Deploy the Smoke Screen
    for(int i = row; i < row + SMOKE_SCREEN_SIZE; i++) {
        for(int j = col; j < col + SMOKE_SCREEN_SIZE; j++) {
            player->smokeScreenGrid[i][j] = SMOKE; // Mark as smoke-screened
        }
    }
    player->smokeScreensUsed++;
    printf("Smoke Screen deployed at cells: ");
    printf("%c%d, %c%d, %c%d, %c%d\n",
           'A' + col, row + 1,
           'A' + (col + 1), row + 1,
           'A' + col, row + 2,
           'A' + (col + 1), row + 2);

    // Clear the screen to preserve secrecy
    clearScreen();
}

//Requires:
// - Two valid, non-null pointers "attacker" and "defender" of type "Player" structure
// - A valid trackingDifficulty integer (type int)
//Effects:
// - Checks if artillery move is available for this turn
// - Prompts player for valid coordinates
// - Validates coordinates
// - Targets a 2x2 area corresponding to player coordinates
// - Updates grids to HIT or MISS
void artillery(Player *attacker, Player *defender, int trackingDifficulty) {
    char input[10];
    int row, col;

    // Check if Artillery is available
    if (!attacker->artilleryAvailable) {
        printf("Artillery strike is not available. You lose your turn.\n");
        return;
    }

    // Prompt for top-left coordinate
    printf("%s, enter the top-left coordinate for the Artillery strike (e.g., B3): ", attacker->name);
    scanf("%s", input);
    // Clear input buffer
    int c;
    while ((c = getchar()) != '\n' && c != EOF);

    // Parse and validate coordinates
    if (!parseCoordinates(input, &row, &col)) {
        printf("Invalid coordinates for Artillery strike. You lose your turn.\n");
        return;
    }

    // Ensure 2x2 area is within grid boundaries
    if (row < 0 || row >= GRID_SIZE - 1 || col < 0 || col >= GRID_SIZE - 1) {
        printf("Artillery strike area is out of bounds. You lose your turn.\n");
        return;
    }

    // Perform Artillery strike
    bool hit = false;
    for (int i = row; i <= row + 1; i++) {
        for (int j = col; j <= col + 1; j++) {
            if (defender->grid[i][j] == SHIP) {
                printf("Hit at %c%d!\n", 'A' + j, i + 1);
                defender->grid[i][j] = HIT;
                attacker->trackingGrid[i][j] = HIT;

                // Update ship status
                Ship *hitShip = findShipAtPosition(defender, i, j);
                if (hitShip != NULL) {
                    hitShip->hits++;
                    if (hitShip->hits == hitShip->size && !hitShip->sunk) {
                        hitShip->sunk = true;
                        defender->shipsRemaining--;
                        printf("You have sunk the %s!\n", hitShip->name);

                        // Increment ships sunk by attacker
                        attacker->shipsSunk++;

                        // Update Smoke Screens Available
                        attacker->smokeScreensAvailable = attacker->shipsSunk - attacker->smokeScreensUsed;

                        // Debugging statements
                        printf("[DEBUG] %s has sunk a ship. Total ships sunk: %d\n",
                               attacker->name, attacker->shipsSunk);
                        printf("[DEBUG] Smoke Screens Available: %d\n",
                               attacker->smokeScreensAvailable);

                        // Unlock advanced moves if applicable
                        unlockAdvancedMoves(attacker,defender);
                    }
                }
                hit = true;
            }
            else if (defender->grid[i][j] == WATER) {
                printf("Miss at %c%d.\n", 'A' + j, i + 1);
                if (trackingDifficulty == 1) {
                    defender->grid[i][j] = MISS;
                    attacker->trackingGrid[i][j] = MISS;
                }
            }
        }
    }

    // Disable Artillery after use
    attacker->artilleryAvailable = 0;

    // Check for game over
    if (defender->shipsRemaining == 0) {
        printf("All enemy ships have been sunk!\n");
        return;
    }
}

//Requires:
// - Two valid, non-null pointers "attacker" and "defender" of type "Player" structure
// - A valid trackingDifficulty integer (type int)
//Effects:
// - Checks if torpedo move is available in this turn
// - Prompts player for a row or a column to target
// - Hits any ship in provied row or column
void torpedo(Player *attacker, Player *defender, int trackingDifficulty) {
    char input[10];
    int row, col;
    char axis;

    // Check if Torpedo is available
    if (!attacker->torpedoAvailable) {
        printf("Torpedo strike is not available. You lose your turn.\n");
        return;
    }

    // Prompt for attack axis (Row or Column)
    printf("%s, enter the axis for the Torpedo strike (R for Row, C for Column): ", attacker->name);
    scanf(" %c", &axis);
    // Clear input buffer
    int c;
    while ((c = getchar()) != '\n' && c != EOF);

    axis = toupper(axis);
    if (axis != 'R' && axis != 'C') {
        printf("Invalid axis selection. You lose your turn.\n");
        return;
    }

    // Prompt for the row or column number
    if (axis == 'R') {
        printf("Enter the row number to target (1-%d): ", GRID_SIZE);
    } else {
        printf("Enter the column letter to target (A-%c): ", 'A' + GRID_SIZE - 1);
    }

    scanf("%s", input);
    // Clear input buffer
    while ((c = getchar()) != '\n' && c != EOF);

    // Parse the input
    if (axis == 'R') {
        row = atoi(input) - 1;
        if (row < 0 || row >= GRID_SIZE) {
            printf("Invalid row number. You lose your turn.\n");
            return;
        }

        printf("%s performs a Torpedo strike on Row %d.\n", attacker->name, row + 1);

        for (int j = 0; j < GRID_SIZE; j++) {
            char cell = defender->grid[row][j];
            if (cell == SHIP) {
                printf("Hit at %c%d!\n", 'A' + j, row + 1);
                defender->grid[row][j] = HIT;
                attacker->trackingGrid[row][j] = HIT;

                // Update ship status
                Ship *hitShip = findShipAtPosition(defender, row, j);
                if (hitShip != NULL) {
                    hitShip->hits++;
                    if (hitShip->hits == hitShip->size && !hitShip->sunk) {
                        hitShip->sunk = true;
                        defender->shipsRemaining--;
                        printf("You have sunk the %s!\n", hitShip->name);

                        // Increment ships sunk by attacker
                        attacker->shipsSunk++;

                        // Update Smoke Screens Available
                        attacker->smokeScreensAvailable = attacker->shipsSunk - attacker->smokeScreensUsed;

                        // Unlock advanced moves if applicable
                        unlockAdvancedMoves(attacker, defender);
                    }
                }
            }
            else if (cell == WATER) {
                printf("Miss at %c%d.\n", 'A' + j, row + 1);
                if (trackingDifficulty == 1) {
                    defender->grid[row][j] = MISS;
                    attacker->trackingGrid[row][j] = MISS;
                }
            }
            // If already HIT or MISS, do nothing
        }
    }
    else { // axis == 'C'
        // Convert column letter to index
        if (strlen(input) != 1 || toupper(input[0]) < 'A' || toupper(input[0]) > ('A' + GRID_SIZE - 1)) {
            printf("Invalid column letter. You lose your turn.\n");
            return;
        }
        col = toupper(input[0]) - 'A';

        printf("%s performs a Torpedo strike on Column %c.\n", attacker->name, 'A' + col);

        for (int i = 0; i < GRID_SIZE; i++) {
            char cell = defender->grid[i][col];
            if (cell == SHIP) {
                printf("Hit at %c%d!\n", 'A' + col, i + 1);
                defender->grid[i][col] = HIT;
                attacker->trackingGrid[i][col] = HIT;

                // Update ship status
                Ship *hitShip = findShipAtPosition(defender, i, col);
                if (hitShip != NULL) {
                    hitShip->hits++;
                    if (hitShip->hits == hitShip->size && !hitShip->sunk) {
                        hitShip->sunk = true;
                        defender->shipsRemaining--;
                        printf("You have sunk the %s!\n", hitShip->name);

                        // Increment ships sunk by attacker
                        attacker->shipsSunk++;

                        // Update Smoke Screens Available
                        attacker->smokeScreensAvailable = attacker->shipsSunk - attacker->smokeScreensUsed;

                        // Unlock advanced moves if applicable
                        unlockAdvancedMoves(attacker, defender);
                    }
                }
            }
            else if (cell == WATER) {
                printf("Miss at %c%d.\n", 'A' + col, i + 1);
                if (trackingDifficulty == 1) {
                    defender->grid[i][col] = MISS;
                    attacker->trackingGrid[i][col] = MISS;
                }
            }
            // If already HIT or MISS, do nothing
        }
    }

    // Disable Torpedo after use
    attacker->torpedoAvailable = 0;

    // Check for game over
    if (defender->shipsRemaining == 0) {
        printf("All enemy ships have been sunk! %s wins!\n", attacker->name);
    }
}

//Requires:
// - Two valid, non-null pointers "bot" and "defender" of type "Player" structure
// - A valid 2D array of type int with size 10x10
//Effects:
// - Creates a heat map by calculating the potential horizontal and vertical placements of all opponents ships and updates heat map accordingly
void calculateHeatmap(Player *bot, Player *defender, int heatmap[GRID_SIZE][GRID_SIZE]) {
    // Initialize heatmap to zero
    for(int i = 0; i < GRID_SIZE; i++) {
        for(int j = 0; j < GRID_SIZE; j++) {
            heatmap[i][j] = 0;
        }
    }

    // Iterate through each ship in the defender's fleet
    for(int s = 0; s < TOTAL_SHIPS; s++) {
        Ship currentShip = defender->fleet[s];
        if(currentShip.sunk) continue; // Skip sunk ships

        // Iterate through all possible horizontal placements
        for(int i = 0; i < GRID_SIZE; i++) {
            for(int j = 0; j <= GRID_SIZE - currentShip.size; j++) {
                bool valid = true;

                // Check if any cell in the placement has been swept with no ships or already targeted
                for(int k = 0; k < currentShip.size; k++) {
                    // Map cell to sweep area
                    int sweptRow = i / SMOKE_SCREEN_SIZE;
                    int sweptCol = j / SMOKE_SCREEN_SIZE;

                    // Check bot's own sweep results
                    if(bot->sweepResults[sweptRow][sweptCol] == SWEEPED_NO_SHIP) {
                        valid = false;
                        break;
                    }

                    // Check if cell has been targeted already (HIT or MISS)
                    if(bot->trackingGrid[i][j + k] != WATER) {
                        valid = false;
                        break;
                    }
                }

                if(valid) {
                    for(int k = 0; k < currentShip.size; k++) {
                        heatmap[i][j + k]++;
                    }
                }
            }
        }

        // Iterate through all possible vertical placements
        for(int j = 0; j < GRID_SIZE; j++) {
            for(int i = 0; i <= GRID_SIZE - currentShip.size; i++) {
                bool valid = true;

                // Check if any cell in the placement has been swept with no ships or already targeted
                for(int k = 0; k < currentShip.size; k++) {
                    int sweptRow = (i + k) / SMOKE_SCREEN_SIZE;
                    int sweptCol = j / SMOKE_SCREEN_SIZE;

                    // Check bot's own sweep results
                    if(bot->sweepResults[sweptRow][sweptCol] == SWEEPED_NO_SHIP) {
                        valid = false;
                        break;
                    }

                    // Check if cell has been targeted already (HIT or MISS)
                    if(bot->trackingGrid[i + k][j] != WATER) {
                        valid = false;
                        break;
                    }
                }

                if(valid) {
                    for(int k = 0; k < currentShip.size; k++) {
                        heatmap[i + k][j]++;
                    }
                }
            }
        }
    }

    // Zero out the heatmap for cells within swept no-ship areas
    for(int i = 0; i <= GRID_SIZE - SMOKE_SCREEN_SIZE; i++) {
        for(int j = 0; j <= GRID_SIZE - SMOKE_SCREEN_SIZE; j++) {
            if(bot->sweepResults[i][j] == SWEEPED_NO_SHIP) {
                // Zero out the entire 2x2 area
                for(int x = i; x < i + SMOKE_SCREEN_SIZE; x++) {
                    for(int y = j; y < j + SMOKE_SCREEN_SIZE; y++) {
                        heatmap[x][y] = 0;
                    }
                }
            }
        }
    }
}

//Requires:
// - A valid 2D array of type int of size 10x10
// - Two non-null pointers "row" and "col" of type int
// - A valid, non-null pointer "bot" of "Player" structure
//Effects:
// - Updates "row" and "col" with the coordinates the highest probability untargeted cell in the heat map
// - If no valid target found, chooses random untargeted cell
void chooseBestTarget(int heatmap[GRID_SIZE][GRID_SIZE], int *row, int *col, Player *bot) {
    int maxHeat = -1;
    *row = -1;
    *col = -1;

    for(int i = 0; i < GRID_SIZE; i++) {
        for(int j = 0; j < GRID_SIZE; j++) {
            if(bot->trackingGrid[i][j] == WATER && heatmap[i][j] > maxHeat) {
                // Map cell to sweep area
                int sweptRow = i / SMOKE_SCREEN_SIZE;
                int sweptCol = j / SMOKE_SCREEN_SIZE;

                // Skip cells within swept no-ship areas
                if(bot->sweepResults[sweptRow][sweptCol] == SWEEPED_NO_SHIP) continue;

                maxHeat = heatmap[i][j];
                *row = i;
                *col = j;
            }
        }
    }

    // If no valid target found, choose a random untargeted cell
    if(*row == -1 || *col == -1) {
        for(int i = 0; i < GRID_SIZE; i++) {
            for(int j = 0; j < GRID_SIZE; j++) {
                if(bot->trackingGrid[i][j] == WATER && heatmap[i][j] > maxHeat) {
                    // Map cell to sweep area
                    int sweptRow = i / SMOKE_SCREEN_SIZE;
                    int sweptCol = j / SMOKE_SCREEN_SIZE;

                    // Skip cells within swept no-ship areas
                    if(bot->sweepResults[sweptRow][sweptCol] == SWEEPED_NO_SHIP) continue;

                    *row = i;
                    *col = j;
                    maxHeat = heatmap[i][j];
                }
            }
        }
    }

    // If still no target, select any untargeted cell randomly
    if(*row == -1 || *col == -1) {
        bool found = false;
        while(!found) {
            *row = rand() % GRID_SIZE;
            *col = rand() % GRID_SIZE;
            if(bot->trackingGrid[*row][*col] == WATER) {
                // Map cell to sweep area
                int sweptRow = *row / SMOKE_SCREEN_SIZE;
                int sweptCol = *col / SMOKE_SCREEN_SIZE;

                // Skip cells within swept no-ship areas
                if(bot->sweepResults[sweptRow][sweptCol] == SWEEPED_NO_SHIP) continue;

                found = true;
            }
        }
    }
}

//Requires:
// - A valid, non-null pointer "bot" of "Player" strcture
// - Two variables "row" and "col" of type int
//Effects:
// - Checks the four adjacent cells of the given coordinate "row"&"col"
// - Checks if each adjacent cell that is not targeted yet is a pending attack
// - Updates the cell to pending attack if not the case
void addAdjacentCells(Player *bot, int row, int col) {
    int directions[4][2] = { {-1,0}, {1,0}, {0,-1}, {0,1} }; // Up, Down, Left, Right

    for(int i = 0; i < 4; i++) {
        int newRow = row + directions[i][0];
        int newCol = col + directions[i][1];

        // Check boundaries
        if(newRow >= 0 && newRow < GRID_SIZE && newCol >=0 && newCol < GRID_SIZE) {
            // Check if the cell has not been targeted yet
            if(bot->trackingGrid[newRow][newCol] == WATER) {
                // Check if it's already in pending attacks
                bool alreadyPending = false;
                for(int p = 0; p < bot->pendingAttackCount; p++) {
                    if(bot->pendingAttacks[p][0] == newRow && bot->pendingAttacks[p][1] == newCol) {
                        alreadyPending = true;
                        break;
                    }
                }

                if(!alreadyPending && bot->pendingAttackCount < MAX_PENDING_ATTACKS) {
                    bot->pendingAttacks[bot->pendingAttackCount][0] = newRow;
                    bot->pendingAttacks[bot->pendingAttackCount][1] = newCol;
                    bot->pendingAttackCount++;
                   
                }
            }
        }
    }
}

//Requires:- Two valid, non-null pointers "bot" and "defender" of "Player" struct
//Effects:
// - Determines direction in which defender's ship is placed
// - Once direction is found, it locks direction until ship is sunk
void determineDirection(Player *bot, Player *defender) {
    // Check all adjacent cells for additional hits to determine direction
    int directions[4][2] = { {-1,0}, {1,0}, {0,-1}, {0,1} }; // Up, Down, Left, Right

    for(int i = 0; i < 4; i++) {
        int adjRow = bot->lastHitRow + directions[i][0];
        int adjCol = bot->lastHitCol + directions[i][1];

        if(adjRow >=0 && adjRow < GRID_SIZE && adjCol >=0 && adjCol < GRID_SIZE) {
            if(bot->trackingGrid[adjRow][adjCol] == HIT) { // Found another hit
                // Determine direction based on the relative position
                bot->directionRow = adjRow - bot->lastHitRow;
                bot->directionCol = adjCol - bot->lastHitCol;

                // Transition to LOCK_DIRECTION Phase
                bot->currentPhase = lockDirection;
             

                // Update last hit position to the adjacent hit
                bot->lastHitRow = adjRow;
                bot->lastHitCol = adjCol;

                return;
            }
        }
    }

    // If no adjacent hit found, remain in TARGET Phase
    bot->currentPhase = Target;
    
}//Requires:- Two valid non null pointers to "bot" and "defender" of Player struct
//          - Two variables "row" and "column" of type int 
//Effects: Once a cell is missed after firing, it decreases its value and the value of its adjacent cells on the heatmap 
void adjustHeatmapOnMiss(Player *bot, int row, int col, Player *defender) {
    int directions[4][2] = { {-1,0}, {1,0}, {0,-1}, {0,1} }; // Up, Down, Left, Right

    for(int i = 0; i < 4; i++) {
        int adjRow = row + directions[i][0];
        int adjCol = col + directions[i][1];

        // Check boundaries
        if(adjRow >=0 && adjRow < GRID_SIZE && adjCol >=0 && adjCol < GRID_SIZE) {
            // Decrease probability
            bot->lastHeatmap[adjRow][adjCol] -= 2;
            if(bot->lastHeatmap[adjRow][adjCol] < 0) {
                bot->lastHeatmap[adjRow][adjCol] = 0;
            }

        }
    }
}
//Requires: two valid non null pointers to "bot and "defender" of Player struct 
//Effects:- Transitions between phases (e.g., Hunt -> Target, Target -> lockDirection) as necessary.
// - Adjusts the heatmap and pending attack queue based on the outcome of the attack.
// - Checks if any of the defender's ships are sunk and transitions phases accordingly.
// - Ends the game if all the defender's ships are sunk, returning 1.
// - Returns 0 if the game is still ongoing or if the bot is unable to act within `maxIterations` attempts.
int botFire(Player *bot, Player *defender) {
    int row, col;
    bool actionTaken = false;
    int maxIterations = 10; // Prevent infinite loops
    int iterations = 0;

    while (!actionTaken && iterations < maxIterations) {
        iterations++;
        

        // **Priority: If there are pending attacks, switch to TARGET phase**
        if (bot->pendingAttackCount > 0 && bot->currentPhase != Target && bot->currentPhase != lockDirection) {
            bot->currentPhase = Target;
           
        }

        switch (bot->currentPhase) {
            case Hunt:
                // Perform Hunt Phase: Use heatmap and pattern-based targeting to choose best target
                {
                    int heatmap[GRID_SIZE][GRID_SIZE];
                    calculateHeatmap(bot, defender, heatmap);
                    chooseBestTarget(heatmap, &row, &col, bot);
                  
                }
                break;

            case Target:
                // Perform Target Phase: Attack pending attacks
                if (bot->pendingAttackCount > 0) {
                    // Get the next pending attack coordinates
                    row = bot->pendingAttacks[0][0];
                    col = bot->pendingAttacks[0][1];
                   
                    // Remove the processed attack from pending attacks
                    for (int i = 1; i < bot->pendingAttackCount; i++) {
                        bot->pendingAttacks[i - 1][0] = bot->pendingAttacks[i][0];
                        bot->pendingAttacks[i - 1][1] = bot->pendingAttacks[i][1];
                    }
                    bot->pendingAttackCount--;
                } else {
                    // No pending attacks; revert to Hunt Phase
                 
                    bot->currentPhase = Hunt;
                    continue; // Restart loop
                }
                break;

            case lockDirection:
                // Perform Lock Direction Phase: Continue firing in determined direction
                {
                    // Ensure that directionRow and directionCol have been set
                    if (bot->directionRow == 0 && bot->directionCol == 0) {
                        // Undefined direction; revert to Target Phase
                        bot->currentPhase = Target;
                        continue; // Restart loop
                    }

                    // Calculate next target based on locked direction
                    row = bot->lastHitRow + bot->directionRow;
                    col = bot->lastHitCol + bot->directionCol;
                        

                    // Check boundaries
                    if (row < 0 || row >= GRID_SIZE || col < 0 || col >= GRID_SIZE) {
                        // Reverse direction if out of bounds
                        bot->directionRow = -bot->directionRow;
                        bot->directionCol = -bot->directionCol;
                        row = bot->lastHitRow + bot->directionRow;
                        col = bot->lastHitCol + bot->directionCol;
                    

                        // Check again after reversing
                        if (row < 0 || row >= GRID_SIZE || col < 0 || col >= GRID_SIZE) {
                            // Cannot fire in any direction; revert to Hunt Phase
                            bot->currentPhase = Hunt;
                            bot->directionRow = 0;
                            bot->directionCol = 0;
                            continue; // Restart loop
                        }
                    }

                    // Check if the cell has already been targeted
                    if (bot->trackingGrid[row][col] != WATER) {
                        // Skip and try reversing direction
                        bot->directionRow = -bot->directionRow;
                        bot->directionCol = -bot->directionCol;
                        row = bot->lastHitRow + bot->directionRow;
                        col = bot->lastHitCol + bot->directionCol;
                        
                        // Check boundaries again
                        if (row < 0 || row >= GRID_SIZE || col < 0 || col >= GRID_SIZE || bot->trackingGrid[row][col] != WATER) {
                            // Cannot fire in any direction; revert to Hunt Phase
                            bot->currentPhase = Hunt;
                            bot->directionRow = 0;
                            bot->directionCol = 0;
                            continue; // Restart loop
                        }
                    }
                }
                break;

            default:
                // Undefined phase; revert to Hunt Phase
                bot->currentPhase = Hunt;
                continue; // Restart loop
        }

        // Check if the selected cell has already been targeted
        if (bot->trackingGrid[row][col] != WATER) {
            if (bot->currentPhase == Target || bot->currentPhase == lockDirection) {
                if (bot->currentPhase == Target && bot->pendingAttackCount > 0) {
                    continue; // Restart loop
                } else {
                    bot->currentPhase = Hunt;
                    continue; // Restart loop
                }
            }
            // If not in Target or Lock Direction Phase, just skip
            break;
        }

        

        printf("Bot fires at %c%d.\n", 'A' + col, row + 1);

        // Perform the attack and determine outcome
        if (defender->grid[row][col] == SHIP) {
            printf("Bot hit your ship at %c%d!\n", 'A' + col, row + 1);
            defender->grid[row][col] = HIT;
            bot->trackingGrid[row][col] = HIT;

            // Update last hit coordinates
            bot->lastHitRow = row;
            bot->lastHitCol = col;

            // Transition phases based on current phase
            if (bot->currentPhase == Hunt) {
                // Transition to Target Phase
                bot->currentPhase = Target;
                // Add adjacent cells to pending attacks
                addAdjacentCells(bot, row, col);
               
            }
            else if (bot->currentPhase == Target) {
                // Second hit; determine direction
                determineDirection(bot, defender);
            }
            else if (bot->currentPhase == lockDirection) {
                // Continue in Lock Direction Phase
                // No additional action needed here
              
            }

            // Check if the ship is sunk
            Ship *hitShip = findShipAtPosition(defender, row, col);
            if (hitShip != NULL) {
                hitShip->hits++;
               

                if (hitShip->hits == hitShip->size && !hitShip->sunk) {
                    hitShip->sunk = true;
                    defender->shipsRemaining--;
                    bot->shipsSunk++;
                    printf("Bot has sunk your %s!\n", hitShip->name);

                    // Unlock advanced moves if applicable
                    unlockAdvancedMoves(bot, defender);

                    // Reset phase and clear pending attacks
                    bot->currentPhase = Hunt;
                    bot->pendingAttackCount = 0;
                    bot->directionRow = 0;
                    bot->directionCol = 0;
                    
                }
            }
        }
        else if (defender->grid[row][col] == WATER) {
            printf("Bot missed at %c%d.\n", 'A' + col, row + 1);
            bot->trackingGrid[row][col] = MISS;

            // Adjust heatmap based on miss
            adjustHeatmapOnMiss(bot, row, col, defender);
            

            // If in Target Phase, switch back to Hunt if no pending attacks
            if (bot->currentPhase == Target && bot->pendingAttackCount == 0) {
                bot->currentPhase = Hunt;
            }
        }
        else {
            // Handle any unexpected cell states
            printf("Bot encountered an unexpected cell state at %c%d.\n", 'A' + col, row + 1);
        }

        // Check for game over
        if (defender->shipsRemaining == 0) {
            printf("All your ships have been sunk! Bot wins!\n");
            return 1; // Game over
        }

        actionTaken = true; // Action has been taken
    }

    // Prevent botFire from going into infinite loops
    if (iterations >= maxIterations) {
        bot->currentPhase = Hunt;
        return 0;
    }

    return 0; // Game continues
}
//Requires: Two valid non null pointers to "bot" and "defender" of player struct 
//Effects: Peforms a radar sweep on a 2x2 area based on previous heatmap calculations
// - If ships are detected in the area, adds corresponding cells to "bot->pendingAttacks" and increments "bot->pendingAttackCount".
// - If ships are detected, transitions "bot->currentPhase" to Target.
void botRadar(Player *bot, Player *defender) {
    if(bot->radarSweepsUsed >= MAX_RADAR_SWEEPS) {
        printf("Bot could not perform Radar sweep. No sweeps left.\n");
        return;
    }

    // Calculate heatmap to determine the best 2x2 area
    int heatmap[GRID_SIZE][GRID_SIZE];
    calculateHeatmap(bot, defender, heatmap);

    int bestRow = -1, bestCol = -1;
    int maxHeat = -1;

    // To handle multiple areas with the same maxHeat
    typedef struct {
        int row;
        int col;
    } Area;

    Area bestAreas[GRID_SIZE * GRID_SIZE];
    int bestAreaCount = 0;

    // Iterate through all possible 2x2 areas
    for(int i = 0; i <= GRID_SIZE - SMOKE_SCREEN_SIZE; i++) {
        for(int j = 0; j <= GRID_SIZE - SMOKE_SCREEN_SIZE; j++) {
            // Skip already swept areas
            if(bot->sweepResults[i][j] == SWEEPED_NO_SHIP || bot->sweepResults[i][j] == SWEEPED_WITH_SHIP) {
                continue;
            }

            // Calculate cumulative heat for the 2x2 area
            int currentHeat = 0;
            for(int x = i; x < i + SMOKE_SCREEN_SIZE; x++) {
                for(int y = j; y < j + SMOKE_SCREEN_SIZE; y++) {
                    currentHeat += heatmap[x][y];
                }
            }

            if(currentHeat > maxHeat) {
                maxHeat = currentHeat;
                bestAreaCount = 0;
                bestAreas[bestAreaCount].row = i;
                bestAreas[bestAreaCount].col = j;
                bestAreaCount++;
            }
            else if(currentHeat == maxHeat && maxHeat > 0) {
                bestAreas[bestAreaCount].row = i;
                bestAreas[bestAreaCount].col = j;
                bestAreaCount++;
            }
        }
    }

    if(bestAreaCount == 0) {
        printf("Bot could not find a suitable area for Radar sweep.\n");
        return;
    }

    // Randomly select one of the best areas to avoid repetition
    if(bestAreaCount > 1) {
        int selected = rand() % bestAreaCount;
        bestRow = bestAreas[selected].row;
        bestCol = bestAreas[selected].col;
    }
    else {
        bestRow = bestAreas[0].row;
        bestCol = bestAreas[0].col;
    }

    printf("Bot performs a Radar sweep at Rows %d-%d, Columns %c-%c.\n", 
           bestRow + 1, bestRow + SMOKE_SCREEN_SIZE, 
           'A' + bestCol, 'A' + bestCol + SMOKE_SCREEN_SIZE - 1);

    // Mark the area as swept
    bot->sweepResults[bestRow][bestCol] = SWEEPED_NO_SHIP; // Assume no ships initially

    // Check if any cell in the 2x2 area contains a ship
    bool shipsFound = false;
    for(int i = bestRow; i < bestRow + SMOKE_SCREEN_SIZE && !shipsFound; i++) {
        for(int j = bestCol; j < bestCol + SMOKE_SCREEN_SIZE && !shipsFound; j++) {
            if(defender->grid[i][j] == SHIP) {
                shipsFound = true;
                break;
            }
        }
    }

    if(shipsFound) {
        printf("Radar sweep detected enemy ships in the area. Preparing to attack the area.\n");
        bot->sweepResults[bestRow][bestCol] = SWEEPED_WITH_SHIP;

        // Add all cells in the 2x2 area to pending attacks
        for(int i = bestRow; i < bestRow + SMOKE_SCREEN_SIZE; i++) {
            for(int j = bestCol; j < bestCol + SMOKE_SCREEN_SIZE; j++) {
                // Avoid adding already hit or missed cells
                if(bot->trackingGrid[i][j] == WATER) {
                    // Check if the cell is already pending
                    bool alreadyPending = false;
                    for(int p = 0; p < bot->pendingAttackCount; p++) {
                        if(bot->pendingAttacks[p][0] == i && bot->pendingAttacks[p][1] == j) {
                            alreadyPending = true;
                            break;
                        }
                    }
                    if(!alreadyPending && bot->pendingAttackCount < MAX_PENDING_ATTACKS) {
                        bot->pendingAttacks[bot->pendingAttackCount][0] = i;
                        bot->pendingAttacks[bot->pendingAttackCount][1] = j;
                        bot->pendingAttackCount++;
                        printf("[DEBUG] Added %c%d to pending attacks.\n", 'A' + j, i + 1);
                    }
                }
            }
        }
        // **Set currentPhase to Target to prioritize pending attacks**
        bot->currentPhase = Target;
    } else {
        printf("Radar sweep found no enemy ships in the area.\n");
    }

    // Increment radar sweeps used
    bot->radarSweepsUsed++;
}
//Requires: A valid non null pointer to "bot" of Player struct 
//Effects:- Deploys Smoke Screens around unsunk ships within a perimeter of size 1.
// - Updates "bot->smokeScreenGrid" for cells where Smoke Screens are deployed.
// - If all available Smoke Screens are deployed, the function exits early.
// - If no Smoke Screens are available or deployable, the function prints a corresponding message.
void botSmokeScreen(Player *bot) {
    // Determine the number of allowed Smoke Screens based on ships sunk
    int allowedSmokeScreens = bot->shipsSunk;
    int availableSmokeScreens = allowedSmokeScreens - bot->smokeScreensUsed;

    if(availableSmokeScreens <= 0) {
        printf("Bot has no Smoke Screens left to deploy.\n");
        return;
    }

    // Define the perimeter around a ship to deploy Smoke Screens
    int perimeterOffset = 1; // Number of cells around the ship to cover

    // Iterate through each ship in the bot's fleet
    for(int s = 0; s < TOTAL_SHIPS; s++) {
        if(bot->fleet[s].sunk) continue; // Skip sunk ships

        // Get the ship's coordinates
        for(int p = 0; p < bot->fleet[s].size; p++) {
            int shipRow = bot->fleet[s].positions[p][0];
            int shipCol = bot->fleet[s].positions[p][1];

            // Define the area around the ship's current position
            for(int i = shipRow - perimeterOffset; i <= shipRow + perimeterOffset; i++) {
                for(int j = shipCol - perimeterOffset; j <= shipCol + perimeterOffset; j++) {
                    // Check if the cell is within grid bounds
                    if(i < 0 || i >= GRID_SIZE || j < 0 || j >= GRID_SIZE)
                        continue;

                    // Skip the ship's own cells
                    if(bot->grid[i][j] == SHIP)
                        continue;

                    // Check if the cell is already covered by a Smoke Screen
                    if(bot->smokeScreenGrid[i][j] == SMOKE)
                        continue;

                    // Deploy Smoke Screen if available
                    if(availableSmokeScreens > 0) {
                        bot->smokeScreenGrid[i][j] = SMOKE;
                        bot->smokeScreensUsed++;
                        availableSmokeScreens--;

                        printf("Bot deployed a Smoke Screen at %c%d.\n", 'A' + j, i + 1);

                        // If no more Smoke Screens are available, exit
                        if(availableSmokeScreens <= 0)
                            return;
                    }
                }
            }
        }
    }

    // Inform if all available Smoke Screens have been deployed
    if(availableSmokeScreens > 0) {
        printf("Bot has unused Smoke Screens remaining.\n");
    }
}
//Requires:- Two valid non null pointers to "bot" and "defender" of type Player Strcut 
//- "trackingDifficulty" must be an integer indicating the difficulty level for tracking missed attacks.
//Effects: - Determines the optimal 2x2 area for an Artillery strike based on the heatmap 
// - If a ship is hit, marks it as HIT and checks whether it is sunk.
// - If a ship is sunk, updates its status and decrements "defender->shipsRemaining".
void botArtillery(Player *bot, Player *defender, int trackingDifficulty) {
    // Check if Artillery is available
    if (!bot->artilleryAvailable) {
        printf("Bot's Artillery strike is not available.\n");
        return;
    }

    // Calculate heatmap with updated sweep results
    int heatmap[GRID_SIZE][GRID_SIZE];
    calculateHeatmap(bot, defender, heatmap);

    int bestRow = -1, bestCol = -1;
    int maxHeat = -1;

    // Iterate through all possible 2x2 areas
    for(int i = 0; i <= GRID_SIZE - SMOKE_SCREEN_SIZE; i++) {
        for(int j = 0; j <= GRID_SIZE - SMOKE_SCREEN_SIZE; j++) {
            // Skip areas that have been swept with no ships
            bool areaCleared = true;
            for(int x = i; x < i + SMOKE_SCREEN_SIZE; x++) {
                for(int y = j; y < j + SMOKE_SCREEN_SIZE; y++) {
                    if(bot->sweepResults[x][y] != SWEEPED_NO_SHIP) {
                        areaCleared = false;
                        break;
                    }
                }
                if(!areaCleared) break;
            }
            if(areaCleared) continue; // Skip cleared areas

            // Check for overlapping with smoke screens
            bool smokePresent = false;
            for(int x = i; x < i + SMOKE_SCREEN_SIZE && !smokePresent; x++) {
                for(int y = j; y < j + SMOKE_SCREEN_SIZE && !smokePresent; y++) {
                    if(defender->smokeScreenGrid[x][y] == SMOKE) {
                        smokePresent = true;
                    }
                }
            }
            if(smokePresent) continue; // Avoid deploying artillery on smoke-covered areas

            // Calculate cumulative heat for the 2x2 area
            int currentHeat = 0;
            for(int x = i; x < i + SMOKE_SCREEN_SIZE; x++) {
                for(int y = j; y < j + SMOKE_SCREEN_SIZE; y++) {
                    currentHeat += heatmap[x][y];
                }
            }

            // Select the area with the highest cumulative heat
            if(currentHeat > maxHeat) {
                maxHeat = currentHeat;
                bestRow = i;
                bestCol = j;
            }
        }
    }

    if(bestRow == -1 || bestCol == -1) {
        printf("Bot could not find a suitable area for Artillery strike.\n");
        return;
    }

    printf("Bot performs an Artillery strike at Rows %d-%d, Columns %c-%c.\n", 
           bestRow + 1, bestRow + SMOKE_SCREEN_SIZE, 
           'A' + bestCol, 'A' + bestCol + SMOKE_SCREEN_SIZE - 1);

    // Perform Artillery strike on the selected 2x2 area
    bool hitOccurred = false;
    for(int i = bestRow; i < bestRow + SMOKE_SCREEN_SIZE; i++) {
        for(int j = bestCol; j < bestCol + SMOKE_SCREEN_SIZE; j++) {
            if(defender->grid[i][j] == SHIP) {
                printf("Hit at %c%d!\n", 'A' + j, i + 1);
                defender->grid[i][j] = HIT;
                bot->trackingGrid[i][j] = HIT;
                hitOccurred = true;

                // Update ship status
                Ship *hitShip = findShipAtPosition(defender, i, j);
                if(hitShip != NULL && !hitShip->sunk) {
                    hitShip->hits++;
                    if(hitShip->hits == hitShip->size) {
                        hitShip->sunk = true;
                        defender->shipsRemaining--;
                        printf("Bot has sunk the %s!\n", hitShip->name);

                        // Increment ships sunk by bot
                        bot->shipsSunk++;

                        // Update Smoke Screens Available
                        bot->smokeScreensAvailable = bot->shipsSunk - bot->smokeScreensUsed;

                        // Unlock advanced moves if applicable
                        unlockAdvancedMoves(bot, defender);
                    }
                }
            }
            else if(defender->grid[i][j] == WATER) {
                printf("Miss at %c%d.\n", 'A' + j, i + 1);
                if (trackingDifficulty == 1) {
                    defender->grid[i][j] = MISS;
                    bot->trackingGrid[i][j] = MISS;
                }
            }
            // No action needed for already HIT or MISS cells
        }
    }

    // Adjust heatmap based on miss
    if(!hitOccurred) {
        for(int i = bestRow; i < bestRow + SMOKE_SCREEN_SIZE; i++) {
            for(int j = bestCol; j < bestCol + SMOKE_SCREEN_SIZE; j++) {
                if(defender->grid[i][j] == WATER || defender->grid[i][j] == MISS) {
                    adjustHeatmapOnMiss(bot, i, j, defender);
                }
            }
        }
    }

    // Disable Artillery after use
    bot->artilleryAvailable = false;

    // Check for game over
    if(defender->shipsRemaining == 0) {
        printf("All enemy ships have been sunk! Bot wins!\n");
        return;
    }
}
//Requires: - Two valid non null pointers of type Player Struct 
// - "trackingDifficulty" must be an integer indicating the difficulty level for tracking missed attacks.
//Effects: - Targets either a row or column based on heatmap calculations.
// - Updates the defender’s grid (HIT, MISS) and bot’s tracking grid accordingly.

void botTorpedo(Player *bot, Player *defender, int trackingDifficulty) {
    // Check if Torpedo is available
    if (!bot->torpedoAvailable) {
        printf("Bot's Torpedo strike is not available.\n");
        return;
    }

    // Calculate heatmap with dynamic adjustments
    int heatmap[GRID_SIZE][GRID_SIZE];
    calculateHeatmap(bot, defender, heatmap);

    // Adjust heatmap based on lastHeatmap to account for misses
    for(int i = 0; i < GRID_SIZE; i++) {
        for(int j = 0; j < GRID_SIZE; j++) {
            heatmap[i][j] += bot->lastHeatmap[i][j];
            if(heatmap[i][j] < 0) {
                heatmap[i][j] = 0; // Prevent negative heat values
            }
        }
    }

    int bestRow = -1, bestCol = -1;
    int maxRowHeat = 0, maxColHeat = 0;

    // Calculate total heat for each row
    for(int i = 0; i < GRID_SIZE; i++) {
        int rowHeat = 0;
        bool rowAlreadyTargeted = true;
        for(int j = 0; j < GRID_SIZE; j++) {
            if(defender->grid[i][j] != HIT && defender->grid[i][j] != MISS && defender->grid[i][j] != SMOKE) {
                rowHeat += heatmap[i][j];
                rowAlreadyTargeted = false;
            }
        }
        if(!rowAlreadyTargeted && rowHeat > maxRowHeat) {
            maxRowHeat = rowHeat;
            bestRow = i;
        }
    }

    // Calculate total heat for each column
    for(int j = 0; j < GRID_SIZE; j++) {
        int colHeat = 0;
        bool colAlreadyTargeted = true;
        for(int i = 0; i < GRID_SIZE; i++) {
            if(defender->grid[i][j] != HIT && defender->grid[i][j] != MISS && defender->grid[i][j] != SMOKE) {
                colHeat += heatmap[i][j];
                colAlreadyTargeted = false;
            }
        }
        if(!colAlreadyTargeted && colHeat > maxColHeat) {
            maxColHeat = colHeat;
            bestCol = j;
        }
    }

    // Decide whether to attack row or column based on higher heat
    bool attackRow = false;
    if(maxRowHeat > maxColHeat) {
        attackRow = true;
    }

    if(attackRow && bestRow != -1) {
        printf("Bot performs a Torpedo strike on Row %d.\n", bestRow + 1);
        for(int j = 0; j < GRID_SIZE; j++) {
            char cell = defender->grid[bestRow][j];
            if(cell == SHIP) {
                printf("Hit at %c%d!\n", 'A' + j, bestRow + 1);
                defender->grid[bestRow][j] = HIT;
                bot->trackingGrid[bestRow][j] = HIT;

                // Update ship status
                Ship *hitShip = findShipAtPosition(defender, bestRow, j);
                if(hitShip != NULL && !hitShip->sunk) {
                    hitShip->hits++;
                    if(hitShip->hits == hitShip->size) {
                        hitShip->sunk = true;
                        defender->shipsRemaining--;
                        printf("Bot has sunk the %s!\n", hitShip->name);

                        // Increment ships sunk by bot
                        bot->shipsSunk++;

                        // Update Smoke Screens Available
                        bot->smokeScreensAvailable = bot->shipsSunk - bot->smokeScreensUsed;

                        // Unlock advanced moves if applicable
                        unlockAdvancedMoves(bot, defender);
                    }
                }
            }
            else if(cell == WATER) {
                printf("Miss at %c%d.\n", 'A' + j, bestRow + 1);
                if(trackingDifficulty == 1) {
                    defender->grid[bestRow][j] = MISS;
                    bot->trackingGrid[bestRow][j] = MISS;
                }
            }
            // No action needed for already HIT or MISS cells
        }
    }
    else if(bestCol != -1) {
        printf("Bot performs a Torpedo strike on Column %c.\n", 'A' + bestCol);
        for(int i = 0; i < GRID_SIZE; i++) {
            char cell = defender->grid[i][bestCol];
            if(cell == SHIP) {
                printf("Hit at %c%d!\n", 'A' + bestCol, i + 1);
                defender->grid[i][bestCol] = HIT;
                bot->trackingGrid[i][bestCol] = HIT;

                // Update ship status
                Ship *hitShip = findShipAtPosition(defender, i, bestCol);
                if(hitShip != NULL && !hitShip->sunk) {
                    hitShip->hits++;
                    if(hitShip->hits == hitShip->size) {
                        hitShip->sunk = true;
                        defender->shipsRemaining--;
                        printf("Bot has sunk the %s!\n", hitShip->name);

                        // Increment ships sunk by bot
                        bot->shipsSunk++;

                        // Update Smoke Screens Available
                        bot->smokeScreensAvailable = bot->shipsSunk - bot->smokeScreensUsed;

                        // Unlock advanced moves if applicable
                        unlockAdvancedMoves(bot, defender);
                    }
                }
            }
            else if(cell == WATER) {
                printf("Miss at %c%d.\n", 'A' + bestCol, i + 1);
                if(trackingDifficulty == 1) {
                    defender->grid[i][bestCol] = MISS;
                    bot->trackingGrid[i][bestCol] = MISS;
                }
            }
            // No action needed for already HIT or MISS cells
        }
    }

    // Adjust heatmap based on miss
    if(bestRow != -1 && maxRowHeat > maxColHeat) {
        for(int j = 0; j < GRID_SIZE; j++) {
            if(defender->grid[bestRow][j] == WATER || defender->grid[bestRow][j] == MISS) {
                adjustHeatmapOnMiss(bot, bestRow, j, defender);
            }
        }
    }
    else if(bestCol != -1 && maxColHeat >= maxRowHeat) {
        for(int i = 0; i < GRID_SIZE; i++) {
            if(defender->grid[i][bestCol] == WATER || defender->grid[i][bestCol] == MISS) {
                adjustHeatmapOnMiss(bot, i, bestCol, defender);
            }
        }
    }

    // Disable Torpedo after use
    bot->torpedoAvailable = false;

    // Check for game over
    if(defender->shipsRemaining == 0) {
        printf("All enemy ships have been sunk! Bot wins!\n");
    }
}
