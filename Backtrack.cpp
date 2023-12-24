#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>

using namespace std;
using namespace chrono;

const int N = 9;

bool isSafe(vector<vector<int>> &grid, int row, int col, int num) {
    for (int x = 0; x < N; x++) {
        if (grid[row][x] == num || grid[x][col] == num || grid[row - row % 3 + x / 3][col - col % 3 + x % 3] == num) {
            return false;
        }
    }
    return true;
}

bool solveSudoku(vector<vector<int>> &grid) {
    for (int row = 0; row < N; row++) {
        for (int col = 0; col < N; col++) {
            if (grid[row][col] == 0) {
                for (int num = 1; num <= 9; num++) {
                    if (isSafe(grid, row, col, num)) {
                        grid[row][col] = num;

                        if (solveSudoku(grid)) {
                            return true;
                        }

                        grid[row][col] = 0;
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

        cout << "Time taken: " << duration.count() << " microseconds" << endl;

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


    } else {
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(stop - start);

        cout << "No solution exists." << endl;
        cout << "Time taken: " << duration.count() << " milliseconds" << endl;
    }

    return 0;
}
