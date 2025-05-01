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

void clearScreen() {
    #ifdef _WIN32
    system("cls");
    #else
    cout << "\033[2J\033[1;1H";
    #endif
}

void displayMenu() {
    clearScreen();
    
    
    cout << BLUE << R"(
     _  _ __ _  _  __  ___
    |  \/  |  _|  \/  |/ _ \|  __ \
    | \  / | |_  | \  / | |  | | |_) |
    | |\/| |  __| | |\/| | |  | |  _  /
    | |  | | |_| |  | | |_| | | \ \
    ||  ||__||  ||\_/||  \_\
    )" << endl;

    
    cout << CYAN << R"(
    ╔══════════════════════╗
    ║ )" << BLUE << "[1] New Game" << CYAN << R"(       ║
    ║ )" << BLUE << "[2] Continue" << CYAN << R"(       ║
    ║ )" << BLUE << "[3] Leaderboard" << CYAN << R"(    ║
    ║ )" << BLUE << "[4] Exit" << CYAN << R"(           ║
    ╚══════════════════════╝
    )" << RESET << endl;

    
    cout << BLUE;
    for(int i = 0; i < 3; ++i) {
        cout << "Select option [1-4]: ";
        cout << (i%2 ? ">" : " ") << "\033[K";
        cout.flush();
        this_thread::sleep_for(chrono::milliseconds(300));
        cout << "\r";
    }
    cout << RESET;
}

int main() {
    initConsole();
    displayMenu();
    return 0;
}
