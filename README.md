# Win10-Sudoku
A C++ implementation of Sudoku using Win32 API.
Made as a final project for Sampoerna University's Data Structures and Algorithms class.
Built using ```g++ v13.2.0``` within the C++23 standard and compiled on an x86_64 architecture.

# Overview
The app is made up of 2 source files, one containing the game logic named Sudoku.cpp, and one containing the GUI for the player to interact with named SUDO.cpp.
The procedure of the game is as follows:
 1. Generate a randomized solved grid / Load a previously saved game.
 2. Use generated solved grid to generate playable grid using recursive back propagation.
 3. Display the playable grid in a window created using the Win32 API.
 4. Handle player input such as writing to a cell using Win32 API.  
 5. Save previous games according to the player's request. 
The save files are stored in the ```Users/.../Documents/savefiles_sudoku``` folder. 
**WARNING**
Do not tamper with the save files in any way.
Do not try to create save files manually. 

# Main contributors
 - Rainer D. Elias
 - Vincent Liem
 - Edison Widjaja
 - Johan Iswara
 - Hans Sebastian

# System requirements
 - ```g++``` v13.2.0 or above
 - Windows 10 or above
 - ```-std=c++23``` or above
 - (Optional) GNU ```make 4.4.1``` or above

# Installation
To install, simply run the makefile located inside the ```src``` folder, or compile each ```.cpp``` file separately and linking them with the required DLLs such as:
 - ```commctrl.h``` 
 - ```gdi32.dll```
 - ```user32.dll```