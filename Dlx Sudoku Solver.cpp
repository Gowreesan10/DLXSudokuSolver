#include <iostream>
#include <math.h>
#include <string>
#include <chrono>
#include <vector>
#include <algorithm>
#include <sstream>
#include <fstream>


using namespace std;
using namespace chrono;

#define MAX_K 1000

struct Node {
    Node* left, *right, *up, *down;
    Node* head;
    int size;
    int rowID[3];
};

int SIZE = 0;
int SIZE_SQUARED = 0;
int SIZE_SQRT = 0;

int ROW_NB = 0;
int COL_NB = 0;

struct Node* masterHead;
struct Node* solution[MAX_K];
struct Node* orig_values[MAX_K];

bool isSolved = false;
std::vector<std::vector<int>> sudokuGrid;


void initialize_values(){
    SIZE_SQUARED = SIZE * SIZE;
    SIZE_SQRT = sqrt(static_cast<double>(SIZE));
    ROW_NB = SIZE * SIZE * SIZE;
    COL_NB = 4 * SIZE * SIZE;
}

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

void MapSolutionToGrid() {
    for (int i = 0; solution[i] != nullptr; i++) {
        sudokuGrid[solution[i]->rowID[1] - 1][solution[i]->rowID[2] - 1] = solution[i]->rowID[0];
    }
}

void PrintGrid(std::vector<std::vector<int>> matrix) {
    for (int i = 0; i < SIZE; ++i) {
        for (int j = 0; j < SIZE; ++j) {
            cout << matrix[i][j] << " ";
        }
        cout << endl;
    }
}

void search(int k) {
    if (masterHead->right == masterHead) {
        isSolved = true;
		MapSolutionToGrid();
		return;
    }

    //Choose the column (constraint) with the minimum remaining values (MRV)
    Node* Col = masterHead->right;
    for (Node* temp = Col->right; temp != masterHead; temp = temp->right)
        if (temp->size < Col->size)
            Col = temp;

    //COVER AND UNLINK NODES AND COL
    coverColumn(Col);

    //ITERARE PREVIOUSLY UNLINKED DOWN NODE
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

void CreateSparseMatrix(bool** matrix) {
    // Part 1: Cell Constraint
    for (int i = 0; i < ROW_NB; i++) {
        matrix[i][i/SIZE] = 1; 
    }

    // Part 2: Row Constraint
    int j = 0, x = 0, counter = 1;
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

    // Part 3: Column Constraint
    j = 2 * SIZE_SQUARED;
    for (int i = 0; i < ROW_NB; i++) {
        matrix[i][j] = 1;  
        j++;
        if (j >= 3 * SIZE_SQUARED)
            j = 2 * SIZE_SQUARED;
    }

    // Part 4: Box Constraint
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

void BuildtoroidalLinkedList(bool** matrix) {
    //Initializing the Header Node
    Node* header = new Node;
    header->left = header;
    header->right = header;
    header->down = header;
    header->up = header;
    
    header->size = -1;
    header->head = header;

    Node* temp = header;
    //Creating Column Headers
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

    
    //Constructing Nodes for Matrix Elements
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

    masterHead = header;
}

//locate the corresponding nodes in the doubly linked list and cover the associated rows to represent initial state of puzzle.
void InitializeSudokuInTorodialLinkedlist(const std::vector<std::vector<int>>& Puzzle) {

    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            if (Puzzle[i][j] > 0) {
                Node* Col = nullptr;
                Node* temp = nullptr;

                for (Col = masterHead->right; Col != masterHead; Col = Col->right) {
                    for (temp = Col->down; temp != Col; temp = temp->down)
                        if (temp->rowID[0] == Puzzle[i][j] && (temp->rowID[1] - 1) == i && (temp->rowID[2] - 1) == j)
                            goto ExitLoops;
                }

    ExitLoops:   coverColumn(Col);
                for (Node* node = temp->right; node != temp; node = node->right) {
                    coverColumn(node->head);
                }
            }
}

bool** createMatrix(int rows, int cols) {
    bool** matrix = new bool*[rows];
    for (int i = 0; i < rows; ++i) {
        matrix[i] = new bool[cols];
        for (int j = 0; j < cols; ++j) {
            matrix[i][j] = false;  
        }
    }
    return matrix;
}

void deleteMatrix(bool** matrix, int rows) {
    for (int i = 0; i < rows; ++i) {
        delete[] matrix[i];
    }
    delete[] matrix;
}

void save_results(string filename){
    std::ofstream outputFile(filename.substr(0, filename.find_last_of('.'))+"_output.txt");
        
    if (!outputFile.is_open()) {
        std::cerr << "Error opening output.txt for writing." << std::endl;
        return; 
    }

    if (isSolved){
        for (const auto& row : sudokuGrid) {
            for (int num : row) {
                outputFile << num << " ";
            }
            outputFile << "\n"; 
        }
    }else{
        outputFile << "No Solution!" << endl;
    }

    outputFile.close();
}

void read_file(std::string filename){
    
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Unable to open the file." << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::vector<int> row;
        std::istringstream iss(line);
        int num;

        while (iss >> num) {
            row.push_back(num);
        }
        SIZE +=1;
        sudokuGrid.push_back(row);
    }
}



int main(int argc, char *argv[]) {

    read_file(argv[1]);
    
    auto start = high_resolution_clock::now();
    initialize_values();

    bool** matrix = createMatrix(ROW_NB, COL_NB);

    CreateSparseMatrix(matrix);
    BuildtoroidalLinkedList(matrix);
    InitializeSudokuInTorodialLinkedlist(sudokuGrid);
    search(0);
    
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);
    cout << "Time taken: " << duration.count() << " millisecond" << endl;

    save_results(argv[1]);
    PrintGrid(sudokuGrid);
    deleteMatrix(matrix,ROW_NB);

    return 0;
}