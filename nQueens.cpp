#include <iostream>
#include <cstdlib>
#include<time.h>
#include<cmath>
#include <vector>
#include <chrono>
#include <thread>

using namespace std::chrono;

using namespace std;
 
class Board {
    private:
        int size;
        int * queenPos;
        int * rowConflicts;
        int * mainDiagonalConflicts;
        int * diagonal2Conflicts;


        void initializeBoardWithRandomShuffle() {
            // At first we put the queens in the same row
            for (int i = 0; i < size; i++) {
                queenPos[i] = i;
            }
            // Then we use random shuffle in order to put the queens in a
            // new tile
            srand ( time(NULL) );
            for (int i = 0; i < size; i++) {
                int pos = rand() % size;
                std::swap(queenPos[i], queenPos[pos]);
            }
        }

        void calculateConflicts() {
            for (int i = 0; i < size*2-1; ++i) {
                diagonal2Conflicts[i] = 0;
                rowConflicts[i] = 0;
                mainDiagonalConflicts[i] = 0;
            }
            for(int i = 0; i < size; i++) {
                ++rowConflicts[queenPos[i]];
                ++mainDiagonalConflicts[i - queenPos[i] + size - 1];
                ++diagonal2Conflicts[i+queenPos[i]];
            }
        }

        void updateConflicts(int col, int oldRow, int newRow) {
            --rowConflicts[oldRow];
            ++rowConflicts[newRow];

            --mainDiagonalConflicts[col - oldRow + size - 1];
            --diagonal2Conflicts[oldRow + col];

            ++mainDiagonalConflicts[col - newRow + size - 1];
            ++diagonal2Conflicts[newRow + col];

        }

        int getQueenWithMaximumConflict() {
            std::vector<int>nominees;
            int maxConflict = 0;
            for(int i = 0; i < size; i++) {
                int currentQueenConflicts = rowConflicts[queenPos[i]] +
                                                        mainDiagonalConflicts[size - 1 + i - queenPos[i]] +
                                                        diagonal2Conflicts[i + queenPos[i]];

                // if the current queen has max conflicts we add her to the candidates list
                if (maxConflict == currentQueenConflicts) {
                    nominees.push_back(i);
                }
                // We have found a queen with more conflicts than the previous ones.
                // We delete the previous candidates and set the new maxConflict
                else if(maxConflict < currentQueenConflicts) {
                    maxConflict = currentQueenConflicts;
                    nominees.clear();
                    nominees.push_back(i);

                }
            }
            // If there is more than one queen with maximum conflicts
            // we select the queen randomly
            int randomQueen = rand() % nominees.size();
            return nominees[randomQueen];
        }

        // similar to qetQueenWithMaximumConflict but for rows
        int getRowWithMinimumConflict(int col) {
            int minConflicts = size;
            std::vector<int>nominees;
            for(int i = 0; i < size; i++) {
                int currentRowConflicts = rowConflicts[i] + mainDiagonalConflicts[size - 1 + col - i] +
                                                            diagonal2Conflicts[col + i];
                if (minConflicts == currentRowConflicts) {
                    nominees.push_back(i);
                }
                else if(currentRowConflicts < minConflicts) {
                    minConflicts = currentRowConflicts;
                    nominees.clear();
                    nominees.push_back(i);

                }
            }

            int randomRow = rand() % nominees.size();
            return nominees[randomRow];
        }

        bool hasConflicts() {
            for(int i = 0; i < size; i++) {
                if(getConflictsOfQueen(i)>0)
                    return true;
            }
            return false;
        }

        bool isSolutionFound() {
            const int MAX_RETRIES = 3;
            this->initializeBoardWithRandomShuffle();
            this->calculateConflicts();
            int tries = 0;
            while(tries++ <= MAX_RETRIES*size) {
                if(!hasConflicts()) {
                    return true;
                }
                int removeQueenFromColumn = getQueenWithMaximumConflict();
                int rowWithMinConflict = getRowWithMinimumConflict(removeQueenFromColumn);
                int removeQueenFromRow = queenPos[removeQueenFromColumn];
                if(rowWithMinConflict != removeQueenFromRow) {
                    queenPos[removeQueenFromColumn] = rowWithMinConflict;
                    updateConflicts(removeQueenFromColumn, removeQueenFromRow, rowWithMinConflict);
                }
            }
            return false;
        }

    public:
        Board(int size) : size(size) {
            this->queenPos = new int[this->size];
            this->rowConflicts = new int[this->size];
            this->diagonal2Conflicts = new int[this->size * 2 - 1];
            this->mainDiagonalConflicts = new int[this->size * 2 - 1];
            this->initializeBoardWithRandomShuffle();
            this->calculateConflicts();
        }

        void solve() {
            if(isSolutionFound()) {
                if(size <= 10) {
                    for(int i = 0; i < size; i++) {
                        for(int j = 0; j < size; j++) {
                            if(queenPos[j] == i)
                                printf("Q ");
                            else
                                printf("_ ");
                        }
                        printf("\n");
                    }
                }
                else {
                    printf("solution found\n");
                }
            }
            else solve();
        }

        int getConflictsOfQueen(int col) {
            int row = queenPos[col];
            return rowConflicts[row] +
                   mainDiagonalConflicts[size - 1 + col - row] +
                   diagonal2Conflicts[col + row] - 3;
        }
};

int main() {
    int n;
    printf ("Enter number of queens: ");
    scanf ("%d",&n);
   Board board = Board(n);
    auto started = std::chrono::high_resolution_clock::now();
    board.solve();
    auto done = std::chrono::high_resolution_clock::now();
    printf("Execution time:\n");
    printf("%lf%s", std::chrono::duration_cast<std::chrono::microseconds>(done-started).count() * 0.000001, " seconds") ;

    return 0;
}