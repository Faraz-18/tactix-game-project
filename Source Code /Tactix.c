#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>  // For rand() and srand()
#include <time.h>    // For seeding rand()

#define EMPTY 0
#define X_PLAYER 1
#define O_PLAYER 2

Sound mark;
Sound wins;
Sound victory;

int grid[3][3] = {{EMPTY, EMPTY, EMPTY},
                  {EMPTY, EMPTY, EMPTY},
                  {EMPTY, EMPTY, EMPTY}};

int currentPlayer = X_PLAYER;
int space = 0;

// Function Prototypes
void ResetGame();
int Winner();
void DrawMenu(int *menuActive, int *singlePlayer);
void AITurn();
int FindBestMove();
int IsWinningMove(int player, int row, int col);
int GetEmptyCell();

// Function to reset the game
void ResetGame() {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            grid[i][j] = EMPTY;
        }
    }
    space = 0;
    currentPlayer = X_PLAYER;
}

// Function to check for a winner
int Winner() {
    for (int i = 0; i < 3; i++) {
        if (grid[i][0] != EMPTY && grid[i][0] == grid[i][1] && grid[i][1] == grid[i][2]) {
            return grid[i][0];
        }
    }
    for (int j = 0; j < 3; j++) {
        if (grid[0][j] != EMPTY && grid[0][j] == grid[1][j] && grid[1][j] == grid[2][j]) {
            return grid[0][j];
        }
    }
    if (grid[0][0] != EMPTY && grid[0][0] == grid[1][1] && grid[1][1] == grid[2][2]) {
        return grid[0][0];
    }
    if (grid[0][2] != EMPTY && grid[0][2] == grid[1][1] && grid[1][1] == grid[2][0]) {
        return grid[0][2];
    }
    return EMPTY;
}

// Menu graphics and selection logic
void DrawMenu(int *menuActive, int *singlePlayer) {
    BeginDrawing();
    ClearBackground(DARKGRAY);

    // Title
    DrawText("TACTIX GAME", 150, 50, 50, RAYWHITE);

    // Single Player Option
    DrawRectangle(150, 200, 300, 100, LIGHTGRAY);
    DrawText("SINGLE PLAYER", 175, 230, 30, BLACK);

    // Multiplayer Option
    DrawRectangle(150, 350, 300, 100, LIGHTGRAY);
    DrawText("MULTIPLAYER", 195, 380, 30, BLACK);

    // Check for mouse clicks on menu options
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mousePos = GetMousePosition();
        if (mousePos.x > 150 && mousePos.x < 450 && mousePos.y > 200 && mousePos.y < 300) {
            *singlePlayer = 1; // Set game mode to Single Player
            *menuActive = 0;   // Exit menu and start the game
        }
        if (mousePos.x > 150 && mousePos.x < 450 && mousePos.y > 350 && mousePos.y < 450) {
            *singlePlayer = 0; // Set game mode to Multiplayer
            *menuActive = 0;   // Exit menu and start the game
        }
    }

    EndDrawing();
}

// AI's turn to make a move
void AITurn() {
    int cell = FindBestMove();
    if (cell != -1) {
        int row = cell / 3;
        int col = cell % 3;
        grid[row][col] = O_PLAYER;  // AI plays as 'O'
        PlaySound(mark);
        space++;
        currentPlayer = X_PLAYER;  // Switch back to player X
    }
}

// Find the best move for AI
int FindBestMove() {
    // Try to find a winning move or block the opponent's winning move
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (grid[i][j] == EMPTY) {
                // Check if AI can win
                if (IsWinningMove(O_PLAYER, i, j)) {
                    return i * 3 + j;
                }
                // Check if AI needs to block player X
                if (IsWinningMove(X_PLAYER, i, j)) {
                    return i * 3 + j;
                }
            }
        }
    }

    // If no immediate win/block, pick the first empty cell
    return GetEmptyCell();
}

// Check if placing a move results in a win
int IsWinningMove(int player, int row, int col) {
    grid[row][col] = player;  // Temporarily place the move
    int win = Winner();      // Check if it leads to a win
    grid[row][col] = EMPTY;  // Undo the move
    return win == player;
}

// Get the first empty cell for fallback
int GetEmptyCell() {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (grid[i][j] == EMPTY) {
                return i * 3 + j;
            }
        }
    }
    return -1;
}

int main(void) {
    const int screenWidth = 600;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Tactix");
    InitAudioDevice();

    mark = LoadSound("move.wav");
    wins = LoadSound("start.wav");
    victory = LoadSound("wins.wav");
    PlaySound(wins);

    SetTargetFPS(60);
    srand(time(0));  // Seed random number generator for AI moves

    int gridSize = 3;
    int cellSize = screenWidth / gridSize;

    int menuActive = 1;      // Menu active state
    int singlePlayer = 0;    // Game mode (1 = Single Player, 0 = Multiplayer)

    while (!WindowShouldClose()) {
        if (menuActive) {
            DrawMenu(&menuActive, &singlePlayer);
            continue;
        }

        int win = Winner();

        if (space < 9 && win == EMPTY) {
            if (currentPlayer == X_PLAYER) {
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    int mouseX = GetMouseX();
                    int mouseY = GetMouseY();

                    int row = mouseY / cellSize;
                    int col = mouseX / cellSize;

                    if (grid[row][col] == EMPTY) {
                        grid[row][col] = X_PLAYER;
                        PlaySound(mark);
                        space++;
                        currentPlayer = (singlePlayer) ? O_PLAYER : O_PLAYER;  // AI or Player 2
                    }
                }
            } else if (currentPlayer == O_PLAYER) {
                if (singlePlayer) {
                    AITurn();  // AI turn
                } else {
                    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                        int mouseX = GetMouseX();
                        int mouseY = GetMouseY();

                        int row = mouseY / cellSize;
                        int col = mouseX / cellSize;

                        if (grid[row][col] == EMPTY) {
                            grid[row][col] = O_PLAYER;
                            PlaySound(mark);
                            space++;
                            currentPlayer = X_PLAYER;
                        }
                    }
                }
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (space < 9 && win == EMPTY) {
            for (int i = 1; i < gridSize; i++) {
                DrawLine(i * cellSize, 0, i * cellSize, screenHeight, BLACK);
                DrawLine(0, i * cellSize, screenWidth, i * cellSize, BLACK);
            }

            for (int row = 0; row < gridSize; row++) {
                for (int col = 0; col < gridSize; col++) {
                    if (grid[row][col] == X_PLAYER) {
                        DrawText("X", col * cellSize + cellSize / 3, row * cellSize + cellSize / 3, 80, DARKBLUE);
                    } else if (grid[row][col] == O_PLAYER) {
                        DrawText("O", col * cellSize + cellSize / 3, row * cellSize + cellSize / 3, 80, DARKGREEN);
                    }
                }
            }

            char turnText[20];
            snprintf(turnText, sizeof(turnText), "Player %s's turn", currentPlayer == X_PLAYER ? "X" : "O");
            DrawText(turnText, screenWidth / 2 - MeasureText(turnText, 20) / 2, screenHeight - 40, 20, DARKGRAY);

        } else if (space == 9 && win == EMPTY) {
            DrawText("Grid is full! Press R to return to menu", screenWidth / 2 - MeasureText("Grid is full! Press R to return to menu", 20) / 2, screenHeight / 2, 20, BLUE);

            if (IsKeyPressed(KEY_R)) {
                ResetGame();
                menuActive = 1; // Return to menu
            }
        } else if (win != EMPTY) {
            PlaySound(victory);
            DrawText(win == X_PLAYER ? "Player X Wins!" : "Player O Wins!", screenWidth / 2 - MeasureText("Player X Wins!", 40) / 2, screenHeight / 2, 40, GREEN);
            DrawText("Press R to return to menu", screenWidth / 2 - MeasureText("Press R to return to menu", 20) / 2, screenHeight / 2 + 50, 20, DARKGRAY);

            if (IsKeyPressed(KEY_R)) {
                ResetGame();
                menuActive = 1; // Return to menu
            }
        }

        EndDrawing();
    }

    UnloadSound(mark);
    UnloadSound(wins);
    UnloadSound(victory);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}
