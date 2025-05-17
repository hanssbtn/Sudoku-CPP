/// Preprocessor directive to prevent multiple redefinitions
#pragma once
#include <fstream>
#include <filesystem>
#include <string>
#include <chrono>
#include "Sudoku.hpp"

#define SAVEFILE_EXTENSION ".sudokugamefile"

/// @brief Game state object. 
struct State {
    /// @brief Saved sudoku grid.
    Sudoku::Grid saved_grid;

    /// @brief Saved difficulty lvl.
    Sudoku::Difficulty lvl;

    /// @brief Saved game duration.
    std::chrono::duration<double> dur;
};

/// @brief Function for saving current game state.
/// @param state State object to save.
/// @param filename Name of file to store state.
void save(const State& state, const std::string& filename);

/// @brief Function for loading a previously saved game.
/// @param state State object to load.
/// @param filename Name of file containing saved game state.
/// @returns ```true``` if there is a savefile. ```false``` otherwise.
bool load(State& state, const std::string& filename);

/// @brief Function for creating / accessing a directory.
/// @param dirname directory name to access.
/// @return ```true``` if the directory exists / created. ```false``` otherwise.
bool get_directory(const std::string& dirname);

/// @brief Function for deleting a save file
/// @param path Absolute path to the file.
/// @return ```true``` if the deletion is successful. return ```false``` otherwise.
bool remove(std::filesystem::path path);