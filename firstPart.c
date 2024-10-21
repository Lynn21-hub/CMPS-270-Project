#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Constants
#define GRID_SIZE 10
#define WATER '~'
#define SHIP 'S'
#define HIT 'X'
#define MISS 'O'
#define MAX_NAME_LENGTH 50

// Function declarations
void initializeGrid(char grid[GRID_SIZE][GRID_SIZE]);
void displayGrid(char grid[GRID_SIZE][GRID_SIZE], int showShips);
int getDifficultyLevel();
void getPlayerNames(char player1[], char player2[]);
int chooseFirstPlayer();
void clearScreen();
int isPlacementValid(char grid[GRID_SIZE][GRID_SIZE], int row, int col, int length, char orientation);
void placeShips(char grid[GRID_SIZE][GRID_SIZE], char playerName[]);
int takeTurn(char playerName[], char opponentGrid[GRID_SIZE][GRID_SIZE], int difficulty);
int allShipsSunk(char grid[GRID_SIZE][GRID_SIZE]);

int main() {
    char player1[MAX_NAME_LENGTH], player2[MAX_NAME_LENGTH];
    char grid1[GRID_SIZE][GRID_SIZE], grid2[GRID_SIZE][GRID_SIZE]; // Player 1 and Player 2's grids
    int difficulty, firstPlayer;
    int gameOver = 0;

    // Initialize grids
    initializeGrid(grid1);
    initializeGrid(grid2);

    // Get player names
    getPlayerNames(player1, player2);

    // Choose tracking difficulty
    difficulty = getDifficultyLevel();

    // Randomly select first player
    firstPlayer = chooseFirstPlayer();
    printf("Player %s will go first!\n", (firstPlayer == 1) ? player1 : player2);

    // Clear screen (to preserve ship secrecy)
    clearScreen();

    // Place ships for both players
    printf("%s, place your ships:\n", player1);
    placeShips(grid1, player1);
    clearScreen();

    printf("%s, place your ships:\n", player2);
    placeShips(grid2, player2);
    clearScreen();

    // Game loop
    int currentPlayer = firstPlayer;
    while (!gameOver) {
        if (currentPlayer == 1) {
            printf("%s's turn:\n", player1);
            if (takeTurn(player1, grid2, difficulty)) {
                printf("%s wins!\n", player1);
                gameOver = 1;
            }
            currentPlayer = 2; // Switch turns
        } else {
            printf("%s's turn:\n", player2);
            if (takeTurn(player2, grid1, difficulty)) {
                printf("%s wins!\n", player2);
                gameOver = 1;
            }
            currentPlayer = 1; // Switch turns
        }
    }

    return 0;
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
        printf("%d ", i + 1);
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
void getPlayerNames(char player1[], char player2[]) {
    printf("Enter name for Player 1: ");
    scanf("%s", player1);
    printf("Enter name for Player 2: ");
    scanf("%s", player2);
}

// Function to randomly choose the first player
int chooseFirstPlayer() {
    srand(time(NULL));
    return rand() % 2 + 1;
}

// Function to clear the screen (simulate it with newlines)
void clearScreen() {
    for (int i = 0; i < 50; i++) {
        printf("\n");
    }
}

// Function to check if a ship can be placed at the given position
int isPlacementValid(char grid[GRID_SIZE][GRID_SIZE], int row, int col, int length, char orientation) {
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
void placeShips(char grid[GRID_SIZE][GRID_SIZE], char playerName[]) {
    int shipSizes[] = { 5, 4, 3, 3, 2 }; // Example ship sizes
    char shipNames[][20] = { "Carrier", "Battleship", "Destroyer", "Submarine", "Patrol Boat" };
    int row, col;
    char orientation;

    for (int i = 0; i < 5; i++) {
        int valid = 0;
        while (!valid) {
            printf("Enter the starting coordinates (e.g., A5) and orientation (H/V) for your %s: ", shipNames[i]);
            char colChar;
            scanf(" %c%d %c", &colChar, &row, &orientation);
            col = colChar - 'A'; // Convert letter to index
            row -= 1; // Convert 1-based to 0-based

            if (isPlacementValid(grid, row, col, shipSizes[i], orientation)) {
                if (orientation == 'H') {
                    for (int j = 0; j < shipSizes[i]; j++) {
                        grid[row][col + j] = SHIP;
                    }
                } else if (orientation == 'V') {
                    for (int j = 0; j < shipSizes[i]; j++) {
                        grid[row + j][col] = SHIP;
                    }
                }
                valid = 1;
            } else {
                printf("Invalid placement. Please try again.\n");
            }
        }
        displayGrid(grid, 1);
    }
}

// Function to take a turn, returns 1 if the game is won, 0 otherwise
int takeTurn(char playerName[], char opponentGrid[GRID_SIZE][GRID_SIZE], int difficulty) {
    int row, col;
    char colChar;

    displayGrid(opponentGrid, difficulty == 1); // Show misses in easy mode, only hits in hard mode

    printf("%s, enter the coordinates for your shot (e.g., A5): ", playerName);
    scanf(" %c%d", &colChar, &row);
    col = colChar - 'A'; // Convert letter to index
    row -= 1; // Convert 1-based to 0-based

    if (opponentGrid[row][col] == SHIP) {
        printf("Hit!\n");
        opponentGrid[row][col] = HIT;
    } else if (opponentGrid[row][col] == WATER) {
        printf("Miss!\n");
        opponentGrid[row][col] = MISS;
    } else {
        printf("You've already fired at this location. Try again.\n");
        return 0; // Retry the turn
    }

    return allShipsSunk(opponentGrid);
}

// Function to check if all ships are sunk
int allShipsSunk(char grid[GRID_SIZE][GRID_SIZE]) {
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (grid[i][j] == SHIP) {
                return 0; // There are still ships remaining
            }
        }
    }
    return 1; // All ships are sunk
}
