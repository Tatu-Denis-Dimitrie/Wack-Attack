export module cout;

import <iostream>;
import <string>;
#include <windows.h> 

namespace mycout {
    export enum class Color {
        Default = 0,
        Red = 31,
        Green = 32,
        Yellow = 33,
        Blue = 34,
        Magenta = 35,
        Cyan = 36,
        White = 37
    };

    void enableAnsiColors() {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD dwMode = 0;

        if (GetConsoleMode(hConsole, &dwMode)) {
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hConsole, dwMode);
        }
    }

    export void display(const std::string& message, Color color = Color::Default) {
        enableAnsiColors();

        std::cout << "\033[" << static_cast<int>(color) << "m" << message << "\033[0m\n";
    }
}

namespace mycerr {
    export enum class Color {
        Default = 0,
        Red = 31,
        Green = 32,
        Yellow = 33,
        Blue = 34,
        Magenta = 35,
        Cyan = 36,
        White = 37
    };
    void enableAnsiColors() {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD dwMode = 0;

        if (GetConsoleMode(hConsole, &dwMode)) {
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hConsole, dwMode);
        }
    }
    export void display(const std::string& message, Color color = Color::Default) {
        enableAnsiColors();
        std::cerr << "\033[" << static_cast<int>(color) << "m" << message << "\033[0m\n";
    }
}
