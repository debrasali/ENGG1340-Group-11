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
    
