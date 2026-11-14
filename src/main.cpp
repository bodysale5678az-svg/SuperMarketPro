// ============================================================
//  SuperMarket Pro — entry point
//  A supermarket management system in modern C++ (C++17)
//
//  Author: Mahmoud Saleh
// ============================================================
#include "Application.h"
#include "SupermarketService.h"
#include <iostream>
#include <filesystem>

using namespace smp;

int main() {
    // Persist everything next to the executable in ./data
    SupermarketService service("data");

    try {
        service.load();
    } catch (const std::exception& e) {
        std::cerr << "Failed to load data: " << e.what() << "\n";
    }

    // First run: seed a default administrator.
    if (service.getUsers().empty()) {
        service.createUser("ADMIN", "admin", "admin123", "Default Administrator");
        service.createUser("CASHIER", "cashier", "cash123", "Front Desk");
        service.save();

        std::cout << "\n==============================================\n";
        std::cout << "  First run detected — demo accounts created:\n";
        std::cout << "    admin / admin123    (Administrator)\n";
        std::cout << "    cashier / cash123   (Cashier)\n";
        std::cout << "==============================================\n\n";
        std::cout << "Press Enter to continue...";
        std::cin.get();
    }

    Application app(service);
    app.run();

    std::cout << "\nGoodbye.\n";
    return 0;
}
