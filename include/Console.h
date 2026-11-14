#pragma once
#include <iostream>
#include <string>
#include <limits>
#include <algorithm>
#include "SupermarketService.h"

namespace smp {

/** Small console helpers: input reading with validation. */
class Console {
public:
    static void clearScreen() {
#ifdef _WIN32
        std::system("cls");
#else
        std::system("clear");
#endif
    }

    static void pause() {
        std::cout << "\nPress Enter to continue...";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    static std::string readLine(const std::string& prompt) {
        std::cout << prompt;
        std::string s;
        std::getline(std::cin, s);
        return s;
    }

    static int readInt(const std::string& prompt) {
        while (true) {
            std::string s = readLine(prompt);
            try {
                size_t idx = 0;
                int v = std::stoi(s, &idx);
                if (idx == s.size()) return v;
            } catch (...) {}
            std::cout << "  ! Please enter a valid integer.\n";
        }
    }

    static double readDouble(const std::string& prompt) {
        while (true) {
            std::string s = readLine(prompt);
            try {
                size_t idx = 0;
                double v = std::stod(s, &idx);
                if (idx == s.size()) return v;
            } catch (...) {}
            std::cout << "  ! Please enter a valid number.\n";
        }
    }

    static bool confirm(const std::string& prompt) {
        std::string s = readLine(prompt + " (y/n): ");
        return !s.empty() && (s[0] == 'y' || s[0] == 'Y');
    }

    static void header(const std::string& title) {
        std::cout << "\n=== " << title << " ===\n";
    }

    static void separator(size_t width = 60) {
        std::cout << std::string(width, '-') << '\n';
    }
};

} // namespace smp
