# DLX Sudoku Solver

This is a Sudoku solver implemented in C++. It uses an efficient algorithm based on Dancing Links and Exact Cover to find solutions to Sudoku puzzles.

## Features

- Solves standard Sudoku puzzles of any size (4x4, 9x9, etc.).
- Uses an optimized algorithm to efficiently search for solutions.
- Outputs the solved Sudoku puzzle to a text file.

## Usage

1. **Compile the Code**: Compile the `main.cpp` file using a C++ compiler. For example:
   ```
   g++ main.cpp -o sudoku_solver
   ```

2. **Run the Program**: Execute the compiled program, providing the input Sudoku puzzle file as a command-line argument:
   ```
   ./sudoku_solver input_puzzle.txt
   ```
   Replace `input_puzzle.txt` with the path to your Sudoku puzzle file.

3. **Output**: The program will generate an output file named `input_puzzle_output.txt` containing the solved Sudoku puzzle. If no solution is found, it will output "No Solution!".

## Input File Format

The input file should contain the initial state of the Sudoku puzzle, where each number represents a cell in the puzzle. Use `0` to represent empty cells. Example:
```
0 0 2 0
0 1 0 0
0 0 0 4
3 0 0 0
```

## Requirements

- C++ compiler (supporting C++11 or later)
- Standard C++ libraries
