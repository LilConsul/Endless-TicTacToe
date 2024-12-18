#include <iostream>
#include "minimax.cpp"

int main() {
    cout << "Welcome to Tic-Tac-Toe!";

    bool PC_ON = true;
    char choice = 'y';

    cout << "\nDo you want to play against the computer? (y/n): ";
    cin >> choice;
    if (choice == 'n' || choice == 'N')
        PC_ON = false;

    TicTacToe game(Board(), PC_ON);
    game.startConsole();
    return 0;
}