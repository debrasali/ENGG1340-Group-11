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
        state.startTime = std::chrono::steady_clock::now();
        state.isPaused = false;
        state.saveFileName = "memory_save.dat";
        
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
    bool isValidPosition(const GameState& state, int row, int col) {
        return row >= 0 && row < state.rows && col >= 0 && col < state.cols;
    }
    
    bool isCardRevealed(const GameState& state, int row, int col) {
        return state.board[row][col]->isRevealed;
    }
    
    void revealCard(GameState& state, int row, int col) {
        state.board[row][col]->isRevealed = true;
    }
    
    void hideCard(GameState& state, int row, int col) {
        state.board[row][col]->isRevealed = false;
    }
    
    bool checkMatch(GameState& state, int row1, int col1, int row2, int col2) {
        if (state.board[row1][col1]->emoji == state.board[row2][col2]->emoji) {
            state.board[row1][col1]->isMatched = true;
            state.board[row2][col2]->isMatched = true;
            state.matches++;
            return true;
        }
        return false;
    }
    
    bool isGameComplete(const GameState& state) {
        return state.matches == (state.rows * state.cols) / 2;
    }

// File operations
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
    
    void saveHighScore(const HighScore& score) {
        std::ofstream file("highscores.txt", std::ios::app);
        if (!file) return;
        
        // Save the complete player name without cutting off the first letter
        file << score.playerName << "|"
             << score.steps << "|"
             << score.timeInSeconds << "|"
             << static_cast<int>(score.difficulty) << "\n";
    }
    
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
    
    // UI helpers
    void clearScreen() {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
    }
    
    void displayBoard(const GameState& state) {
        clearScreen();
        std::cout << "Steps: " << state.steps << " | Matches: " << state.matches 
                  << " | Time: " << formatTime(getElapsedTime(state)) << "\n\n";
        
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
    
