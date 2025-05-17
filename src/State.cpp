#include "State.hpp"

void save(const State& state, const std::string filename){
    // Open file with specified filename and descriptor.
    std::ofstream file(filename + SAVEFILE_EXTENSION, std::ios::binary);

    // Write the game state into the created file.
    file.write(reinterpret_cast<const char*>(&state), sizeof(State));
};

bool load(State& state, const std::string filename){
    // Open file with specified filename and descriptor.
    std::ifstream file(filename + SAVEFILE_EXTENSION, std::ios::binary);

    // If the file does not exist, return false.
    if (!file) return false;
    
    // Read the game state into the created file,
    // store it into the state object and return true.
    file.read(reinterpret_cast<char*>(&state), sizeof(State));
    return true;
}
 
bool get_directory(const std::string& dirname) { 
    std::error_code errcode;

    // Try to create the directory.
    // If the directory exists / is created. Return true.
    // else return false. The directory is created first 
    // to prevent race conditions on multithreaded environments.
    if (!std::filesystem::create_directory(dirname, errcode)){
        if (std::filesystem::exists(dirname)) {
            errcode.clear(); // Clear error code since the directory exists,
            return true;
        }
        return false;
    }
    return true;
}

bool remove(std::filesystem::path path){ 
    std::error_code errcode;
    if (!std::filesystem::remove(path, errcode)) {
        std::cerr << errcode;
        return false;
    }
    return true;
}