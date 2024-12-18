#ifndef MINIMAX_CPP
#define MINIMAX_CPP

#include <vector>
#include <iostream>

using namespace std;

class STATIC_SETTINGS {
protected:
    //Adjustable settings
    static constexpr bool DEBUG = false;
    static constexpr unsigned int RECURSION_DEPTH = 4;

    static constexpr unsigned int BASIC_SIZE = 7;
    static constexpr unsigned int EXTEND_SIZE = 3;

    static constexpr char EMPTY = ' ';
    static constexpr char PC_CHAR = 'X';
    static constexpr char USER_CHAR = '0';

    //Non-adjustable settings. DON'T TOUCH THEM
    static constexpr int WIN_SCORE = 1000000;
    static constexpr int NULL_INT = -10000;
    static constexpr double NULL_DOUBLE = -10000.0;
    const vector<double> NULL_VEC_DOUBLE = {NULL_DOUBLE, NULL_DOUBLE, NULL_DOUBLE};
    const vector<int> NULL_VEC_INT = {NULL_INT, NULL_INT, NULL_INT};
};

class Board : protected STATIC_SETTINGS {
private:
    vector<vector<int>> matrix;

public:
    Board(int boardSize = BASIC_SIZE) {
        matrix = vector<vector<int>>(boardSize, vector<int>(boardSize, 0));
    }

    Board(const Board &other) {
        matrix = other.matrix;
    }

    Board &operator=(const Board &other) {
        if (this == &other) {
            return *this;
        }
        matrix = other.matrix;
        return *this;
    }

public:
    int getBoardSize() {
        return matrix.size();
    }

    void remove(int posX, int posY) {
        matrix[posY][posX] = 0;
    }

    void move_noUser(int posX, int posY, bool isX) {
        matrix[posY][posX] = isX ? 2 : 1;
    }

    bool move(int posX, int posY, bool isX) {
        if (posX < 0 || posY < 0 || posX > matrix[0].size() || posY > matrix.size()) return false;
        if (matrix[posY][posX] != 0) return false;
        matrix[posY][posX] = isX ? 2 : 1;
        return true;
    }

    vector<pair<int, int>> generateMoves() {
        vector<pair<int, int>> moveList;
        int rows = static_cast<int>(matrix.size());
        if (rows == 0) return moveList;
        int cols = static_cast<int>(matrix[0].size());

        const vector<pair<int, int>> directions = {
                {-1, 0}, // North
                {-1, 1}, // Northeast
                {0,  1}, // East
                {1,  1}, // Southeast
                {1,  0}, // South
                {1,  -1}, // Southwest
                {0,  -1}, // West
                {-1, -1}  // Northwest
        };

        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                if (matrix[i][j] > 0) continue;

                //Easier to check for every direction
                for (const auto &dir: directions) {
                    int ni = i + dir.first;
                    int nj = j + dir.second;

                    if (ni >= 0 && ni < rows && nj >= 0 && nj < cols) {
                        if (matrix[ni][nj] > 0) {
                            moveList.emplace_back(i, j);
                            break;
                        }
                    }

                }
            }
        }

        return moveList;
    }

    vector<vector<int>> getBoardMatrix() {
        return matrix;
    }

    vector<vector<char>> getBoard_char() {
        vector<vector<char>> myBoard(matrix.size(), vector<char>(matrix[0].size(), '~'));
        for (int i = 0; i < matrix.size(); i++)
            for (int j = 0; j < matrix[0].size(); j++) {
                int curr = matrix[i][j];
                if (curr == 0) myBoard[i][j] = EMPTY;
                if (curr == 1) myBoard[i][j] = PC_CHAR;
                if (curr == 2) myBoard[i][j] = USER_CHAR;
            }
        if (DEBUG) cout << "\n*Size. i: " << matrix[0].size() << "; j: " << matrix.size();
        return myBoard;
    }

    void extend(int &posY, int &posX) {
        if (EXTEND_SIZE <= 0) return;

        int extend_TOP = EXTEND_SIZE - posX;
        int extend_BOTTOM = posX + EXTEND_SIZE - (matrix.size() - 1);
        int extend_LEFT = EXTEND_SIZE - posY;
        int extend_RIGHT = posY + EXTEND_SIZE - (matrix[0].size() - 1);

        for (int i = 0; i < extend_TOP; i++) {
            matrix.insert(matrix.begin(), vector<int>(matrix[0].size(), 0));
            posX += 1;
        }

        for (int i = 0; i < extend_BOTTOM; i++)
            matrix.push_back(vector<int>(matrix[0].size(), 0));

        for (int i = 0; i < extend_LEFT; i++) {
            for (int j = 0; j < matrix.size(); j++)
                matrix[j].insert(matrix[j].begin(), 0);
            posY += 1;
        }

        for (int i = 0; i < extend_RIGHT; i++)
            for (int j = 0; j < matrix.size(); j++)
                matrix[j].push_back(0);
    }

    void print() {
        vector<vector<char>> temp_board = getBoard_char();
        int rows = temp_board.size();
        int cols = temp_board[0].size();

        cout << "\n     ";
        for (int j = 0; j < cols; j++)
            cout << "  " << j << "    ";
        cout << "\n    ";

        for (int j = 0; j < cols; j++)
            cout << "-------";
        cout << "-\n";

        for (int i = 0; i < rows; i++) {
            cout << "  " << i << " | ";
            for (int j = 0; j < cols; j++)
                cout << "  " << temp_board[i][j] << "  | ";
            cout << "\n    ";

            for (int j = 0; j < cols; j++)
                cout << "-------";
            cout << "-\n";
        }
    }
};

class Algorithm : protected STATIC_SETTINGS {
private:
    int evaluationCount = 0;
    Board board;

    static void evaluateWay(vector<vector<int>> &matrix, int i, int j,
                            bool isBot, bool botsTurn, vector<int> &evaluation_grades) {
        if (matrix[i][j] == (isBot ? 2 : 1)) {
            evaluation_grades[0]++;
        } else if (matrix[i][j] == 0) {
            if (evaluation_grades[0] > 0) {
                evaluation_grades[1]--;
                evaluation_grades[2] += rateEvaluation(evaluation_grades[0], evaluation_grades[1], isBot == botsTurn);
                evaluation_grades[0] = 0;
            }
            evaluation_grades[1] = 1;
        } else {
            if (evaluation_grades[0] > 0) {
                evaluation_grades[2] += rateEvaluation(evaluation_grades[0], evaluation_grades[1], isBot == botsTurn);
                evaluation_grades[0] = 0;
            }
            evaluation_grades[1] = 2;
        }
    }

    static void evaluateWay_OnePass(vector<int> &evaluation_grades, bool isBot, bool playersTurn) {
        if (evaluation_grades[0] > 0) {
            evaluation_grades[2] += rateEvaluation(evaluation_grades[0], evaluation_grades[1], isBot == playersTurn);
        }
        evaluation_grades[0] = 0;
        evaluation_grades[1] = 2;
    }

    double evaluateForPlayer(Board &board, bool blacksTurn) {
        evaluationCount++;
        double blackScore = getScore(board, true, blacksTurn);
        double whiteScore = getScore(board, false, blacksTurn);
        return whiteScore / (blackScore == 0 ? 1.0 : blackScore);
    }

    static int evaluateHorizontal(vector<vector<int>> &matrix, bool forBot, bool botsTurn) {
        vector<int> evaluations = {0, 2, 0};
        for (vector<int> &row: matrix) {
            for (int j = 0; j < row.size(); j++) {
                evaluateWay(matrix, &row - &matrix[0], j, forBot, botsTurn, evaluations);
            }
            evaluateWay_OnePass(evaluations, forBot, botsTurn);
        }
        return evaluations[2];
    }

    static int evaluateVertical(vector<vector<int>> &matrix, bool forBot, bool botsTurn) {
        vector<int> evaluations = {0, 2, 0};
        for (int j = 0; j < matrix[0].size(); j++) {
            for (int i = 0; i < matrix.size(); i++) {
                evaluateWay(matrix, i, j, forBot, botsTurn, evaluations);
            }
            evaluateWay_OnePass(evaluations, forBot, botsTurn);
        }
        return evaluations[2];
    }

    static int evaluateDiagonal(vector<vector<int>> &matrix, bool forBot, bool botsTurn) {
        vector<int> evaluations = {0, 2, 0};
        for (int k = 0; k <= 2 * (matrix.size() - 1); k++) {
            int iStart = max(0, k - (int) matrix.size() + 1);
            int iEnd = min((int) matrix.size() - 1, k);
            for (int i = iStart; i <= iEnd; ++i) {
                evaluateWay(matrix, i, k - i, forBot, botsTurn, evaluations);
            }
            evaluateWay_OnePass(evaluations, forBot, botsTurn);
        }
        for (int k = 1 - matrix.size(); k < matrix.size(); k++) {
            int iStart = max(0, k);
            int iEnd = min((int) matrix.size() + k - 1, (int) matrix.size() - 1);
            for (int i = iStart; i <= iEnd; ++i) {
                evaluateWay(matrix, i, i - k, forBot, botsTurn, evaluations);
            }
            evaluateWay_OnePass(evaluations, forBot, botsTurn);
        }
        return evaluations[2];
    }

    static int rateEvaluation(int count, int blocked, bool currentTurn) {
        if (blocked == 2 && count < 5) return 0;
        int winGuarantee = WIN_SCORE * 10;
        switch (count) {
            case 5:
                return WIN_SCORE;
            case 4:
                return currentTurn ? winGuarantee : (blocked == 0 ? winGuarantee / 4 : 200);
            case 3:
                return blocked == 0 ? (currentTurn ? 50000 : 200) : (currentTurn ? 10 : 5);
            case 2:
                return blocked == 0 ? (currentTurn ? 7 : 5) : 3;
            case 1:
                return 1;
            default:
                return WIN_SCORE * 2;
        }
    }

    vector<double> minimax(int depth, Board &tempBoard, bool max, double alpha, double beta) {
        // The basic minimax algorithm consists in checking all possible moves and evaluating each of them
        // By evaluating both own moves and the opponent's (min and max scores), the algorithm chooses the best.
        //
        // For the 3*3 field, never set the maximum depth of recursion in squiggles
        // In our case, we have a field of 19*19 (or more, but for the algorithm it can be considered infinite)
        //
        // Also, in the current implementation, it is supplemented by alpha–beta pruning of some moves
        // In addition to this, the function to generate possible moves does not consider moves outside the escalated events
        // (No further than 1 cell from all filled cells)

        if (depth == 0) return {evaluateForPlayer(tempBoard, !max), NULL_DOUBLE, NULL_DOUBLE};
        vector<pair<int, int>> allPossibleMoves = tempBoard.generateMoves();
        if (allPossibleMoves.empty()) return {evaluateForPlayer(tempBoard, !max), NULL_DOUBLE, NULL_DOUBLE};

        vector<double> bestMove(3, 0.0);
        if (max) {
            // [0] - Score, [1] - x-coordinate, [2] - y-coordinate
            bestMove = {-1.0, 0.0, 0.0};

            for (pair<int, int> &move: allPossibleMoves) {
                tempBoard.move_noUser(move.second, move.first, false);
                vector<double> tempMove = minimax(depth - 1, tempBoard, false, alpha, beta);
                tempBoard.remove(move.second, move.first);

                if (tempMove[0] > alpha) alpha = tempMove[0];
                if (tempMove[0] >= beta) return tempMove;
                if (tempMove[0] > bestMove[0]) {
                    bestMove = tempMove;
                    bestMove[1] = move.first;
                    bestMove[2] = move.second;
                }
            }
        } else {
            // [0] - Score, [1] - x-coordinate, [2] - y-coordinate
            bestMove = {(double) WIN_SCORE, (double) allPossibleMoves[0].first, (double) allPossibleMoves[0].second};

            for (pair<int, int> &move: allPossibleMoves) {
                tempBoard.move_noUser(move.second, move.first, true);
                vector<double> tempMove = minimax(depth - 1, tempBoard, true, alpha, beta);
                tempBoard.remove(move.second, move.first);

                if (tempMove[0] < beta) beta = tempMove[0];
                if (tempMove[0] <= alpha) return tempMove;
                if (tempMove[0] < bestMove[0]) {
                    bestMove = tempMove;
                    bestMove[1] = move.first;
                    bestMove[2] = move.second;
                }
            }
        }
        return bestMove;
    }

    vector<double> getWinningMove(Board &currentBoard) {
        vector<pair<int, int>> allPossibleMoves = currentBoard.generateMoves();
        for (pair<int, int> &move: allPossibleMoves) {
            evaluationCount++;
            Board tmp(currentBoard);
            tmp.move_noUser(move.second, move.first, false);
            if (getScore(tmp, false, false) >= WIN_SCORE) return {0.0, (double) move.first, (double) move.second};
        }
        return NULL_VEC_DOUBLE;
    }

public:
    explicit Algorithm(const Board &board) : board(board) {}

    int getScore(Board &currentBoard, bool forBot, bool botsTurn) {
        vector<vector<int>> boardMatrix = currentBoard.getBoardMatrix();
        return evaluateHorizontal(boardMatrix, forBot, botsTurn) +
               evaluateVertical(boardMatrix, forBot, botsTurn) +
               evaluateDiagonal(boardMatrix, forBot, botsTurn);
    }

    vector<int> getNextMove(int depth) {
        vector<double> bestMove = getWinningMove(board);
        if (bestMove == NULL_VEC_DOUBLE) {
            Board newBoard(board);
            bestMove = minimax(depth, newBoard, true, -1.0, WIN_SCORE);
        }
        return {static_cast<int>(bestMove[1]), static_cast<int>(bestMove[2])};
    }
};

class TicTacToe : protected STATIC_SETTINGS {
private:
    Board board;
    bool isPlayerTurn = true;
    bool PC_ACTIVATED;
    int winner;

    bool placeMove(int posX, int posY, bool isX) {
        bool success = board.move(posX, posY, isX);
        if (success) board.extend(posX, posY);
        return success;
    }

public:
    explicit TicTacToe(Board board = Board(), bool PC_ON = true) : board(board), PC_ACTIVATED(PC_ON), winner(0) {
        if (DEBUG) cout << "\n*PC_ACTIVATED: " << PC_ACTIVATED;
        if (PC_ACTIVATED) {
            placeMove(board.getBoardSize() / 2, board.getBoardSize() / 2, false);
            isPlayerTurn = true;
        }
    }

    TicTacToe &operator=(const TicTacToe &other) {
        if (this != &other) {
            board = other.board;
            isPlayerTurn = other.isPlayerTurn;
            PC_ACTIVATED = other.PC_ACTIVATED;
            winner = other.winner;
        }
        return *this;
    }

    TicTacToe(const TicTacToe &other) :
            board(other.board),
            isPlayerTurn(other.isPlayerTurn),
            PC_ACTIVATED(other.PC_ACTIVATED),
            winner(other.winner) {}

    void pcGame() {
        if (isPlayerTurn) {
            cout << "\nYour turn: ";
            int x, y;
            cin >> x >> y;
            if (!move(x, y)) {
                board.print();
                cout << "\nYou can't move this direction! Try again.";
            }
        } else {
            pcMakeNextMove();
            board.print();
        }
    }

    void playerGame() {
        board.print();
        char currChar = who() == 1 ? PC_CHAR : USER_CHAR;
        cout << "\nInput your turn (" << currChar << "): ";
        int x, y;
        cin >> x >> y;
        if (!move(x, y)) cout << "\nYou can't move this direction! Try again.";
    }

    string makeWinString() {
        string whoWon = "\n\n";
        if (winner == 1) whoWon += (PC_ACTIVATED ? "PC" : string(1, PC_CHAR));
        else whoWon += (PC_ACTIVATED ? "Player" : string(1, USER_CHAR));

        whoWon += " won the game! Congratulations!";
        return whoWon;
    }

    void startConsole() {
        if (PC_ACTIVATED) board.print();
        while (true) {
            winner = checkWinner();
            if (winner != 0) {
                board.print();
                cout << makeWinString();
                return;
            }
            if (PC_ACTIVATED) pcGame();
            else playerGame();
        }
    }

    int checkWinner() {
        Algorithm check(board);
        if (check.getScore(board, true, false) >= WIN_SCORE) return 2;
        if (check.getScore(board, false, true) >= WIN_SCORE) return 1;
        return 0;
    }

    bool pcMakeNextMove() {
        if (!PC_ACTIVATED) {
            cerr << "PC is not activated, error occurred while trying to initiate pcMakeNextMove." << endl;
            return false;
        }
        try {
            Algorithm PC(board);
            vector<int> PC_Move = PC.getNextMove(RECURSION_DEPTH);
            if (DEBUG) cout << "\n*pcMakeNextMove. i: " << PC_Move[1] << ", j: " << PC_Move[0];
            move(PC_Move[1], PC_Move[0]);
        } catch (exception &e) {
            if (DEBUG) cerr << endl << e.what();
            return false;
        }
        return true;
    }

    vector<int> PC_best() {
        Algorithm PC(board);
        return PC.getNextMove(RECURSION_DEPTH);
    }

    bool move(int X_Pos, int Y_Pos) {
        bool success;
        if (PC_ACTIVATED) {
            success = isPlayerTurn ? placeMove(X_Pos, Y_Pos, true) : placeMove(X_Pos, Y_Pos, false);
            if (success) isPlayerTurn = !isPlayerTurn;
            return success;
        }
        success = placeMove(X_Pos, Y_Pos, isPlayerTurn);
        if (success) isPlayerTurn = !isPlayerTurn;
        return success;
    }

    int who() const {
        return isPlayerTurn ? 1 : 2;
    }

    vector<vector<int>> getBoard() {
        return board.getBoardMatrix();
    }

    vector<vector<char>> getBoardChar() {
        return board.getBoard_char();
    }
};

#endif /*MINIMAX_CPP*/