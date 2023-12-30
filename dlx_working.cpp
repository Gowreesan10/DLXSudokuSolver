#include <iostream>
#include <cmath>
#include <string>
#include <chrono>

using namespace std;
using namespace chrono;

#define MAX_K 1000
#define SIZE 9

struct Node {
    Node* left, *right, *up, *down;
    Node* head;
    int size;
    int rowID[3];
};

const int SIZE_SQUARED = SIZE * SIZE;
const int SIZE_SQRT = sqrt(static_cast<double>(SIZE));
const int ROW_NB = SIZE * SIZE * SIZE;
const int COL_NB = 4 * SIZE * SIZE;

struct Node Head;
struct Node* HeadNode = &Head;
struct Node* solution[MAX_K];
struct Node* orig_values[MAX_K];
bool matrix[ROW_NB][COL_NB] = { { 0 } };
bool isSolved = false;

void coverColumn(Node* col) {
    col->left->right = col->right;
    col->right->left = col->left;
    for (Node* node = col->down; node != col; node = node->down) {
        for (Node* temp = node->right; temp != node; temp = temp->right) {
            temp->down->up = temp->up;
            temp->up->down = temp->down;
            temp->head->size--;
        }
    }
}

void uncoverColumn(Node* col) {
    for (Node* node = col->up; node != col; node = node->up) {
        for (Node* temp = node->left; temp != node; temp = temp->left) {
            temp->head->size++;
            temp->down->up = temp;
            temp->up->down = temp;
        }
    }
    col->left->right = col;
    col->right->left = col;
}

void search(int k) {
    if (HeadNode->right == HeadNode) {
        isSolved = true;
        return;
    }

    Node* Col = HeadNode->right;
    for (Node* temp = Col->right; temp != HeadNode; temp = temp->right)
        if (temp->size < Col->size)
            Col = temp;

    coverColumn(Col);

    for (Node* temp = Col->down; temp != Col; temp = temp->down) {
        solution[k] = temp;
        for (Node* node = temp->right; node != temp; node = node->right) {
            coverColumn(node->head);
        }

        search(k + 1);

        temp = solution[k];
        solution[k] = nullptr;
        Col = temp->head;
        for (Node* node = temp->left; node != temp; node = node->left) {
            uncoverColumn(node->head);
        }
    }

    uncoverColumn(Col);
}

void BuildSparseMatrix(bool matrix[ROW_NB][COL_NB]) {
    int j = 0, counter = 0;
    for (int i = 0; i < ROW_NB; i++) {
        matrix[i][j] = 1;
        counter++;
        if (counter >= SIZE) {
            j++;
            counter = 0;
        }
    }

    int x = 0;
    counter = 1;
    for (j = SIZE_SQUARED; j < 2 * SIZE_SQUARED; j++) {
        for (int i = x; i < counter * SIZE_SQUARED; i += SIZE)
            matrix[i][j] = 1;

        if ((j + 1) % SIZE == 0) {
            x = counter * SIZE_SQUARED;
            counter++;
        }
        else
            x++;
    }

    j = 2 * SIZE_SQUARED;
    for (int i = 0; i < ROW_NB; i++) {
        matrix[i][j] = 1;
        j++;
        if (j >= 3 * SIZE_SQUARED)
            j = 2 * SIZE_SQUARED;
    }

    x = 0;
    for (j = 3 * SIZE_SQUARED; j < COL_NB; j++) {
        for (int l = 0; l < SIZE_SQRT; l++) {
            for (int k = 0; k < SIZE_SQRT; k++)
                matrix[x + l * SIZE + k * SIZE_SQUARED][j] = 1;
        }

        int temp = j + 1 - 3 * SIZE_SQUARED;

        if (temp % (SIZE_SQRT * SIZE) == 0)
            x += (SIZE_SQRT - 1) * SIZE_SQUARED + (SIZE_SQRT - 1) * SIZE + 1;
        else if (temp % SIZE == 0)
            x += SIZE * (SIZE_SQRT - 1) + 1;
        else
            x++;
    }
}

void BuildLinkedList(bool matrix[ROW_NB][COL_NB]) {
    Node* header = new Node;
    header->left = header;
    header->right = header;
    header->down = header;
    header->up = header;
    header->size = -1;
    header->head = header;
    Node* temp = header;

    for (int i = 0; i < COL_NB; i++) {
        Node* newNode = new Node;
        newNode->size = 0;
        newNode->up = newNode;
        newNode->down = newNode;
        newNode->head = newNode;
        newNode->right = header;
        newNode->left = temp;
        temp->right = newNode;
        temp = newNode;
    }

    int ID[3] = { 0,1,1 };
    for (int i = 0; i < ROW_NB; i++) {
        Node* top = header->right;
        Node* prev = nullptr;

        if (i != 0 && i % SIZE_SQUARED == 0) {
            ID[0] -= SIZE - 1;
            ID[1]++;
            ID[2] -= SIZE - 1;
        }
        else if (i != 0 && i % SIZE == 0) {
            ID[0] -= SIZE - 1;
            ID[2]++;
        }
        else {
            ID[0]++;
        }

        for (int j = 0; j < COL_NB; j++, top = top->right) {
            if (matrix[i][j]) {
                Node* newNode = new Node;
                newNode->rowID[0] = ID[0];
                newNode->rowID[1] = ID[1];
                newNode->rowID[2] = ID[2];
                if (prev == nullptr) {
                    prev = newNode;
                    prev->right = newNode;
                }
                newNode->left = prev;
                newNode->right = prev->right;
                newNode->right->left = newNode;
                prev->right = newNode;
                newNode->head = top;
                newNode->down = top;
                newNode->up = top->up;
                top->up->down = newNode;
                top->size++;
                top->up = newNode;
                if (top->down == top)
                    top->down = newNode;
                prev = newNode;
            }
        }
    }

    HeadNode = header;
}

void TransformListToCurrentGrid(int Puzzle[][SIZE]) {
    int index = 0;
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            if (Puzzle[i][j] > 0) {
                Node* Col = nullptr;
                Node* temp = nullptr;
                for (Col = HeadNode->right; Col != HeadNode; Col = Col->right) {
                    for (temp = Col->down; temp != Col; temp = temp->down)
                        if (temp->rowID[0] == Puzzle[i][j] && (temp->rowID[1] - 1) == i && (temp->rowID[2] - 1) == j)
                            goto ExitLoops;
                }
    ExitLoops:   coverColumn(Col);
                orig_values[index] = temp;
                index++;
                for (Node* node = temp->right; node != temp; node = node->right) {
                    coverColumn(node->head);
                }
            }
}

void MapSolutionToGrid(int Sudoku[][SIZE]) {
    for (int i = 0; solution[i] != nullptr; i++) {
        Sudoku[solution[i]->rowID[1] - 1][solution[i]->rowID[2] - 1] = solution[i]->rowID[0];
    }
    for (int i = 0; orig_values[i] != nullptr; i++) {
        Sudoku[orig_values[i]->rowID[1] - 1][orig_values[i]->rowID[2] - 1] = orig_values[i]->rowID[0];
    }
}

void PrintGrid(int Sudoku[][SIZE]) {
    string ext_border = "+", int_border = "|";
    int counter = 1;
    int additional = 0;
    if (SIZE > 16)
        additional = SIZE;
    for (int i = 0; i < ((SIZE + SIZE_SQRT - 1) * 2 + additional + 1); i++) {
        ext_border += '-';

        if (i > 0 && i % ((SIZE_SQRT * 2 + SIZE_SQRT * (SIZE > 9) + 1) * counter + counter - 1) == 0) {
            int_border += '+';
            counter++;
        }
        else
            int_border += '-';
    }
    ext_border += '+';
    int_border += "|";

    cout << ext_border << endl;
    for (int i = 0; i < SIZE; i++) {
        cout << "| ";
        for (int j = 0; j < SIZE; j++) {
            if (Sudoku[i][j] == 0)
                cout << ". ";
            else
                cout << Sudoku[i][j] << " ";
            if (additional > 0 && Sudoku[i][j] < 10)
                cout << " ";
            if ((j + 1) % SIZE_SQRT == 0)
                cout << "| ";
        }
        cout << endl;
        if ((i + 1) % SIZE_SQRT == 0 && (i + 1) < SIZE)
            cout << int_border << endl;
    }
    cout << ext_border << endl << endl;
}

void SolveSudoku(int Sudoku[][SIZE]) {
    BuildSparseMatrix(matrix);
    BuildLinkedList(matrix);
    TransformListToCurrentGrid(Sudoku);
    search(0);
    if (!isSolved)
        cout << "No Solution!" << endl;
    isSolved = false;
}

int main() {
    auto start = high_resolution_clock::now();

// 	int sudokuGrid[SIZE][SIZE] = {
//     {0, 15, 1, 0, 4, 16, 12, 0, 0, 5, 0, 9, 7, 0, 0, 8},
//     {0, 0, 0, 0, 11, 0, 15, 3, 0, 0, 13, 0, 0, 0, 16, 0},
//     {0, 0, 7, 3, 0, 0, 10, 6, 11, 0, 14, 0, 0, 0, 0, 9},
//     {0, 0, 14, 0, 0, 0, 7, 0, 16, 4, 0, 0, 0, 0, 0, 13},
//     {0, 0, 16, 0, 8, 0, 13, 0, 1, 0, 6, 0, 0, 15, 5, 11},
//     {14, 11, 12, 8, 0, 0, 0, 15, 2, 13, 4, 0, 16, 0, 0, 7},
//     {15, 13, 4, 0, 14, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 3},
//     {0, 9, 0, 0, 0, 0, 11, 0, 0, 0, 0, 14, 0, 0, 6, 12},
//     {0, 0, 0, 0, 2, 6, 0, 5, 10, 16, 12, 8, 9, 0, 0, 15},
//     {0, 0, 0, 0, 15, 0, 0, 0, 0, 14, 0, 3, 0, 0, 0, 0},
//     {8, 0, 11, 0, 0, 0, 16, 0, 13, 15, 0, 0, 0, 0, 0, 10},
//     {0, 0, 0, 7, 0, 13, 0, 10, 9, 0, 0, 0, 0, 16, 12, 0},
//     {16, 7, 0, 13, 6, 12, 0, 0, 0, 10, 0, 0, 0, 0, 9, 0},
//     {12, 0, 5, 15, 0, 0, 14, 16, 8, 0, 11, 0, 0, 0, 0, 0},
//     {0, 2, 3, 0, 0, 9, 0, 0, 12, 0, 0, 1, 0, 0, 10, 0},
//     {0, 0, 0, 10, 0, 0, 0, 11, 4, 0, 0, 6, 0, 13, 8, 0}
// };

int sudokuGrid[SIZE][SIZE] = {
    {0, 2, 0, 0, 0, 7, 5, 0, 8},
    {0, 0, 7, 0, 0, 0, 3, 0, 0},
    {1, 0, 0, 0, 0, 2, 0, 6, 0},
    {0, 0, 5, 0, 0, 0, 4, 0, 0},
    {0, 3, 0, 8, 1, 6, 0, 0, 0},
    {0, 0, 0, 4, 0, 0, 0, 8, 0},
    {5, 0, 0, 0, 6, 1, 8, 0, 0},
    {0, 0, 3, 0, 4, 0, 6, 0, 0},
    {9, 0, 0, 5, 7, 0, 0, 4, 0}
};

    int EmptyPuzzle[SIZE][SIZE] = { {0} };

    SolveSudoku(sudokuGrid);

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "Time taken: " << duration.count() << " microsec" << endl;

    // Free dynamically allocated memory
    for (int i = 0; i < ROW_NB; i++) {
        for (Node* node = HeadNode->right; node != HeadNode; node = node->right) {
            if (matrix[i][node->rowID[1]])
                delete node;
        }
    }

    return 0;
}