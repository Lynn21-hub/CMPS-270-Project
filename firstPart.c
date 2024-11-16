#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include "battleship.h"
#include "moves.h"




// Constants
#define GRID_SIZE 10
#define WATER '~'
#define SHIP 'S'

#define MAX_NAME_LENGTH 50

// Function declarations
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
void initializePlayer(Player *player);




int main() {
    Player player1, player2;
    int difficulty, firstPlayer;
    int gameOver = 0;

    // Initialize players
    initializePlayer(&player1);
    initializePlayer(&player2);

    // Get player names
    getPlayerNames(&player1, &player2);

    // Choose tracking difficulty
    difficulty = getDifficultyLevel();

    // Randomly select first player
    firstPlayer = chooseFirstPlayer();
    printf("Player %s will go first!\n", (firstPlayer == 1) ? player1.name : player2.name);

    // Clear screen (to preserve ship secrecy)
    clearScreen();

    // Place ships for both players
    printf("%s, place your ships:\n", player1.name);
    placeShips(&player1);
    clearScreen();

    printf("%s, place your ships:\n", player2.name);
    placeShips(&player2);
    clearScreen();

    // Game loop
    int currentPlayer = firstPlayer;
    while (!gameOver) {
        if (currentPlayer == 1) {
            printf("%s's turn:\n", player1.name);
            if (takeTurn(&player1, &player2, difficulty)) {
                printf("%s wins!\n", player1.name);
                gameOver = 1;
            }
            currentPlayer = 2; // Switch turns
        } else {
            printf("%s's turn:\n", player2.name);
            if (takeTurn(&player2, &player1, difficulty)) {
                printf("%s wins!\n", player2.name);
                gameOver = 1;
            }
            currentPlayer = 1; // Switch turns
        }
    }

    return 0;
}

void initializePlayer(Player *player) {
    // 1. Initialize the player's grid
    initializeGrid(player->grid);

    // 2. Reset counters and flags
    player->radarSweepsUsed = 0;
    player->smokeScreensAvailable = 0; // Set to 0 or starting number
    player->artilleryAvailable = 0;
    player->torpedoAvailable = 0;
    player->shipsRemaining = TOTAL_SHIPS;

    // 3. Initialize the smoke screen grid
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            player->smokeScreenGrid[i][j] = 0;
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
    printf("  A B C D E F G H I J\n");
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
    printf("Enter name for Player 2: ");
    fgets(player2->name, MAX_NAME_LENGTH, stdin);

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
            col = colChar - 'A'; // Convert letter to index
            row -= 1;            // Convert 1-based to 0-based
            orientation = (char) toupper(orientation);

            if (isPlacementValid(player->grid, row, col, shipSizes[i], orientation)) {
                // Place ship
                if (orientation == 'H') {
                    for (int j = 0; j < shipSizes[i]; j++) {
                        player->grid[row][col + j] = SHIP;
                    }
                } else if (orientation == 'V') {
                    for (int j = 0; j < shipSizes[i]; j++) {
                        player->grid[row + j][col] = SHIP;
                    }
                }
                valid = 1;
            } else {
                printf("Invalid placement. Please try again.\n");
            }
        }
        displayGrid(player->grid, 1);
    }
}


// Function to take a turn, returns 1 if the game is won, 0 otherwise
int takeTurn(Player *currentPlayer, Player *opponent, int trackingDifficulty) {
    // Display the opponent's grid
    printf("\n%s's Tracking Grid:\n", currentPlayer->name);
    displayGrid(opponent->grid, trackingDifficulty == 1); // Show misses in Easy mode

    // Show available moves
    printf("\nAvailable moves:\n");
    printf("1. Fire [coordinate]\n");
    if (currentPlayer->radarSweepsUsed < 3) {
        printf("2. Radar [top-left coordinate] (Uses left: %d)\n", 3 - currentPlayer->radarSweepsUsed);
    }
    // Include advanced moves if they are available
    if (currentPlayer->artilleryAvailable) {
        printf("3. Artillery [coordinate]\n");
    }
    if (currentPlayer->torpedoAvailable) {
        printf("4. Torpedo [coordinate]\n");
    }
    // Assume Smoke Screen is always available
    printf("5. Smoke\n");

    // Prompt for player's move
    char command[20];
    char parameters[20];

    printf("\n%s, enter your move: ", currentPlayer->name);
    scanf("%s", command);
    // Clear input buffer to read the rest of the line
    fgets(parameters, sizeof(parameters), stdin);
    // Remove newline character from parameters
    parameters[strcspn(parameters, "\n")] = 0;

    // Convert command to uppercase for consistency
    for (int i = 0; command[i]; i++) {
        command[i] = (char) toupper(command[i]);

    }

    // Handle commands
    if (strcmp(command, "FIRE") == 0) {
        if (fire(currentPlayer, opponent, trackingDifficulty)) {
            return 1; // Game over
        }
    } else if (strcmp(command, "RADAR") == 0) {
        radar(currentPlayer, opponent);
    } else if (strcmp(command, "SMOKE") == 0) {
        // Implement smoke screen functionality
        smokeScreen(currentPlayer);
    } else if (strcmp(command, "ARTILLERY") == 0 && currentPlayer->artilleryAvailable) {
        // Implement artillery functionality
        artillery(currentPlayer, opponent);
    } else if (strcmp(command, "TORPEDO") == 0 && currentPlayer->torpedoAvailable) {
        // Implement torpedo functionality
        torpedo(currentPlayer, opponent);
    } else {
        printf("Invalid command or move not available. You lose your turn.\n");
    }

    return 0; // Game continues
}


// Function to check if all ships are sunk
int allShipsSunk(Player *player) {
    return player->shipsRemaining == 0;
}





