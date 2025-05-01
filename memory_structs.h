#ifndef MEMORY_STRUCTS_H
#define MEMORY_STRUCTS_H

#include <string>
#include <vector>
#include <memory>
#include <ctime>
#include <chrono>

enum class Difficulty {
    EASY,    // 3x4 grid
    MEDIUM,  // 5x6 grid
    HARD,    // 7x8 grid
    EXTREME  // 9x10 grid
};

struct Card {
    std::string emoji;      // The emoji character
    bool isRevealed;        // Whether the card is face up
    bool isMatched;         // Whether the card has been matched
    int row;                // Row position (0-based)
    int col;                // Column position (0-based)
};

struct GameState {
    std::vector<std::vector<std::shared_ptr<Card>>> board;  // 2D grid of cards
    int rows;                                               // Number of rows
    int cols;                                               // Number of columns
    int steps;                                              // Number of steps taken
    int matches;                                            // Number of matches found
    Difficulty difficulty;                                  // Current difficulty level
    std::string saveFileName;                              // File name for saving game
    std::chrono::steady_clock::time_point startTime;          // Game start time
    std::chrono::steady_clock::time_point pausedTime;           // Track when game was paused
    bool isPaused;                                           // Track if game is currently paused
};

struct HighScore {
    std::string playerName;
    int steps;
    int timeInSeconds;
    Difficulty difficulty;
};

#endif
