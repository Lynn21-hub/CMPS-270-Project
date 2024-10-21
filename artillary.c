void artillery(char opponentGrid[SIZE][SIZE], int row, int col) {
    bool hit = false;
    for (int i = row; i < row + 2; i++) {
        for (int j = col; j < col + 2; j++) {
            if (i < SIZE && j < SIZE && opponentGrid[i][j] == 'S') {
                opponentGrid[i][j] = 'X'; // Mark hit
                hit = true;
            }
        }
    }
    if (hit) {
        printf("Artillery: Hit!\n");
    } else {
        printf("Artillery: Miss!\n");
    }
}

