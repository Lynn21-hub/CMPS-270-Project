#include "helper.h"
#include "battleship.h"
#include "moves.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>
void initializePlayer(Player *player, int botFlag) {
    // Initialize the player's grid
    initializeGrid(player->grid);

    // Reset counters and flags
    player->radarSweepsUsed = 0;
    player->smokeScreensUsed = 0;
    player->artilleryAvailable = 0;
    player->torpedoAvailable = 0;
    player->shipsRemaining = TOTAL_SHIPS;
    player->shipsSunk = 0;
    player->radarUsedAgainstBotLastTurn = false;

    // Initialize the smoke screen grid
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            player->smokeScreenGrid[i][j] = 0;
            player->trackingGrid[i][j] = WATER;
        }
    }

    // Initialize the isBot flag
    player->isBot = botFlag;

    // Set Smoke Screens Available based on ships sunk (initially 0)
     player->smokeScreensAvailable = 0;

     // Initialize sweptAreas to false
    for(int i = 0; i <= GRID_SIZE - SMOKE_SCREEN_SIZE; i++) {
        for(int j = 0; j <= GRID_SIZE - SMOKE_SCREEN_SIZE; j++) {
            player->sweptAreas[i][j] = false;
        }
    }

    // Initialize pending attacks
    player->pendingAttackCount = 0;

        for(int i = 0; i < TOTAL_SHIPS; i++) {
            strcpy(player->fleet[i].name, ""); // Placeholder, to be set in placeShipsBot
            player->fleet[i].size = 0;
            player->fleet[i].hits = 0;
            player->fleet[i].sunk = false;
            for(int j = 0; j < MAX_SHIP_SIZE; j++) {
                player->fleet[i].positions[j][0] = -1;
                player->fleet[i].positions[j][1] = -1;
            }
        }
    }
    // Function to initialize the bot with additional attributes
void initializeBot(Player *bot) {
    // First, initialize general player attributes
    initializePlayer(bot, 1); // 1 indicates bot

    // Initialize bot-specific attributes
    bot->currentPhase = Hunt;
    bot->lastHitRow = -1;
    bot->lastHitCol = -1;
    bot->directionRow = 0;
    bot->directionCol = 0;
    bot->moveCount = 0;
    bot->turnsSinceLastRadarSweep = 0;
    bot->artilleryAvailable = false;

    // Initialize lastHeatmap to 0
    for(int i = 0; i < GRID_SIZE; i++) {
        for(int j = 0; j < GRID_SIZE; j++) {
            bot->lastHeatmap[i][j] = 0;
        }
    }

}


// Function to initialize the grid with water
void initializeGrid(char grid[GRID_SIZE][GRID_SIZE]) {
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            grid[i][j] = WATER;
        }
    }
}

// Function to display the grid
void displayGrid(char grid[GRID_SIZE][GRID_SIZE], int showShips) {
    printf("   A B C D E F G H I J\n");
    for (int i = 0; i < GRID_SIZE; i++) {
        printf("%2d ", i + 1);
        for (int j = 0; j < GRID_SIZE; j++) {
            if (grid[i][j] == SHIP && !showShips) {
                printf("%c ", WATER); // Hide ships when not showing them
            } else {
                printf("%c ", grid[i][j]);
            }
        }
        printf("\n");
    }
}

// Function to ask for difficulty level
int getDifficultyLevel() {
    int difficulty;
    do {
        printf("Choose tracking difficulty level (1 for Easy, 2 for Hard): ");
        scanf("%d", &difficulty);
    } while (difficulty != 1 && difficulty != 2);
    return difficulty;
}

// Function to get player names
void getPlayerNames(Player *player1, Player *player2) {
    printf("Enter name for Player 1: ");
    fgets(player1->name, MAX_NAME_LENGTH, stdin);
    // Remove newline character from name
    player1->name[strcspn(player1->name, "\n")] = 0;

    // Set default name for bot
    strcpy(player2->name, "Bot");
}

// Function to randomly choose the first player
int chooseFirstPlayer() {
    srand((unsigned int) time(NULL));
    return rand() % 2 + 1;
}



// Function to check if a ship can be placed at the given position
int isPlacementValid(char grid[GRID_SIZE][GRID_SIZE], int row, int col, int length, char orientation) {
    if (row < 0 || row >= GRID_SIZE || col < 0 || col >= GRID_SIZE) //adding validation for row and col for bounds checking
        return 0;
    if (orientation == 'H') {
        if (col + length > GRID_SIZE) return 0;
        for (int i = 0; i < length; i++) {
            if (grid[row][col + i] != WATER) return 0;
        }
    } else if (orientation == 'V') {
        if (row + length > GRID_SIZE) return 0;
        for (int i = 0; i < length; i++) {
            if (grid[row + i][col] != WATER) return 0;
        }
    }
    return 1;
}

// Function to place ships on the grid
void placeShips(Player *player) {
    int shipSizes[] = {5, 4, 3, 2}; // Sizes of the ships
    char *shipNames[] = {"Carrier", "Battleship", "Destroyer", "Submarine"};
    int row, col;
    char orientation;

    for (int i = 0; i < TOTAL_SHIPS; i++) {
        int valid = 0;
        while (!valid) {
            printf("Enter the starting coordinates (e.g., A5) and orientation (H/V) for your %s: ", shipNames[i]);
            char colChar;
            scanf(" %c%d %c", &colChar, &row, &orientation);
            colChar = toupper(colChar);
            col = colChar - 'A'; // Convert letter to index
            row -= 1;            // Convert 1-based to 0-based
            orientation = toupper(orientation);

            if (isPlacementValid(player->grid, row, col, shipSizes[i], orientation)) {
                // Place ship and record positions
                if (orientation == 'H') {
                    for (int j = 0; j < shipSizes[i]; j++) {
                        player->grid[row][col + j] = SHIP;
                        player->fleet[i].positions[j][0] = row;
                        player->fleet[i].positions[j][1] = col + j;
                    }
                } else if (orientation == 'V') {
                    for (int j = 0; j < shipSizes[i]; j++) {
                        player->grid[row + j][col] = SHIP;
                        player->fleet[i].positions[j][0] = row + j;
                        player->fleet[i].positions[j][1] = col;
                    }
                }
                // Initialize ship in player's fleet
                strcpy(player->fleet[i].name, shipNames[i]);
                player->fleet[i].size = shipSizes[i];
                player->fleet[i].hits = 0;
                player->fleet[i].sunk = false;

                valid = 1;
            } else {
                printf("Invalid placement. Please try again.\n");
            }
        }
        displayGrid(player->grid, 1);
    }
}

// Function to place ships randomly for the bot
void placeShipsBot(Player *player) {
    int shipSizes[TOTAL_SHIPS] = {5, 4, 3, 2}; // 4 ships
    char *shipNames[TOTAL_SHIPS] = {"Carrier", "Battleship", "Destroyer", "Submarine"};
    srand(time(NULL) + rand()); // Initialize random seed

    for(int i = 0; i < TOTAL_SHIPS; i++) {
        bool placed = false;
        while(!placed) {
            int row = rand() % GRID_SIZE;
            int col = rand() % GRID_SIZE;
            char orientation = (rand() % 2) ? 'H' : 'V';

            if(isPlacementValid(player->grid, row, col, shipSizes[i], orientation)) {
                // Place ship
                if(orientation == 'H') {
                    for(int j = 0; j < shipSizes[i]; j++) {
                        player->grid[row][col + j] = SHIP;
                        player->fleet[i].positions[j][0] = row;
                        player->fleet[i].positions[j][1] = col + j;
                    }
                } else { // 'V'
                    for(int j = 0; j < shipSizes[i]; j++) {
                        player->grid[row + j][col] = SHIP;
                        player->fleet[i].positions[j][0] = row + j;
                        player->fleet[i].positions[j][1] = col;
                    }
                }
                strcpy(player->fleet[i].name, shipNames[i]);
                player->fleet[i].size = shipSizes[i];
                player->fleet[i].hits = 0;
                player->fleet[i].sunk = false;
                placed = true;

                // Debugging statement
                printf("Bot placed %s at ", shipNames[i]);
                for(int j = 0; j < shipSizes[i]; j++) {
                    printf("%c%d ", 'A' + player->fleet[i].positions[j][1], player->fleet[i].positions[j][0] + 1);
                }
                printf("\n");
            }
        }
    }

    printf("Bot has placed all its ships.\n");
}

int takeTurn(Player *currentPlayer, Player *opponent, int trackingDifficulty) {
    // Display the opponent's tracking grid
    printf("\n%s's Tracking Grid:\n", currentPlayer->name);
    displayGrid(currentPlayer->trackingGrid, 0); // Do not show ships

    if (currentPlayer->isBot) {
        // Bot's turn logic
        printf("\n%s is taking its turn...\n", currentPlayer->name);

         currentPlayer->turnsSinceLastRadarSweep++;

        // **Priority 1: Respond to Radar Sweep**
        if (opponent->radarUsedAgainstBotLastTurn && currentPlayer->smokeScreensAvailable > 0) {
            printf("Opponent used a Radar Sweep last turn. Deploying Smoke Screen...\n");
            botSmokeScreen(currentPlayer);
            opponent->radarUsedAgainstBotLastTurn = false; // Reset the flag
            return 0; // Continue game
        }

        // **Priority 2: Deploy Radar Sweep if available and cooldown met**
        if (currentPlayer->radarSweepsUsed < MAX_RADAR_SWEEPS && currentPlayer->turnsSinceLastRadarSweep >= 3) {
            botRadar(currentPlayer, opponent);
            currentPlayer->turnsSinceLastRadarSweep = 0; // Reset cooldown
            return 0; // Continue game
        }
        // **Priority 3: Use Artillery if available**
        if (currentPlayer->artilleryAvailable > 0) {
            botArtillery(currentPlayer, opponent, trackingDifficulty);
            if (opponent->shipsRemaining == 0) {
                return 1; // Game over
            }
            return 0; // Continue game
        }

        // **Priority 4: Use Torpedo if available**
        if (currentPlayer->torpedoAvailable > 0) {
            botTorpedo(currentPlayer, opponent, trackingDifficulty);
            if (opponent->shipsRemaining == 0) {
                return 1; // Game over
            }
            return 0; // Continue game
        }

        // **Priority 5: Perform Firing Action Based on Phases**
        if (botFire(currentPlayer, opponent)) {
            return 1; // Game over
        }

        return 0; // Continue game
    } else {
        // Human player's turn
        printf("\nAvailable moves:\n");
        printf("1. Fire\n");
        if (currentPlayer->radarSweepsUsed < 3) {
            printf("2. Radar (Uses left: %d)\n", 3 - currentPlayer->radarSweepsUsed);
        }
        if (currentPlayer->smokeScreensAvailable > 0) {
            printf("3. Smoke (Available: %d)\n", currentPlayer->smokeScreensAvailable);
        }
        if (currentPlayer->artilleryAvailable) {
            printf("4. Artillery\n");
        }
        if (currentPlayer->torpedoAvailable) {
            printf("5. Torpedo\n");
        }

        // Prompt for player's move
        char command[20];

        printf("\n%s, enter your move: ", currentPlayer->name);
        scanf("%s", command);
        // Clear input buffer
        int c;
        while ((c = getchar()) != '\n' && c != EOF);

        // Convert command to uppercase for consistency
        for (int i = 0; command[i]; i++) {
            command[i] = toupper(command[i]);
        }

        // Handle commands
        if (strcmp(command, "FIRE") == 0) {
            if (fire(currentPlayer, opponent, trackingDifficulty)) {
                return 1; // Game over
            }
        } else if (strcmp(command, "RADAR") == 0) {
            radar(currentPlayer, opponent);
        } else if (strcmp(command, "SMOKE") == 0) {
            smokeScreen(currentPlayer);
        } else if (strcmp(command, "ARTILLERY") == 0 && currentPlayer->artilleryAvailable) {
            artillery(currentPlayer, opponent, trackingDifficulty);
        } else if (strcmp(command, "TORPEDO") == 0 && currentPlayer->torpedoAvailable) {
            torpedo(currentPlayer, opponent, trackingDifficulty);
        } else {
            printf("Invalid command or move not available. You lose your turn.\n");
        }

        return 0; // Game continues
    }
}
// Function to check if all ships are sunk
int allShipsSunk(Player *player) {
    return player->shipsRemaining == 0;
}

//this function handles inout validation 
int parseCoordinates(char *input, int *row, int *col) {
    // Skip leading whitespace
    while (isspace((unsigned char)*input)) input++;

    // Check if input is empty after trimming
    if (strlen(input) < 2 || strlen(input) > 3) return 0;

    // Extract column character
    char colChar = toupper(input[0]);
    if (colChar < 'A' || colChar > 'J') return 0;

    // Extract row number
    char rowStr[3];
    strncpy(rowStr, &input[1], 2);
    rowStr[2] = '\0';
    int rowNum = atoi(rowStr);
    if (rowNum < 1 || rowNum > GRID_SIZE) return 0;

    *col = colChar - 'A';
    *row = rowNum - 1;

    return 1; // Successful parsing
}
// Function to clear the screen (simulate it with newlines)
void clearScreen() {
    #ifdef _WIN32
        system("cls"); // Windows command to clear screen
    #else
        system("clear"); // Unix/Linux command to clear screen
    #endif
}
Ship* findShipAtPosition(Player *player, int row, int col) {
    for(int i = 0; i < TOTAL_SHIPS; i++) {
        for(int j = 0; j < player->fleet[i].size; j++) {
            if(player->fleet[i].positions[j][0] == row && player->fleet[i].positions[j][1] == col) {
                return &player->fleet[i];
            }
        }
    }
    return NULL; // No ship found at the position
}
//fucntion to unlock advanced moves like torpedo and artillary 
void unlockAdvancedMoves(Player *attacker, Player *defender) {
    int shipsSunk = attacker->shipsSunk;

    if (shipsSunk == 1 && !attacker->artilleryAvailable) {
        attacker->artilleryAvailable = 1;
        printf("Artillery strike unlocked for your next turn!\n");
    }

    if (shipsSunk == 3 && !attacker->torpedoAvailable) {
        attacker->torpedoAvailable = 1;
        printf("Torpedo strike unlocked for your next turn!\n");
    }
}