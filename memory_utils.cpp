#include "memory_utils.h"
#include <iostream>
#include <fstream>
#include <random>
#include <chrono>
#include <thread>
#include <algorithm>
#include <iomanip>

namespace MemoryUtils {
    // Random number generation
    int randomInt(int min, int max) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(min, max);
        return dis(gen);
    }
    
    // Game initialization
    void initializeGame(GameState& state, Difficulty difficulty) {
        state.difficulty = difficulty;
        state.steps = 0;
        state.matches = 0;
        
        state.isPaused = false;
        state.saveFileName = "memory_save.dat";
        state.selectedPowerUp = PowerUpType::NONE; // Initialize power-up state
        state.powerUpUsed = true;               // Initialize as used (no power-up active)
        state.skipNextStepCount = false;       // Initialize step skip flag
        
        // Set grid size based on difficulty
        switch (difficulty) {
            case Difficulty::EASY:
                state.rows = 3;
                state.cols = 4;
                break;
            case Difficulty::MEDIUM:
                state.rows = 5;
                state.cols = 6;
                break;
            case Difficulty::HARD:
                state.rows = 7;
                state.cols = 8;
                break;
            case Difficulty::EXTREME:
                state.rows = 9;
                state.cols = 10;
                break;
        }
        
        initializeBoard(state);
    }

    // Initializes the board with shuffled pairs of cards based on the game state.
    void initializeBoard(GameState& state) {
        // Clear existing board
        state.board.clear();
        state.board.resize(state.rows, std::vector<std::shared_ptr<Card>>(state.cols));
        
        // Generate emojis for pairs
        int totalPairs = (state.rows * state.cols) / 2;
        std::vector<std::string> emojis = generateEmojis(totalPairs);
        
        // Create pairs of cards
        std::vector<std::shared_ptr<Card>> cards;
        for (int i = 0; i < totalPairs; ++i) {
            for (int j = 0; j < 2; ++j) {
                auto card = std::make_shared<Card>();
                card->emoji = emojis[i];
                card->isRevealed = false;
                card->isMatched = false;
                cards.push_back(card);
            }
        }
        
        // Shuffle cards
        std::random_device rd;
        std::mt19937 gen(rd());
        std::shuffle(cards.begin(), cards.end(), gen);
        
        // Place cards on board
        int cardIndex = 0;
        for (int row = 0; row < state.rows; ++row) {
            for (int col = 0; col < state.cols; ++col) {
                state.board[row][col] = cards[cardIndex++];
                state.board[row][col]->row = row;
                state.board[row][col]->col = col;
            }
        }
    }

    // Generates a vector of unique emoji strings for the board.
    std::vector<std::string> generateEmojis(int count) {
        // List of emojis to use
        std::vector<std::string> allEmojis = {
            "🌟", "🔥", "💧", "🌈", "⚡", "🍎", "🌹", "☀️",
            "🌙", "⭐", "☁️", "❄️", "🌊", "🍀", "🌴", "🍁",
            "🍂", "🌸", "🌼", "🌻", "🍉", "🍇", "🍓", "🍒",
            "🍍", "🥥", "🍆", "🥕", "🌽", "🍄", "🥐", "🍔",
            "🍟", "🍕", "🍝", "🍣", "🍤", "🍦", "🍰", "🍫",
            "🍬", "🍭", "☕", "🍵", "🍷", "🍺", "🎉", "🎁"
        };

        // Shuffle and select the required number of emojis
        std::random_device rd;
        std::mt19937 gen(rd());
        std::shuffle(allEmojis.begin(), allEmojis.end(), gen);
        
        return std::vector<std::string>(allEmojis.begin(), allEmojis.begin() + count);
    }
    
    // Game mechanics
    // Checks if the given row and column are within the bounds of the board.
    // Output: true if the position is valid; false otherwise.
    bool isValidPosition(const GameState& state, int row, int col) {
        return row >= 0 && row < state.rows && col >= 0 && col < state.cols;
    }

    // Checks if the card at the specified position is currently revealed.
    // Output: true if the card is revealed; false otherwise.
    bool isCardRevealed(const GameState& state, int row, int col) {
        return state.board[row][col]->isRevealed;
    }

    // Reveals the card at the specified position.
    void revealCard(GameState& state, int row, int col) {
        state.board[row][col]->isRevealed = true;
    }

    // Hides the card at the specified position.
    void hideCard(GameState& state, int row, int col) {
        state.board[row][col]->isRevealed = false;
    }

    // Checks if the two specified positions on the board contain matching cards.
    // Output: true if the cards match; false otherwise.
    bool checkMatch(GameState& state, int row1, int col1, int row2, int col2) {
        if (state.board[row1][col1]->emoji == state.board[row2][col2]->emoji) {
            state.board[row1][col1]->isMatched = true;
            state.board[row2][col2]->isMatched = true;
            state.matches++;
            return true;
        }
        return false;
    }

    // Checks if all pairs have been matched and the game is complete.
    // Output: true if the game is complete; false otherwise.
    bool isGameComplete(const GameState& state) {
        return state.matches == (state.rows * state.cols) / 2;
    }

    // File operations: saves the current game state to a file.
    // Output: true if the game was saved successfully; false otherwise.
    bool saveGame(const GameState& state) {
        std::ofstream file(state.saveFileName, std::ios::binary);
        if (!file) return false;
        
        // Save basic game state
        file.write(reinterpret_cast<const char*>(&state.steps), sizeof(state.steps));
        file.write(reinterpret_cast<const char*>(&state.matches), sizeof(state.matches));
        file.write(reinterpret_cast<const char*>(&state.difficulty), sizeof(state.difficulty));
        
        // Save timer state
        file.write(reinterpret_cast<const char*>(&state.startTime), sizeof(state.startTime));
        file.write(reinterpret_cast<const char*>(&state.pausedTime), sizeof(state.pausedTime));
        file.write(reinterpret_cast<const char*>(&state.isPaused), sizeof(state.isPaused));
        
        // Save board
        for (const auto& row : state.board) {
            for (const auto& card : row) {
                size_t emojiLength = card->emoji.length();
                file.write(reinterpret_cast<const char*>(&emojiLength), sizeof(emojiLength));
                file.write(card->emoji.c_str(), emojiLength);
                file.write(reinterpret_cast<const char*>(&card->isRevealed), sizeof(card->isRevealed));
                file.write(reinterpret_cast<const char*>(&card->isMatched), sizeof(card->isMatched));
            }
        }
        
        return true;
    }

    // Loads the game state from a file if it exists.
    // Output: true if the game was loaded successfully; false otherwise.
    bool loadGame(GameState& state) {
        std::ifstream file(state.saveFileName, std::ios::binary);
        if (!file) return false;
        
        // Load basic game state
        file.read(reinterpret_cast<char*>(&state.steps), sizeof(state.steps));
        file.read(reinterpret_cast<char*>(&state.matches), sizeof(state.matches));
        file.read(reinterpret_cast<char*>(&state.difficulty), sizeof(state.difficulty));
        
        // Load timer state
        file.read(reinterpret_cast<char*>(&state.startTime), sizeof(state.startTime));
        file.read(reinterpret_cast<char*>(&state.pausedTime), sizeof(state.pausedTime));
        file.read(reinterpret_cast<char*>(&state.isPaused), sizeof(state.isPaused));
        
        // Initialize board
        state.board.clear();
        state.board.resize(state.rows, std::vector<std::shared_ptr<Card>>(state.cols));
        
        // Load board
        for (int row = 0; row < state.rows; ++row) {
            for (int col = 0; col < state.cols; ++col) {
                auto card = std::make_shared<Card>();
                size_t emojiLength;
                file.read(reinterpret_cast<char*>(&emojiLength), sizeof(emojiLength));
                card->emoji.resize(emojiLength);
                file.read(&card->emoji[0], emojiLength);
                file.read(reinterpret_cast<char*>(&card->isRevealed), sizeof(card->isRevealed));
                file.read(reinterpret_cast<char*>(&card->isMatched), sizeof(card->isMatched));
                card->row = row;
                card->col = col;
                state.board[row][col] = card;
            }
        }
        
        return true;
    }

    // Saves a high score entry
    void saveHighScore(const HighScore& score) {
        std::ofstream file("highscores.txt", std::ios::app);
        if (!file) return;
        
        // Save the complete player name
        file << score.playerName << "|"
             << score.steps << "|"
             << score.timeInSeconds << "|"
             << static_cast<int>(score.difficulty) << "\n";
    }

    // Loads all high scores
    // Output: Vector of high score entries.    
    std::vector<HighScore> loadHighScores() {
        std::vector<HighScore> scores;
        std::ifstream file("highscores.txt");
        if (!file) return scores;
        
        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            HighScore score;
            std::string difficultyStr;
            
            // Read the complete player name
            if (std::getline(iss, score.playerName, '|') &&
                iss >> score.steps &&
                iss.ignore() &&
                iss >> score.timeInSeconds &&
                iss.ignore() &&
                iss >> difficultyStr) {
                score.difficulty = static_cast<Difficulty>(std::stoi(difficultyStr));
                scores.push_back(score);
            }
        }
        
        // Sort scores by steps (ascending) and time (ascending)
        std::sort(scores.begin(), scores.end(), [](const HighScore& a, const HighScore& b) {
            if (a.steps != b.steps) return a.steps < b.steps;
            return a.timeInSeconds < b.timeInSeconds;
        });
        
        return scores;
    }
    
    // UI helpers: clears the console screen.
    void clearScreen() {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
    }

    // Displays the current state of the board to the console.
    void displayBoard(const GameState& state) {
        clearScreen();
        std::cout << "Steps: " << state.steps << " | Matches: " << state.matches 
                  << " | Time: " << formatTime(getElapsedTime(state));

        // Display available power-up
        if (state.selectedPowerUp != PowerUpType::NONE && !state.powerUpUsed) {
            std::cout << " | Power-up: ";
            switch (state.selectedPowerUp) {
                case PowerUpType::QUICK_PEEK:
                    std::cout << "Quick Peek (Press P)";
                    break;
                case PowerUpType::COUNT_MANIPULATOR:
                    std::cout << "Count Manipulator (Press P)";
                    break;
                default: // NONE or should not happen
                    break;
            }
        }
        std::cout << "\n\n";
        
        // Print column headers (A, B, C, ...)
        std::cout << "  ";
        for (int col = 0; col < state.cols; ++col) {
            std::cout << " " << static_cast<char>('A' + col) << " ";
        }
        std::cout << "\n";
        
        // Print board
        for (int row = 0; row < state.rows; ++row) {
            // Print row number
            std::cout << row + 1 << " ";
            
            // Print cards
            for (int col = 0; col < state.cols; ++col) {
                const auto& card = state.board[row][col];
                if (card->isMatched) {
                    std::cout << " ✓ ";
                } else if (card->isRevealed) {
                    std::cout << " " << card->emoji << " ";
                } else {
                    std::cout << " ■ ";
                }
            }
            std::cout << "\n";
        }
    }

    // Displays the list of high scores in a formatted table.
    void displayHighScores(const std::vector<HighScore>& scores) {
        clearScreen();
        std::cout << "High Scores\n";
        std::cout << "-----------\n\n";
        
        std::cout << std::left << std::setw(20) << "Player"
                  << std::setw(10) << "Steps"
                  << std::setw(15) << "Time"
                  << "Difficulty\n";
        std::cout << std::string(60, '-') << "\n";
        
        for (const auto& score : scores) {
            std::string difficulty;
            switch (score.difficulty) {
                case Difficulty::EASY: difficulty = "Easy"; break;
                case Difficulty::MEDIUM: difficulty = "Medium"; break;
                case Difficulty::HARD: difficulty = "Hard"; break;
                case Difficulty::EXTREME: difficulty = "Extreme"; break;
            }
            
            std::cout << std::left << std::setw(20) << score.playerName
                      << std::setw(10) << score.steps
                      << std::setw(15) << formatTime(score.timeInSeconds)
                      << difficulty << "\n";
        }
    }

    // Prints text to the console one character at a time, with a delay between characters.
    void printSlow(const std::string& text, int delay) {
        for (char c : text) {
            std::cout << c << std::flush;
            std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        }
    }

    // Waits for the user to press Enter to continue.
    void waitForInput() {
        std::cout << "\nPress Enter to continue...";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    
    // Parses a board position string (e.g., "A1") into row and column indices.
    // Output: true if parsing was successful; false otherwise.
    bool parsePosition(const std::string& input, int& row, int& col) {
        if (input.length() < 2) return false;
        
        char colChar = toupper(input[0]);
        if (colChar < 'A' || colChar > 'Z') return false;
        
        try {
            int rowNum = std::stoi(input.substr(1));
            if (rowNum < 1) return false;
            
            col = colChar - 'A';
            row = rowNum - 1;
            return true;
        } catch (...) {
            return false;
        }
    }

    // Converts row and column indices to a board position string (e.g., "A1").
    // Output: Board position string.
    std::string positionToString(int row, int col) {
        return std::string(1, 'A' + col) + std::to_string(row + 1);
    }
    
    // Time tracking
    int getElapsedTime(const GameState& state) {
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - state.startTime);
        return static_cast<int>(duration.count());
    }

    // Formats a time duration in seconds as a string in MM:SS format.
    // Output: Formatted time string.
    std::string formatTime(int seconds) {
        int minutes = seconds / 60;
        seconds %= 60;
        return std::to_string(minutes) + ":" + 
               (seconds < 10 ? "0" : "") + std::to_string(seconds);
    }
} 
