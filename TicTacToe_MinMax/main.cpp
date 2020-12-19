#include <iostream>
#include <algorithm>
#include <vector>

using namespace std;

#define s32INF (0xFFFFFFF)


class Board {
    char board[3][3];
public:
    Board() {
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                board[i][j] = ' ';
            }
        }
    }

    Board(char board[][3]) {
        for(int i = 0; i < 3; i++) {
            for(int j = 0; j < 3; j++) {
                this->board[i][j] = board[i][j];
            }
        }
    }

    bool hasNoEmptyCells() const {
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (isEmpty(i, j)) {
                    return false;
                }
            }
        }

        return true;
    }

    bool isEmpty(int i, int j) const {
        return board[i][j] == ' ';
    }

    char* operator[](int i) {
        return board[i];
    }

    string getRow(int row) const {
        string value = string() + board[row][0] + board[row][1] + board[row][2];
        return value;
    }

    string getCol(int col) const {
        string value = string() + board[0][col] + board[1][col] + board[2][col];
        return value;
    }

    string getMainDiagonal() const {
        string value = string() + board[0][0] + board[1][1] + board[2][2];
        return value;
    }

    string getSecondaryDiagonal() const {
        string value = string() + board[0][2] + board[1][1] + board[2][0];
        return value;
    }

    void print() const {
        for(int i = 0; i < 3; i++) {
            std::cout << " | ";
            for(int j = 0; j < 3; j++) {
                std::cout << board[i][j] << " | ";
            }
            std::cout << std::endl;
        }
        printf("\n\n");
    }

    vector<Board> getChildren(char player) {
        vector<Board> children;

        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (!isEmpty(i, j)) {
                    continue;
                }
                Board nextState = Board(this->board);
                nextState[i][j] = player;
                children.push_back(nextState);
            }
        }

        return children;
    }
};

class TicTacToe {
private:
    std::string WIN_MAX = "XXX";
    std::string WIN_MIN = "OOO";
    char MAX_MOVE = 'X';
    char MIN_MOVE = 'O';

    bool isTerminalState(const Board &board) {

        if (board.hasNoEmptyCells()) {
            return true;
        }

        string mainDiagonal = board.getMainDiagonal();
        string secondaryDiagonal = board.getSecondaryDiagonal();

        if (mainDiagonal == WIN_MAX || secondaryDiagonal == WIN_MAX ||
            mainDiagonal == WIN_MIN || secondaryDiagonal == WIN_MIN) {
            return true;
        }

        for (int i = 0; i < 3; i++) {
            string row = board.getRow(i);
            string col = board.getCol(i);
            if (row == WIN_MAX || col == WIN_MAX ||
                row == WIN_MIN || col == WIN_MIN) {
                return true;
            }
        }

        return false;
    }

    int getTerminalStateValue(const Board& board) {

        string mainDiagonal = board.getMainDiagonal();
        string secondaryDiagonal = board.getSecondaryDiagonal();

        if (mainDiagonal == WIN_MAX || secondaryDiagonal == WIN_MAX) {
            return 1;
        }

        if (mainDiagonal == WIN_MIN || secondaryDiagonal == WIN_MIN) {
            return -1;
        }

        for (int i = 0; i < 3; i++) {
            string row = board.getRow(i);
            string col = board.getCol(i);
            if (row == WIN_MAX || col == WIN_MAX) {
                return 1;
            }
            if (row == WIN_MIN ||
                col == WIN_MIN) {
                return -1;
            }
        }

        return 0;
    }

    int MaxValue(Board board, int alpha, int beta, int &depth) {
        depth++;
        if (isTerminalState(board)) {
            return getTerminalStateValue(board);
        }

        int best = -s32INF;
        vector<Board> children = board.getChildren(MAX_MOVE);
        for (auto child : children) {
            best = std::max(best, MinValue(child, alpha, beta, depth));

            if (best >= beta) {
                return best;
            }

            alpha = std::max(alpha, best);
        }

        return best;
    }

    int MinValue(Board board, int alpha, int beta, int &depth) {
        depth++;
        if (isTerminalState(board)) {
            return getTerminalStateValue(board);
        }

        int best = s32INF;
        vector<Board> children = board.getChildren(MIN_MOVE);

        for (auto child : children) {
            best = std::min(best, MaxValue(child, alpha, beta, depth));

            if (best <= alpha) {
                return best;
            }

            beta = std::min(beta, best);
        }

        return best;
    }

    Board findBestMove(Board board) {
        int maxValue = -1;
        int bestDepth = s32INF;
        Board bestBoard = board;

        vector<Board> children = board.getChildren(MAX_MOVE);

        for (auto child : children) {
            int depth = 0;
            int value = MinValue(child, -s32INF, s32INF, depth);

            if (value > maxValue) {
                maxValue = value;
                bestBoard = child;
                bestDepth = depth;
            }
            else if (value == maxValue && depth < bestDepth) {
                bestDepth = depth;
                bestBoard = child;
            }
        }
        return bestBoard;
    }
    enum {HUMAN, AI} firstPlayer;
public:
    void choosePlayer() {
        cout << "Be the first player? [y/n]: ";
        char c;
        cin >> c;
        firstPlayer = (c == 'y') ? HUMAN : AI;
    }
    void start() {
        Board board;
        if (firstPlayer == HUMAN) {
            board.print();
            int i,j;
            std::cin >> i >> j;
            board[i][j] = MIN_MOVE;
            board.print();
        }

        while (!isTerminalState(board)) {
            board = findBestMove(board);
            board.print();
            if (isTerminalState(board)) {
                break;
            }
            int i,j;
            std::cin >> i >> j;
            board[i][j] = MIN_MOVE;
            board.print();
        }

    }
};

int main() {

    TicTacToe game;
    game.choosePlayer();
    game.start();

    return 0;
}