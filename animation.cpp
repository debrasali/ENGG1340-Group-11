#include "animation.h"
#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

void initConsole() {
    #ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
    #endif
}

const string BLUE = "\033[94m";
const string CYAN = "\033[96m";
const string RESET = "\033[0m";

void typePrint(const string& text, int delayMs) {
    for (char c : text) {
        cout << c << flush;
        this_thread::sleep_for(chrono::milliseconds(delayMs));
    }
}

void clearScreenAnimate() {
    #ifdef _WIN32
    system("cls");
    #else
    cout << "\033[2J\033[1;1H";
    #endif
}

void displayAnimatedMenu() {
    clearScreenAnimate();
    
    typePrint(BLUE + R"(
    (_______)                              
     _  _  _ _____ ____   ___   ____ _   _ 
    | ||_|| | ___ |    \ / _ \ / ___) | | |
    | |   | | ____| | | | |_| | |   | |_| |
    |_|   |_|_____)_|_|_|\___/|_|    \__  |
                                    (____/ 
     _______                               
    (_______)                              
     _   ___ _____ ____  _____             
    | | (_  (____ |    \| ___ |            
    | |___) / ___ | | | | ____|            
     \_____/\_____|_|_|_|_____)               
)" + RESET + "\n", 1);

    typePrint(CYAN + R"(
    ╔═════════════════════════════╗
    ║ )" + BLUE + "[1] New Game" + CYAN +  R"(               ║
    ║ )" + BLUE + "[2] Load Game" + CYAN +  R"(              ║
    ║ )" + BLUE + "[3] View High Scores" + CYAN +  R"(       ║
    ║ )" + BLUE + "[4] Exit" + CYAN +  R"(                   ║
    ╚═════════════════════════════╝
)" + RESET + "\n", 10);

    cout << BLUE;
    for(int i = 0; i < 3; ++i) {
        cout << "Enter your choice: ";
        cout << (i%2 ? ">" : " ") << "\033[K";
        cout.flush();
        this_thread::sleep_for(chrono::milliseconds(300));
        cout << "\r";
    }
    cout << "Enter your choice: " << RESET;
}

void gameWinScreen() {
    typePrint(YELLOW + R"(
        .
                  .       |         .    .
            .  *         -*-          *
                 \        |         /   .
.    .            .      /^\     .              .    .
   *    |\   /\    /\  / / \ \  /\    /\   /|    *
 .   .  |  \ \/ /\ \ / /     \ \ / /\ \/ /  | .     .
         \ | _ _\/_ _ \_\_ _ /_/_ _\/_ _ \_/
           \  *  *  *   \ \/ /  *  *  *  /
            ` ~ ~ ~ ~ ~  ~\/~ ~ ~ ~ ~ ~ '

                      YOU WON!
    
)" + RESET + "\n", 1)
}
