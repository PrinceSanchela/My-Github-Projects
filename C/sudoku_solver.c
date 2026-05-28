#include <stdio.h>
#include <stdbool.h>

#define N 9

// Function to print the Sudoku board in a pretty grid
void printBoard(int board[N][N])
{
    printf("\n+-------+-------+-------+\n");
    for (int r = 0; r < N; ++r)
    {
        for (int c = 0; c < N; ++c)
        {
            if (c % 3 == 0)
                printf("| ");
            if (board[r][c] == 0)
                printf(". "); // show empty cells as dots
            else
                printf("%d ", board[r][c]);
        }
        printf("|\n");
        if ((r + 1) % 3 == 0)
            printf("+-------+-------+-------+\n");
    }
}

// Check if a number already exists in the given row
bool usedInRow(int board[N][N], int row, int num)
{
    for (int col = 0; col < N; ++col)
        if (board[row][col] == num)
            return true;
    return false;
}

// Check if a number already exists in the given column
bool usedInCol(int board[N][N], int col, int num)
{
    for (int row = 0; row < N; ++row)
        if (board[row][col] == num)
            return true;
    return false;
}

// Check if a number already exists in the 3×3 box
bool usedInBox(int board[N][N], int boxStartRow, int boxStartCol, int num)
{
    for (int r = 0; r < 3; ++r)
        for (int c = 0; c < 3; ++c)
            if (board[boxStartRow + r][boxStartCol + c] == num)
                return true;
    return false;
}

// Check if placing a number is safe
bool isSafe(int board[N][N], int row, int col, int num)
{
    return !usedInRow(board, row, num) &&
           !usedInCol(board, col, num) &&
           !usedInBox(board, row - row % 3, col - col % 3, num);
}

// Find an unfilled position
bool findUnassignedLocation(int board[N][N], int *row, int *col)
{
    for (*row = 0; *row < N; ++(*row))
        for (*col = 0; *col < N; ++(*col))
            if (board[*row][*col] == 0)
                return true;
    return false;
}

// Backtracking Sudoku solver
bool solveSudoku(int board[N][N])
{
    int row, col;

    if (!findUnassignedLocation(board, &row, &col))
        return true; // solved!

    for (int num = 1; num <= 9; ++num)
    {
        if (isSafe(board, row, col, num))
        {
            board[row][col] = num;

            if (solveSudoku(board))
                return true;

            board[row][col] = 0; // backtrack
        }
    }
    return false;
}

int main()
{
    int board[N][N];

    printf("\n==============================\n");
    printf("       SUDOKU SOLVER  \n");
    printf("==============================\n\n");
    printf("Made BY Prince Sanchela\n\n");
    printf(" Enter your Sudoku (9x9 grid)\n");
    printf(" Use 0 for empty cells\n\n");

    for (int i = 0; i < N; ++i)
    {
        printf("Enter row %d (9 numbers separated by spaces): ", i + 1);
        for (int j = 0; j < N; ++j)
            scanf("%d", &board[i][j]);
    }

    printf("\nYour Input Sudoku:\n");
    printBoard(board);

    if (solveSudoku(board))
    {
        printf("\n✅ SOLVED SUDOKU:\n");
        printBoard(board);
    }
    else
    {
        printf("\n❌ No solution exists for the given Sudoku.\n");
    }

    printf("\n==============================\n");
    printf("        THANK YOU! 😊\n");
    printf("==============================\n");

    return 0;
}
