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

void displayMenu() {
    clearScreenAnimate();
    
    typePrint(BLUE + R"(
    ______  ___                                     
    ___   |/  /___________ _____________________  __
    __  /|_/ /_  _ \_  __ `__ \  __ \_  ___/_  / / /
    _  /  / / /  __/  / / / / / /_/ /  /   _  /_/ / 
    /_/  /_/  \___//_/ /_/ /_/\____//_/    _\__, /  
                                           /____/   
    _________                                       
    __  ____/_____ _______ ________                 
    _  / __ _  __ `/_  __ `__ \  _ \                
    / /_/ / / /_/ /_  / / / / /  __/                
    \____/  \__,_/ /_/ /_/ /_/\___/               
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
