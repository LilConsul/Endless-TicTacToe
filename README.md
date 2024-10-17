# Endless Tic Tac Toe
Endless Tic Tac Toe is a console-based game that allows players to engage in Tic Tac Toe on an infinite grid. The game features an AI opponent that uses the Minimax algorithm to make optimal moves. Unlike standard 3x3 grid, this game requires players to align 5 "X" or "O" in a row.

## Minimax Algorithm Overview
The Minimax algorithm evaluates all possible moves to determine the best one by considering both the player's and the opponent's potential outcomes. Unlike the standard 3x3 grid, this implementation is designed for an infinite grid, effectively allowing for a 9x9 field, or larger.

This version incorporates alpha-beta pruning to optimize move selection. Additionally, the algorithm limits possible moves to within one cell of any filled spaces, ensuring efficient gameplay without unnecessary computations.

## Installation 

1. Clone the repository:
```bash
git clone https://github.com/LilConsul/Endless-TicTacToe-CPP.git
```

2. Navigate to the project directory and compile the code using any C++ compiler.

## Settings
Find `STATIC_SETTINGS` class to adjust basic setting of the game.
```c++
    const bool DEBUG = false;
    const unsigned int RECURSION_DEPTH = 4;

    static const unsigned int BASIC_SIZE = 7;
    static const unsigned int EXTEND_SIZE = 3;

    const char EMPTY = ' ';
    const char PC_CHAR = 'X';
    const char USER_CHAR = '0';
```
- `RECURSION_DEPTH` - Defines how many moves the AI evaluates ahead. Higher value leads to stronger AI, but slower evaluation process.
- `BASIC_SIZE` - Sets the initial size of the board when the game starts.
- `EXTEND_SIZE` - Determines how much the grid expands when needed.

## Code Integration
To use the Endless TicTacToe in your own projects the small integration example included. 
#### To start default game against PC in console use this:
```c++
#include "minimax.cpp"

int main(){
    TicTacToe gameInstance; // Default constructor, PC is turned ON by default
    gameInstance.startConsole(); // Start game in the console
}
```
#
#### To adjust any setting and use as backend file follow instructions:
```c++
#include "minimax.cpp"

int main(){
    // Adjust board size
    Board board(7);
    bool PC_On = true;

    // Initialize the game itself.
    TicTacToe gameInstance(board, PC_On);

    // 0 => no winner.
    // 1 => the player wins.
    // 2 => the computer wins.
    int winner = gameInstance.checkWinner();


    // Makes the next move for the computer.
    // Must be called only if the PC is activated.
    bool pcMadeMove = gameInstance.pcMakeNextMove();

    //Retrieves the best move for the computer without making it.
    vector<int> pcMove = gameInstance.PC_best();

    // Places a move on the board at the specified coordinates.
    int X_Pos = 0;
    int Y_Pos = 0;
    bool moveSuccess = gameInstance.move(X_Pos, Y_Pos);

    // Returns the current player (1 for player, 2 for PC).
    int whoIsNext = gameInstance.who();

    // Retrieves the current state of the board as a vector of vectors of integers.
    vector<vector<int>> boardInt = gameInstance.getBoard();

    // Retrieves the current state of the board represented as characters.
    vector<vector<char>> boardChar = gameInstance.getBoardChar();
}
```