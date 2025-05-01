#include "memory_utils.h"
#include "animation.h"
#include <iostream>
#include <string>
#include <limits>
#include <thread>
#include <chrono>
#include <iomanip>
#include <map>

using namespace MemoryUtils;

void displayDifficultyMenu() {
    clearScreen();
    std::cout << "Select Difficulty\n";
    std::cout << "----------------\n\n";
    std::cout << "1. Easy (3x4)\n";
    std::cout << "2. Medium (5x6)\n";
    std::cout << "3. Hard (7x8)\n";
    std::cout << "4. Extreme (9x10)\n\n";
    std::cout << "Enter your choice: ";
}

void displayHighScoresMenu(const std::vector<HighScore>& scores) {
    clearScreen();
    std::cout << "High Scores\n";
    std::cout << "-----------\n\n";
    
    if (scores.empty()) {
        std::cout << "No high scores yet!\n\n";
    } else {
        // Group scores by difficulty
        std::map<Difficulty, std::vector<HighScore>> scoresByDifficulty;
        for (const auto& score : scores) {
            scoresByDifficulty[score.difficulty].push_back(score);
        }

        // Display scores for each difficulty
        for (const auto& [difficulty, diffScores] : scoresByDifficulty) {
            std::string difficultyName;
            switch (difficulty) {
                case Difficulty::EASY: difficultyName = "Easy (3x4)"; break;
                case Difficulty::MEDIUM: difficultyName = "Medium (5x6)"; break;
                case Difficulty::HARD: difficultyName = "Hard (7x8)"; break;
                case Difficulty::EXTREME: difficultyName = "Extreme (9x10)"; break;
            }
            
            std::cout << "\n" << difficultyName << "\n";
            std::cout << std::string(45, '-') << "\n";
            
            std::cout << std::left << std::setw(20) << "Player"
                      << std::setw(10) << "Steps"
                      << "Time\n";
            std::cout << std::string(45, '-') << "\n";
            
            for (const auto& score : diffScores) {
                std::cout << std::left << std::setw(20) << score.playerName
                          << std::setw(10) << score.steps
                          << formatTime(score.timeInSeconds) << "\n";
                std::cout << std::string(45, '-') << "\n";  // Add line after each score
            }
        }
    }
    
    std::cout << "\nPress Enter to return to main menu...";
    std::cin.ignore();
    std::cin.get();
}

Difficulty getDifficultyFromChoice(int choice) {
    switch (choice) {
        case 1: return Difficulty::EASY;
        case 2: return Difficulty::MEDIUM;
        case 3: return Difficulty::HARD;
        case 4: return Difficulty::EXTREME;
        default: return Difficulty::EASY;
    }
}

void playGame(GameState& state) {
    // If game was paused, resume the timer
    if (state.isPaused) {
        auto pauseDuration = std::chrono::steady_clock::now() - state.pausedTime;
        state.startTime += pauseDuration;
        state.isPaused = false;
    }

    int finalTime = 0;  // Store the final time when game is completed

    while (!isGameComplete(state)) {
        displayBoard(state);
        std::cout << "\nEnter position of first card (e.g., A1) or 'q' to quit: ";
        std::string input;
        std::cin >> input;

        // Check for developer key
        if (input == "alvinbwdean") {
            // Instantly match all cards
            for (int row = 0; row < state.rows; ++row) {
                for (int col = 0; col < state.cols; ++col) {
                    if (!state.board[row][col]->isMatched) {
                        state.board[row][col]->isMatched = true;
                        state.matches++;
                    }
                }
            }
            // Capture final time immediately
            finalTime = getElapsedTime(state);
            // Delete save file since game is complete
            std::remove("memory_save.dat");
            break;  // Exit the game loop
        }

        if (input == "q" || input == "Q") {
            // Pause the timer
            state.pausedTime = std::chrono::steady_clock::now();
            state.isPaused = true;
            saveGame(state);
            std::cout << "\nGame saved. Press Enter to return to main menu...";
            std::cin.ignore();
            std::cin.get();
            return;
        }

        int firstRow = -1, firstCol = -1;
        int secondRow = -1, secondCol = -1;
        
        if (!parsePosition(input, firstRow, firstCol) || 
            !isValidPosition(state, firstRow, firstCol) ||
            isCardRevealed(state, firstRow, firstCol) ||
            state.board[firstRow][firstCol]->isMatched) {
            std::cout << "Invalid position! Try again.\n";
            std::this_thread::sleep_for(std::chrono::seconds(3));
            continue;
        }
        
        revealCard(state, firstRow, firstCol);
        displayBoard(state);
        saveGame(state);
        
        // Get second card
        std::cout << "\nEnter position of second card (e.g., A1) or 'q' to quit: ";
        std::cin >> input;
        
        if (input == "q" || input == "Q") {
            hideCard(state, firstRow, firstCol); // Hide the first card before quitting
            std::cout << "\nGame saved. Press Enter to return to main menu...";
            std::cin.ignore();
            std::cin.get();
            return;
        }
        
        if (!parsePosition(input, secondRow, secondCol) || 
            !isValidPosition(state, secondRow, secondCol) ||
            isCardRevealed(state, secondRow, secondCol) ||
            state.board[secondRow][secondCol]->isMatched ||
            (firstRow == secondRow && firstCol == secondCol)) {
            std::cout << "Invalid position! Try again.\n";
            std::this_thread::sleep_for(std::chrono::seconds(3));
            continue;
        }
        
        revealCard(state, secondRow, secondCol);
        displayBoard(state);
        saveGame(state);
        
        // Check for match
        if (checkMatch(state, firstRow, firstCol, secondRow, secondCol)) {
            std::cout << "\nMatch found!\n";
            waitForInput();
        } else {
            std::cout << "\nNo match. Cards will be hidden in 3 seconds...\n";
            // Clear any pending input
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            // Simple 3-second wait
            std::this_thread::sleep_for(std::chrono::seconds(3));
            hideCard(state, firstRow, firstCol);
            hideCard(state, secondRow, secondCol);
        }
        
        state.steps++;
        firstRow = -1;
        saveGame(state);

        if (isGameComplete(state)) {
            // Capture final time immediately when game is completed normally
            finalTime = getElapsedTime(state);
            // Delete save file since game is complete
            std::remove("memory_save.dat");
        }
    }

    // Game completed (either normally or through developer key)
    clearScreen();
    std::cout << "Congratulations! You've completed the game!\n";
    std::cout << "Total steps: " << state.steps << "\n";
    std::cout << "Total time: " << formatTime(finalTime) << "\n\n";
    
    // Save high score
    std::string playerName;
    std::cout << "Enter your name for the high score: ";
    std::cin.ignore();
    std::getline(std::cin, playerName);
    
    HighScore score;
    score.playerName = playerName;
    score.steps = state.steps;
    score.timeInSeconds = finalTime;  // Use the captured final time
    score.difficulty = state.difficulty;
    
    saveHighScore(score);
    std::cout << "\nHigh score saved!\n";
    std::cout << "Press Enter to return to main menu...\n";
    std::cin.get();
}

int main() {
    initConsole();
    GameState state;
    int choice;
    
    while (true) {
        displayAnimatedMenu();
        std::cin >> choice;
        
        switch (choice) {
            case 1: { // New Game
                // Delete any existing saved game when starting new game
                std::remove("memory_save.dat");
                
                displayDifficultyMenu();
                int diffChoice;
                std::cin >> diffChoice;
                
                if (diffChoice < 1 || diffChoice > 4) {
                    std::cout << "Invalid choice!\n";
                    std::this_thread::sleep_for(std::chrono::seconds(3));
                    break;
                }
                
                initializeGame(state, getDifficultyFromChoice(diffChoice));
                playGame(state);
                break;
            }
            
            case 2: { // Load Game
                if (!loadGame(state)) {
                    std::cout << "\nThere is currently no game running.\n";
                    std::cout << "Press Enter to return to main menu...";
                    std::cin.ignore();
                    std::cin.get();
                    break;
                }
                
                // Check if the loaded game is already complete
                if (isGameComplete(state)) {
                    std::cout << "\nThis game is already complete. Starting a new game...\n";
                    std::remove("memory_save.dat");
                    std::cout << "Press Enter to return to main menu...";
                    std::cin.ignore();
                    std::cin.get();
                    break;
                }
                
                playGame(state);
                break;
            }
            
            case 3: { // View High Scores
                std::vector<HighScore> scores = loadHighScores();
                displayHighScoresMenu(scores);
                break;
            }
            
            case 4: // Exit
                return 0;
                
            default:
                std::cout << "Invalid choice!\n";
                waitForInput();
        }
    }
    
    return 0;
} 
