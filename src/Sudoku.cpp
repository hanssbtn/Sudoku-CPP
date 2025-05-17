#include <iostream>
#include <vector>
#include <random>
#include <cstdlib>
#include <stdexcept>
#include <algorithm>
#include <chrono>
#include <array>
#include <unordered_map>
#include "Sudoku.hpp"

// trim from start (in place)
inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

Sudoku::Grid::Grid(Difficulty lvl) {            
    // Allocate memory for sudoku elements.  
    this->sol.reserve(81);
    this->sol.resize(81, 0);    
    this->gridHT.rehash(243);
	this->filledIndexes.reserve(81);
	this->filledIndexes.resize(81, true);
    
    this->numberOfSolutions = 0;

    // Generate diagonal boxes for less collision checking.
    {
        std::vector<int> numbers = {0, 1, 2, 3, 4, 5, 6, 7, 8};
        int r, c;
        srand(this->seed);
        this->seed = std::chrono::system_clock::now().time_since_epoch().count();
        unsigned char ch;
        for (r = 0; r < 3; r++){
            for (c = 0; c < 3; c++){
                int index = rand() % numbers.size();
                ch = numbers.at(index);
                this->sol.at(r * 9 + c) = ch + '1';
                this->gridHT[r * 9 + ch] = true;
                this->gridHT[c * 9 + 81 + ch] = true;
                this->gridHT[ch + 162] = true;
                numbers.erase(numbers.begin() + index);
            }
        }

        numbers = {0, 1, 2, 3, 4, 5, 6, 7, 8};
        for (; r < 6; r++){
            for (c = 3; c < 6; c++){
                int index = rand() % numbers.size();
                ch = numbers.at(index);
                this->sol.at(r * 9 + c) = ch + '1';
                this->gridHT[r * 9 + ch] = true;
                this->gridHT[c * 9 + 81 + ch] = true;
                this->gridHT[ch + 36 + 162] = true;
                numbers.erase(numbers.begin() + index);
            }
        }

        numbers = {0, 1, 2, 3, 4, 5, 6, 7, 8};
        for (; r <9; r++){
            for (c = 6; c < 9; c++){
                int index = rand() % numbers.size();
                ch = numbers.at(index);
                this->sol.at(r * 9 + c) = ch + '1';
                this->gridHT[r * 9 + ch] = true;
                this->gridHT[c * 9 + 81 + ch] = true;
                this->gridHT[ch + 72 + 162] = true;
                numbers.erase(numbers.begin() + index);
            }
        }
        // Generate rest of solved grid.
        std::vector<int> remainingIndexes = {
            3,  4,  5,  6,  7,  8,
            12, 13, 14, 15, 16, 17, 
            21, 22, 23, 24, 25, 26,
            27, 28, 29, 33, 34, 35,
            36, 37, 38, 42, 43, 44,
            45, 46, 47, 51, 52, 53,
            54, 55, 56, 57, 58, 59,
            63, 64, 65, 66, 67, 68,
            72, 73, 74, 75, 76, 77,
        };
        std::vector<int>& rem = remainingIndexes;
        std::vector<int>::const_iterator it = remainingIndexes.begin();
        genSol(rem, it);   
    }
    
    // Randomize block rows and columns, as well as rows and / or columns within block rows / columns.
    {
        int counter = 0, srcBlockRowCol, destBlockRowCol, targetRowCol, r, srcRowCol, destRowCol;
        unsigned char temp;
        while (counter < 3){
            srand(this->seed + counter);
            targetRowCol = rand() % 2;
            srcBlockRowCol = rand() % 3;
            destBlockRowCol = rand() % 3;
            if (destBlockRowCol == srcBlockRowCol) destBlockRowCol = (destBlockRowCol + 1) % 3;        
            switch (targetRowCol) {
                // Swap the block rows.
                case 0:
                    #ifdef __DBG
                        std::cout << "Switching block row " << srcBlockRowCol + 1 << " with " << destBlockRowCol + 1 << std::endl; 
                    #endif
                    destBlockRowCol *= 27;
                    srcBlockRowCol *= 27;
                    for (r = 0; r < 27; r++){
                        temp = this->sol.at(r + destBlockRowCol);
                        this->sol.at(r + destBlockRowCol) = this->sol.at(r + srcBlockRowCol);
                        this->sol.at(r + srcBlockRowCol) = temp;
                    }
                    break;
                // Swap the block columns.
                default:
                    #ifdef __DBG
                        std::cout << "Switching block column " << srcBlockRowCol + 1 << " with " << destBlockRowCol + 1<< std::endl; 
                    #endif
                    srcBlockRowCol *= 3;
                    destBlockRowCol *= 3; 
                    for (int c = 0; c < 3; c++){
                        for (r = 0; r < 81; r += 9){
                            temp = this->sol.at(r + c + destBlockRowCol);
                            this->sol.at(r + c + destBlockRowCol) = this->sol.at(r + c + srcBlockRowCol);  
                            this->sol.at(r + c + srcBlockRowCol) = temp;
                        }
                    }
                    break;
            }
            counter++;
        }
        
        // Renew the seed after use.
        this->seed = std::chrono::system_clock::now().time_since_epoch().count();
        
        // Randomize rows and columns in block.
        while (counter < 9){
            srand(this->seed + counter);
            targetRowCol = rand() % 2;
            destBlockRowCol = rand() % 3;
            destRowCol = rand() % 3;
            srcRowCol = rand() % 3;
            if (srcRowCol == destRowCol) destRowCol = (destRowCol + 1) % 3;
            switch (targetRowCol) {
                // Switch rows within selected block row.
                case 0:
                    #ifdef __DBG
                        std::cout << "Switching row " << srcRowCol + 1 << " of block row " << destBlockRowCol + 1 << " with row " << destRowCol + 1 <<std::endl;
                    #endif
                    destBlockRowCol *= 27;
                    destRowCol *= 9;
                    srcRowCol *= 9;
                    for (r = 0; r < 9; r++) {
                        temp = this->sol.at(r + destBlockRowCol + srcRowCol);
                        this->sol.at(r + destBlockRowCol + srcRowCol) = this->sol.at(r + destBlockRowCol + destRowCol);
                        this->sol.at(r + destBlockRowCol + destRowCol) = temp;
                    }
                    break;
                // Switch columns within selected block column.
                default:
                    #ifdef __DBG
                        std::cout << "Switching column " << srcRowCol + 1 << " of block column " << destBlockRowCol + 1 << " with column " << destRowCol + 1 <<std::endl;
                    #endif
                    destBlockRowCol *= 3;
                    for (r = 0; r < 81; r += 9){
                        temp = this->sol.at(r + destBlockRowCol + srcRowCol);
                        this->sol.at(r + destBlockRowCol + srcRowCol) = this->sol.at(r + destBlockRowCol + destRowCol); 
                        this->sol.at(r + destBlockRowCol + destRowCol) = temp;
                    }
                    break;
            }
            counter++;
        }

        // Renew the seed after use.
        this->seed = std::chrono::system_clock::now().time_since_epoch().count(); 
    }

    // Set the solution to the generated grid, 
    // then remove elements in a random order.
    // Clear the map for checking the solution.
    this->grid = this->sol;

    // Remove n elements based on difficulty.
    // If invalid, raise error and exit.
    {
        try { 
            std::vector<int> indexes = {
                0,  1,  2,  3,  4,  5,  6,  7,  8,
                9,  10, 11, 12, 13, 14, 15, 16, 17,
                18, 19, 20, 21, 22, 23, 24, 25, 26,
                27, 28, 29, 30, 31, 32, 33, 34, 35,
                36, 37, 38, 39, 40, 41, 42, 43, 44,
                45, 46, 47, 48, 49, 50, 51, 52, 53,
                54, 55, 56, 57, 58, 59, 60, 61, 62,
                63, 64, 65, 66, 67, 68, 69, 70, 71,
                72, 73, 74, 75, 76, 77, 78, 79, 80
            };

            srand(this->seed);
            this->seed = std::chrono::system_clock::now().time_since_epoch().count();
            int elementsToRemove;
            switch (lvl) {
                case EASY:
                    elementsToRemove = 22;
                    break;
                case MEDIUM:
                    elementsToRemove = 27;
                    break;
                case HARD:
                    elementsToRemove = 37;
                    break;
                default:
                    throw std::runtime_error("Invalid difficulty level.");
                    break;
            }
            std::vector<int> removedIndexes;
            int counter = 0;
            while (counter < 3){
                int randomIndex = rand() % indexes.size();
                int removed = indexes.at(randomIndex);
                unsigned char ele = this->grid.at(removed);
                this->grid.at(removed) = '\0';
                removedIndexes.push_back(removed);
                indexes.erase(indexes.begin() + randomIndex);
                this->gridHT[ROW(removed) * 9 + (ele - '1')] = false;
                this->gridHT[COL(removed) * 9 + (ele - '1') + 81] = false;
                this->gridHT[SUB(removed) * 9 + (ele - '1') + 162] = false;
                counter++;
            }
            if (!this->isValid()) {
                throw std::runtime_error("Generated invalid grid.\n");
            }   
            removeCells(elementsToRemove, indexes, removedIndexes);
			for (size_t i = 0; i < removedIndexes.size(); i++) {
				this->filledIndexes[removedIndexes[i]] = false;	
			}
        } catch (const std::exception &e) {
            std::cerr << "Error: " << e.what() << std::endl;
            exit(-1);  
        }
    }
}

bool Sudoku::Grid::checkSol() {
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (!this->sol.at(i * 9 + j)) return false;
        }
    }
    return true;
}

bool Sudoku::Grid::setCell(int row, int col, unsigned char val) {
	if (this->filledIndexes.at(row * 9 + col)) return false;
    this->grid.at((row * 9) + col) = val;
	return true;
};

bool Sudoku::Grid::solveGrid(const std::vector<int>& emptyIndexes, std::vector<int>::const_iterator iter){
    if (iter == emptyIndexes.end() - 1){
        for (auto num = 0; num < 9; num++){
            if (!(this->gridHT[ROW(*iter) * 9 + num] || this->gridHT[COL(*iter) * 9 + num + 81] || this->gridHT[SUB(*iter) * 9 + num + 162])) {
                this->gridHT[ROW(*iter) * 9 + num] = false;
                this->gridHT[COL(*iter) * 9 + num + 81] = false;
                this->gridHT[SUB(*iter) * 9 + num + 162] = false;
                this->numberOfSolutions++;
                return this->numberOfSolutions < 2;
            }
        }
        return false;
    }
    for (auto num = 0; num < 9; num++){
        if (!(this->gridHT[ROW(*iter) * 9 + num] || this->gridHT[COL(*iter) * 9 + num + 81] || this->gridHT[SUB(*iter) * 9 + num + 162])) {
            this->grid.at(*iter) = num + '1';
            this->gridHT[ROW(*iter) * 9 + num] = true;
            this->gridHT[COL(*iter) * 9 + num + 81] = true;
            this->gridHT[SUB(*iter) * 9 + num + 162] = true;
            if (solveGrid(emptyIndexes, iter + 1)) {
                this->grid.at(*iter) = 0;
                this->gridHT[ROW(*iter) * 9 + num] = false;
                this->gridHT[COL(*iter) * 9 + num + 81] = false;
                this->gridHT[SUB(*iter) * 9 + num + 162] = false;
                return this->numberOfSolutions < 2;
            }
            this->grid.at(*iter) = 0;
            this->gridHT[ROW(*iter) * 9 + num] = false;
            this->gridHT[COL(*iter) * 9 + num + 81] = false;
            this->gridHT[SUB(*iter) * 9 + num + 162] = false;
        }
    }
    return false;
}
            
bool Sudoku::Grid::genSol(const std::vector<int>& rem, std::vector<int>::const_iterator it){
    if (it == rem.end() - 1) {
        for (int i = 0; i < 9; i++){
            if (!(this->gridHT[ROW(*it) * 9 + i] || this->gridHT[COL(*it) * 9 + i + 81] || this->gridHT[SUB(*it) * 9 + i + 162])) {
                this->sol.at(*it) = i + '1';
                this->gridHT[ROW(*it) * 9 + i] = true;
                this->gridHT[COL(*it) * 9 + i + 81] = true;
                this->gridHT[SUB(*it) * 9 + i + 162] = true;
                return true;
            }
        }
        return false;
    }

    for (int i = 0; i < 9; i++){
        if (!(this->gridHT[ROW(*it) * 9 + i] || this->gridHT[COL(*it) * 9 + i + 81] || this->gridHT[SUB(*it) * 9 + i + 162])) {
            this->sol.at(*it) = i + '1';
            this->gridHT[ROW(*it) * 9 + i] = true;
            this->gridHT[COL(*it) * 9 + i + 81] = true;
            this->gridHT[SUB(*it) * 9 + i + 162] = true;
            if (genSol(rem, it + 1)){
                return true;
            }
            this->gridHT[ROW(*it) * 9 + i] = false;
            this->gridHT[COL(*it) * 9 + i + 81] = false;
            this->gridHT[SUB(*it) * 9 + i + 162] = false;
        }
    }   
    return false;
}
            
bool Sudoku::Grid::removeCells(const int cellsToRemove, std::vector<int>& indexes, std::vector<int>& emptyIndexes){
    if (!cellsToRemove) {
        return true;
    }
    srand(std::chrono::system_clock::now().time_since_epoch().count());
    int randomIndex = rand() % indexes.size();
    int removedIndex = indexes.at(randomIndex);
    unsigned char removedElement = this->grid.at(removedIndex);
    this->grid.at(removedIndex) = '\0';                                             // Clear the cell at that grid, and empty the hash bucket at the index.
    this->gridHT[ROW(removedIndex) * 9 + (removedElement - '1')] = false;
    this->gridHT[COL(removedIndex) * 9 + (removedElement - '1') + 81] = false;
    this->gridHT[SUB(removedIndex) * 9 + (removedElement - '1') + 162] = false;     
    emptyIndexes.push_back(removedIndex);
    indexes.erase(indexes.begin() + randomIndex);                                   // Remove the index from the vector to prevent redundant checks.
    this->numberOfSolutions = 0;                                                    // Reset the number of solutions for the puzzle grid.      
    std::vector<int>::const_iterator iter = emptyIndexes.begin();                   
    solveGrid(emptyIndexes, iter);                                                  // Solve the puzzle and count the number of solutions.
    if (this->numberOfSolutions == 1 && Sudoku::Grid::removeCells(cellsToRemove - 1, indexes, emptyIndexes)) {
        return true;
    }
    this->grid.at(removedIndex) = removedElement;                                   // Reassign the removed element, and try again. 
    emptyIndexes.pop_back();                                                        // Remove the index from the vector, as the index cannot be removed without making the puzzle grid non-unique.
    this->gridHT[ROW(removedIndex) * 9 + (removedElement - '1')] = true;
    this->gridHT[COL(removedIndex) * 9 + (removedElement - '1') + 81] = true;
    this->gridHT[SUB(removedIndex) * 9 + (removedElement - '1') + 162] = true;
    return Sudoku::Grid::removeCells(cellsToRemove, indexes, emptyIndexes);         // Recursively call the function and try again.
}

std::istream& operator>>(std::istream& ist, Sudoku::Difficulty& diff){
    int tmp;
    if (ist >> tmp) {
        diff = static_cast<Sudoku::Difficulty>(tmp);
    }
    return ist;
}

unsigned int Sudoku::hashByIndex::operator()(const unsigned int& key) const noexcept {
    return key;
}

void Sudoku::Grid::displayGrid(){
    std::cout << "_______________________________________________________" << std::endl;
    for (int i = 0; i < 9; i++){
        std::cout << "|";
        for (int j = 0; j < 8; j++){
            this->grid.at(i*9 + j) ?  std::cout << "  " << this->grid.at((i * 9) + j) << "  |" : std::cout << "   " << "  |";
        }
        this->grid.at((i * 9) + 8) ? std::cout << "  " << this->grid.at((i * 9) + 8) << "  |  " << i + 1 << std::endl : std::cout << "   " << "  |  " << i + 1 << std::endl;
        std::cout << "_______________________________________________________" << std::endl;
    }
    std::cout << "   1     2     3     4     5     6     7     8     9   " << std::endl;
}

bool Sudoku::Grid::isValid(){
    std::vector<bool> HT;
    HT.reserve(243);
    HT.resize(243, false);
    int index = 0;
    for (const auto &i : this->sol){    
        if ((HT.at(COL(index) * 9 + (i - '0') - 1) || HT.at(ROW(index) * 9 + (i - '0') + 80) || HT.at(SUB(index) * 9 + 161 + (i - '0')))) return false;
        HT.at(COL(index) * 9 + (i - '0') - 1) = true;
        HT.at(SUB(index) * 9 + (i - '0') + 161) = true;
        HT.at(ROW(index) * 9 + (i - '0') + 80) = true;
        index++;
    }
    return true;
}

bool Sudoku::Grid::validate(){
    size_t i = 0;
    for (const auto &cell : this->grid){
        if (cell != this->sol[i]) return false;
        i++;
    }
    return true;   
}

void Sudoku::Grid::displaySol() {
    std::cout << "_______________________________________________________" << std::endl;
    for (int i = 0; i < 9; i++){
        std::cout << "|";
        for (int j = 0; j < 8; j++){
            std::cout << "  " << this->sol.at((i * 9) + j) << "  |";
        }
        std::cout << "  " << this->sol.at((i * 9) + 8) << "  |  " << i + 1 << std::endl;
        std::cout << "_______________________________________________________" << std::endl;
    }
    std::cout << "   1     2     3     4     5     6     7     8     9   " << std::endl;
}

bool Sudoku::Grid::parseCommand(std::string &cmd, int res[3]) {
	int numbers_found = 0;
	ltrim(cmd);
	rtrim(cmd);
	if (cmd.empty()) {
		std::cout << "Empty command string\n";
		return false;
	}
	for (size_t i = 0; i < cmd.size(); ++i) {
		char c = cmd[i];
		switch (c) {
			case ' ': 
				continue;
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9': {
				if (numbers_found > 2) {
					std::cout << "Found extraneous number at index " << i << " [" << c<< "]\n";
					return false;
				}
				res[numbers_found++] = c - '0';
			} break;
			default: {
				std::cout << "Found illegal character at index " << i << " [" << c << "]\n";
			} return false;
		}
	}
	if (numbers_found != 3) {
		std::cout << "Didn't find enough numbers (found " << numbers_found << ")\n";
		return false;
	}
	return true;
}

int main(void){
    using namespace std;
    using Sudoku::Difficulty;
    unordered_map<Difficulty, string> diff_lvl = {{Difficulty::EASY, "EASY"}, {Difficulty::MEDIUM, "MEDIUM"}, {Difficulty::HARD, "HARD"}};

    Difficulty lvl = Difficulty::MEDIUM;
    cout << "Enter difficulty level: ";
    cin >> lvl;
	string str = "";
	std::getline(std::cin, str);
	str = "";
    cout << "Difficulty chosen: " << diff_lvl[lvl] << endl;
    Sudoku::Grid grid(lvl);
    cout << endl;
    
	int command[3] = {};

	while (1) {
		cout << "Current grid:\n";
		grid.displayGrid();
		cout << "Enter command (<1-9> <1-9> <1-9>) | (SOLVE): ";
		std::getline(std::cin, str);
		if (str == "SOLVE") {
			cout << "Solution:\n";
			grid.displaySol();
			break;
		}
		bool res = grid.parseCommand(str, command);
		cout << command[0] << ' ' << command[1] << ' ' << command[2] << '\n';
		if (!res) continue;
		res = grid.setCell(command[0] - 1, command[1] - 1, command[2] + '0');
		if (!res) {
			cout << "Index " << command[0] << ", " << command[1] << " is contained in the starting grid\n";
		}
		if (grid.validate()) {
			cout << "You won!";
			break;
		}
	}

    return 0;
}