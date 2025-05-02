#include "memory_utils.h"
#include "animation.h"
#include <iostream>
#include <string>
#include <limits>
#include <thread>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <map>

using namespace MemoryUtils;

void displayDifficultyMenu() {
    clearScreen();
    // Read and display current coin count
    int coins = 0;
    std::ifstream coinFile("coin.txt");
    if (coinFile.is_open()) {
        coinFile >> coins;
        coinFile.close();
    }
    std::cout << "Coins: " << coins << "\n\n";
    
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
    } else {
        // Only start a new timer if it's not a resumed game
        state.startTime = std::chrono::steady_clock::now(); 
    }

    int finalTime = 0;  // Store the final time when game is completed
    state.skipNextStepCount = false; // Initialize skip flag

    while (!isGameComplete(state)) {
        displayBoard(state);
        std::cout << "\nEnter position of first card (e.g., A1), 'q' to quit, or 'P' to use power-up: ";
        std::string input;
        std::cin >> input;

        // Check for power-up activation
        if (input == "P" || input == "p") {
            if (state.selectedPowerUp != PowerUpType::NONE && !state.powerUpUsed) {
                if (state.selectedPowerUp == PowerUpType::QUICK_PEEK) {
                    std::cout << "\nActivating Quick Peek!\n";
                    // Reveal all cards
                    for (int r = 0; r < state.rows; ++r) {
                        for (int c = 0; c < state.cols; ++c) {
                            if (!state.board[r][c]->isMatched) {
                                revealCard(state, r, c);
                            }
                        }
                    }
                    displayBoard(state); // Show revealed board
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                    // Hide non-matched cards again
                    for (int r = 0; r < state.rows; ++r) {
                        for (int c = 0; c < state.cols; ++c) {
                            if (!state.board[r][c]->isMatched) {
                                hideCard(state, r, c);
                            }
                        }
                    }
                    state.powerUpUsed = true;
                    std::cout << "Quick Peek used.\n";
                    std::this_thread::sleep_for(std::chrono::seconds(1)); // Brief pause after hiding
                    continue; // Go back to the start of the loop to display the board and prompt again
                } else if (state.selectedPowerUp == PowerUpType::COUNT_MANIPULATOR) {
                    std::cout << "\nActivating Count Manipulator! Your next step won't be counted.\n";
                    state.skipNextStepCount = true;
                    state.powerUpUsed = true;
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                    continue; // Go back to the start of the loop
                }
            } else if (state.powerUpUsed && state.selectedPowerUp != PowerUpType::NONE) {
                 std::cout << "\nPower-up already used!\n";
                 std::this_thread::sleep_for(std::chrono::seconds(1));
                 continue;
            } else {
                std::cout << "\nNo power-up available!\n";
                std::this_thread::sleep_for(std::chrono::seconds(1));
                continue;
            }
        }

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

        int row1 = -1, col1 = -1;
        int row2 = -1, col2 = -1;
        
        if (!parsePosition(input, row1, col1) || 
            !isValidPosition(state, row1, col1) ||
            isCardRevealed(state, row1, col1) ||
            state.board[row1][col1]->isMatched) {
            std::cout << "Invalid position! Try again.\n";
            std::this_thread::sleep_for(std::chrono::seconds(3));
            continue;
        }
        
        revealCard(state, row1, col1);
        displayBoard(state);
        saveGame(state);
        
        // Get second card
        std::cout << "\nEnter position of second card (e.g., A1) or 'q' to quit: ";
        std::cin >> input;
        
        if (input == "q" || input == "Q") {
            hideCard(state, row1, col1); // Hide the first card before quitting
            std::cout << "\nGame saved. Press Enter to return to main menu...";
            std::cin.ignore();
            std::cin.get();
            return;
        }
        
        if (!parsePosition(input, row2, col2) || 
            !isValidPosition(state, row2, col2) ||
            isCardRevealed(state, row2, col2) ||
            state.board[row2][col2]->isMatched ||
            (row1 == row2 && col1 == col2)) {
            std::cout << "Invalid position! Try again.\n";
            std::this_thread::sleep_for(std::chrono::seconds(3));
            hideCard(state, row1, col1);
            continue;
        }
        
        revealCard(state, row2, col2);
        displayBoard(state);
        saveGame(state);
        
        // Increment steps only if not skipped by power-up
        if (!state.skipNextStepCount) {
            state.steps++;
        } else {
            std::cout << "\nStep count skipped due to Count Manipulator!\n";
            state.skipNextStepCount = false; // Reset flag after skipping one step
            std::this_thread::sleep_for(std::chrono::seconds(1)); 
        }

        // Check for match
        if (checkMatch(state, row1, col1, row2, col2)) {
            std::cout << "\nMatch found!\n";
            waitForInput();
        } else {
            std::cout << "\nNo match. Cards will be hidden in 3 seconds...\n";
            // Clear any pending input
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            // Simple 3-second wait
            std::this_thread::sleep_for(std::chrono::seconds(3));
            hideCard(state, row1, col1);
            hideCard(state, row2, col2);
        }
        
        if (isGameComplete(state)) {
            // Capture final time immediately when game is completed normally
            finalTime = getElapsedTime(state);
            // Delete save file since game is complete
            std::remove("memory_save.dat");
        }
    }

    // Game completed (either normally or through developer key)
    clearScreen();
    std::cout << "\nCongratulations! You've completed the game!\n";
    std::cout << "Total steps: " << state.steps << "\n";

    // Calculate elapsed time and capture it
    auto endTime = std::chrono::steady_clock::now();
    finalTime = std::chrono::duration_cast<std::chrono::seconds>(endTime - state.startTime).count();

    std::cout << "Total time: " << formatTime(finalTime) << "\n\n";
    
    // Add Coin Award Logic
    int coinsEarned = 0;
    switch (state.difficulty) {
        case Difficulty::EASY:
            coinsEarned = 100;
            break;
        case Difficulty::MEDIUM:
            coinsEarned = 200;
            break;
        case Difficulty::HARD:
            coinsEarned = 300;
            break;
        case Difficulty::EXTREME:
            coinsEarned = 500;
            break;
    }

    int currentCoins = 0;
    std::ifstream coinFileIn("coin.txt");
    if (coinFileIn.is_open()) {
        coinFileIn >> currentCoins;
        coinFileIn.close();
    }

    currentCoins += coinsEarned;

    std::ofstream coinFileOut("coin.txt");
    if (coinFileOut.is_open()) {
        coinFileOut << currentCoins;
        coinFileOut.close();
    }

    std::cout << "You earned " << coinsEarned << " coins! Total coins: " << currentCoins << "\n\n";

    
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
                
                while (diffChoice < 1 || diffChoice > 4) {
                    std::cout << "Invalid choice!\n";
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                    displayDifficultyMenu();
                    std::cin >> diffChoice;
                    break;
                }
                
                initializeGame(state, getDifficultyFromChoice(diffChoice));

                // *** Add Power-up Purchase Logic ***
                clearScreen();
                int currentCoins = 0;
                std::ifstream coinFileIn("coin.txt");
                if (coinFileIn.is_open()) {
                    coinFileIn >> currentCoins;
                    coinFileIn.close();
                }

                std::cout << "Coins: " << currentCoins << "\n\n";
                std::cout << "Purchase Power-up\n";
                std::cout << "---------------------\n";
                const int quickPeekCost = 50;
                const int countManipulatorCost = 100;
                std::cout << "1. Quick Peek (Cost: " << quickPeekCost << ") - Briefly reveal all cards\n";
                std::cout << "2. Count Manipulator (Cost: " << countManipulatorCost << ") - Next step doesn't count\n";
                std::cout << "3. None\n\n";
                std::cout << "Enter your choice: ";

                int powerUpChoice;
                std::cin >> powerUpChoice;

                state.selectedPowerUp = PowerUpType::NONE; // Default to none
                state.powerUpUsed = true; // Default to used (no power-up active)

                if (powerUpChoice == 1) { // Quick Peek
                    if (currentCoins >= quickPeekCost) {
                        currentCoins -= quickPeekCost;
                        state.selectedPowerUp = PowerUpType::QUICK_PEEK;
                        state.powerUpUsed = false; // Power-up available
                        std::cout << "\nPurchased Quick Peek!\n";
                    } else {
                        std::cout << "\nCoin is not enough! Proceeding without power-up.\n";
                    }
                } else if (powerUpChoice == 2) { // Count Manipulator
                    if (currentCoins >= countManipulatorCost) {
                        currentCoins -= countManipulatorCost;
                        state.selectedPowerUp = PowerUpType::COUNT_MANIPULATOR;
                        state.powerUpUsed = false; // Power-up available
                        std::cout << "\nPurchased Count Manipulator!\n";
                    } else {
                        std::cout << "\nCoin is not enough! Proceeding without power-up.\n";
                    }
                } else { // None or invalid choice
                    std::cout << "\nProceeding without power-up.\n";
                }

                // Save updated coin count if a power-up was purchased
                if (state.selectedPowerUp != PowerUpType::NONE) {
                    std::ofstream coinFileOut("coin.txt");
                    if (coinFileOut.is_open()) {
                        coinFileOut << currentCoins;
                        coinFileOut.close();
                    }
                }
                
                std::cout << "\nPress Enter to start the game...";
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear buffer
                std::cin.get(); // Wait for Enter
                // *** End Power-up Purchase Logic ***

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
                std::this_thread::sleep_for(std::chrono::seconds(3));
        }
    }
    
    return 0;
} 

