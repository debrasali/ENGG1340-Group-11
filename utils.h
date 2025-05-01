#ifndef MEMORY_UTILS_H
#define MEMORY_UTILS_H

#include "memory_structs.h"
#include <string>
#include <vector>

namespace MemoryUtils {
    // Random number generation
    int randomInt(int min, int max);
    
    // Game initialization
    void initializeGame(GameState& state, Difficulty difficulty);
    void initializeBoard(GameState& state);
    std::vector<std::string> generateEmojis(int count);
    
    // Game mechanics
    bool isValidPosition(const GameState& state, int row, int col);
    bool isCardRevealed(const GameState& state, int row, int col);
    void revealCard(GameState& state, int row, int col);
    void hideCard(GameState& state, int row, int col);
    bool checkMatch(GameState& state, int row1, int col1, int row2, int col2);
    bool isGameComplete(const GameState& state);
    
    // File operations
    bool saveGame(const GameState& state);
    bool loadGame(GameState& state);
    void saveHighScore(const HighScore& score);
    std::vector<HighScore> loadHighScores();
    
    // UI helpers
    void clearScreen();
    void displayBoard(const GameState& state);
    void displayHighScores(const std::vector<HighScore>& scores);
    void printSlow(const std::string& text, int delay = 50);
    void waitForInput();
    
    // Position conversion
    bool parsePosition(const std::string& input, int& row, int& col);
    std::string positionToString(int row, int col);
    
    // Time tracking
    int getElapsedTime(const GameState& state);
    std::string formatTime(int seconds);
}

#endif
