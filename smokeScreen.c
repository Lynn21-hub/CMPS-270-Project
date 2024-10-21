void smokeScreen(char opponentGrid[SIZE][SIZE], int row, int col) {
    // Mark the obscured area with a different character (e.g., 'M' for obscured)
    for (int i = row; i < row + 2; i++) {
        for (int j = col; j < col + 2; j++) {
            if (i < SIZE && j < SIZE) {
                opponentGrid[i][j] = 'M'; // Mark as obscured
            }
        }
    }
    printf("Smoke screen activated!\n");
}
