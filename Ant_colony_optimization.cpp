#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <fstream>
#include <chrono>

using namespace std;
using namespace chrono;

const int N = 9;
const int UNASSIGNED = 0;

struct Ant {
    vector<vector<int>> grid;
    int fitness;
};

bool isSafe(const vector<vector<int>>& grid, int row, int col, int num) {
    for (int x = 0; x < N; x++)
        if (grid[row][x] == num || grid[x][col] == num)
            return false;

    int startRow = row - row % 3, startCol = col - col % 3;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (grid[i + startRow][j + startCol] == num)
                return false;

    return true;
}

Ant solveSudoku(Ant ant) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (ant.grid[i][j] == UNASSIGNED) {
                vector<int> numbers {1, 2, 3, 4, 5, 6, 7, 8, 9};
                random_device rd;
                mt19937 g(rd());
                shuffle(numbers.begin(), numbers.end(), g);

                for (int num : numbers) {
                    if (isSafe(ant.grid, i, j, num)) {
                        ant.grid[i][j] = num;
                        ant = solveSudoku(ant);
                        if (ant.fitness == 0) {
                            return ant;
                        }
                        ant.grid[i][j] = UNASSIGNED;
                    }
                }
                return ant;
            }
        }
    }
    ant.fitness = 0;
    return ant;
}

int main() {
    Ant ant;
    ant.grid = vector<vector<int>>(N, vector<int>(N, UNASSIGNED));
    ant.fitness = 1;

    ifstream input("input.txt");
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            input >> ant.grid[i][j];

    auto start = high_resolution_clock::now();
    ant = solveSudoku(ant);
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);

    cout << "Time taken: " << duration.count() << " microseconds" << endl;

    if (ant.fitness == 0) {
        ofstream output("solved_sudoku.txt");
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++)
                output << ant.grid[i][j] << " ";
            output << "\n";
        }
    } else {
        cout << "No solution exists" << endl;
    }
    return 0;
}
