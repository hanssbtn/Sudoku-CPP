// Header guard to prevent multiple redefinitions.
#pragma once

#include <iostream>
#include <vector>
#include <random>
#include <cstdlib>
#include <array>
#include <stdexcept>
#include <algorithm>
#include <chrono>
#include <unordered_map> 

// Macros for finding row, column, and subgrid of a specific index in the grid. 
// Input {i ∈ Z| 0 <= i <= 80}: i.
// Formula for row index {r ∈ Z| 0 <= r <= 8}: r(i) = floor(i / 9).
// Formula for column index {c ∈ Z| 0 <= c <= 8}: c(i) = i % 9.
// Formula for subgrid index {s ∈ Z| 0 <= s <= 8}: s(i) = (floor(r(i) / 3) * 3) + floor(c(i) / 3).
#define ROW(i) ((int)(i / 9))
#define COL(i) (i % 9)
#define SUB(i) ((int)(ROW(i) / 3) * 3 + (int)(COL(i) / 3))

/// @brief Namespace containing relevant component for processing game logic.
namespace Sudoku {

    /// @brief Difficulty level type. 
    /// Used to determine how many numbers are removed from the grid.
    enum Difficulty {
        EASY = 0,
        MEDIUM = 1,
        HARD = 2
    };

    /// @brief Custom hash function for collision checking. 
    struct hashByIndex {
        unsigned int operator()(const unsigned int& key) const noexcept;
    };

    /// @brief Sudoku grid object. Contains the solved grid as well as methods for generating and manipulating the grid.
    class Grid {

        /// @brief PRNG seed for shuffling the numbers.
        unsigned int seed;

        /// @brief number of possible solutions for a given valid grid.
        unsigned int numberOfSolutions;

        /// @brief Vector representing the grid to be solved.
        std::vector<unsigned char> grid;

        /// @brief Vector representing the solution. 
        std::vector<unsigned char> sol;

        /// @brief Hash table for checking collisions.
        std::unordered_map<unsigned int, bool, hashByIndex> gridHT;

        /// @brief Hash table for checking filled indexes.
        std::vector<bool> filledIndexes;

        /// @brief Private method for checking if the board has been generated.
        /// @returns true if the board is full, false otherwise.
        bool checkSol();

        /// @brief Private method for solving the generated grid.
        /// @param emptyIndexes const reference to a vector containing empty indexes of the grid.
        /// @param iter const_iterator object to emptyIndexes.
        /// @return true if the grid is solved / solvable, false otherwise.
        bool solveGrid(const std::vector<int>& emptyIndexes, std::vector<int>::const_iterator iter);

        /// @brief Private method for generating the rest of the grid. 
        /// @param rem Vector containing remaining indexes to be filled.
        /// @param it reference to the next element in the vector.
        /// @returns true if the grid can be generated. false otherwise.
        bool genSol(const std::vector<int>& rem, std::vector<int>::const_iterator it);

		/// @brief Private method for checking if the generated grid is valid.
		/// A valid grid only has 1 unique solution. For debugging purposes only.
		/// @return true if the grid is valid. false otherwise. 
		bool isValid();
		
        public:
			/// @brief Constructor function for the Grid object.
			/// @param lvl Chosen difficulty level.
			Grid(Difficulty lvl = Sudoku::Difficulty::MEDIUM);
		
			/// @brief Method for setting the value of a cell in a grid with coordinate (row, col) with value val.
            /// @param row row index.
            /// @param col column index.
            /// @param val value to be inserted.
			/// @return true if the move is valid. returns false otherwise.
            bool setCell(int row, int col, unsigned char val);
            
            /// @brief Method for validate the answer submitted by the user. 
            /// @return true if the grid is valid, false otherwise.
            bool validate();
			
			/// @brief Method for displaying generated grid.
			void displayGrid();
                
			/// @brief Method for displaying solution grid.
			void displaySol();
            
            /// @brief Method for removing certain cell values to generate a puzzle grid using backtracking recursion.
            /// @param cellsToRemove number of cells to remove. Varies with difficulty. 
            /// @param indexes reference to a std::vector containing all remaining indexes.
            /// @param removedIndexes reference to a std::vector containing all removed indexes. 
            /// @return true if the removal generates a puzzle with a unique solution, false otherwise.
            bool removeCells(const int cellsToRemove, std::vector<int>& indexes, std::vector<int>& removedIndexes);
			
			/// @brief Method for parsing commands from the terminal.
			/// @param cmd command to parse (row, col, value)
			/// @param res int array for storing the parsed command
            /// @return true if the command is valid. false otherwise.
			bool parseCommand(std::string &cmd, int res[3]);
    };
}

/// @brief Custom definition for >> operator for the Difficulty enum.
/// @param ist input stream.
/// @param diff input variable of type Difficulty.
/// @return Modified input stream.
std::istream& operator>>(std::istream& ist, Sudoku::Difficulty& diff);
