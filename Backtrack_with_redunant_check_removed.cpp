#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>

using namespace std;
using namespace chrono;

const int N = 9;

//avoids redundant checks for each number, making the solver more 
vector<vector<int>> rowUsed(N, vector<int>(N + 1, 0));
vector<vector<int>> colUsed(N, vector<int>(N + 1, 0));
vector<vector<vector<int>>> subgridUsed(N / 3, vector<vector<int>>(N / 3, vector<int>(N + 1, 0)));

bool isSafe(int row, int col, int num) {
    return !rowUsed[row][num] && !colUsed[col][num] && !subgridUsed[row / 3][col / 3][num];
}

void updateUsed(int row, int col, int num, bool used) {
    rowUsed[row][num] = used;
    colUsed[col][num] = used;
    subgridUsed[row / 3][col / 3][num] = used;
}

bool solveSudoku(vector<vector<int>> &grid) {
    for (int row = 0; row < N; row++) {
        for (int col = 0; col < N; col++) {
            if (grid[row][col] == 0) {
                for (int num = 1; num <= 9; num++) {
                    if (isSafe(row, col, num)) {
                        grid[row][col] = num;
                        updateUsed(row, col, num, true);

                        if (solveSudoku(grid)) {
                            return true;
                        }

                        grid[row][col] = 0;
                        updateUsed(row, col, num, false);
                    }
                }
                return false;
            }
        }
    }
    return true;
}

void printGrid(const vector<vector<int>> &grid) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            cout << grid[i][j] << " ";
        }
        cout << endl;
    }
}

bool compareWithSolution(const vector<vector<int>> &output, const vector<vector<int>> &solution) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (output[i][j] != solution[i][j]) {
                return false;
            }
        }
    }
    return true;
}

int main() {
    ifstream inputFile("input.txt");
    vector<vector<int>> sudokuGrid(N, vector<int>(N, 0));

    if (inputFile.is_open()) {
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                inputFile >> sudokuGrid[i][j];
                if (sudokuGrid[i][j] != 0) {
                    updateUsed(i, j, sudokuGrid[i][j], true);
                }
            }
        }
        inputFile.close();
    } else {
        cout << "Unable to open the input file." << endl;
        return 1;
    }

    auto start = high_resolution_clock::now();

    if (solveSudoku(sudokuGrid)) {
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);

        cout << "Solved Sudoku:" << endl;
        printGrid(sudokuGrid);

        ofstream outputFile("output.txt");
        if (outputFile.is_open()) {
            for (int i = 0; i < N; i++) {
                for (int j = 0; j < N; j++) {
                    outputFile << sudokuGrid[i][j] << " ";
                }
                outputFile << endl;
            }
            outputFile.close();
        } else {
            cout << "Unable to open the output file." << endl;
            return 1;
        }

        ifstream solutionFile("solution.txt");
        vector<vector<int>> solutionGrid(N, vector<int>(N, 0));

        if (solutionFile.is_open()) {
            for (int i = 0; i < N; i++) {
                for (int j = 0; j < N; j++) {
                    solutionFile >> solutionGrid[i][j];
                }
            }
            solutionFile.close();
        } else {
            cout << "Unable to open the solution file." << endl;
            return 1;
        }

        if (compareWithSolution(sudokuGrid, solutionGrid)) {
            cout << "Solution is correct!" << endl;
        } else {
            cout << "Solution is incorrect." << endl;
        }

        cout << "Time taken: " << duration.count() << " microseconds" << endl;
    } else {
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(stop - start);

        cout << "No solution exists." << endl;
        cout << "Time taken: " << duration.count() << " milliseconds" << endl;
    }

    return 0;
}
