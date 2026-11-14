#pragma once
#include <memory>
#include <functional>
#include <map>
#include "SupermarketService.h"
#include "Console.h"

namespace smp {

/**
 * Application — the command-line front end.
 *
 * Owns the menu flow, delegates every business decision to
 * SupermarketService and never manipulates storage itself.
 */
class Application {
private:
    SupermarketService& svc;
    std::shared_ptr<User> currentUser;
    std::vector<CartItem> cart;

    void printProductTable(const std::vector<Product>& list) const {
        if (list.empty()) {
            std::cout << "  (no products)\n";
            return;
        }
        std::cout << std::left << std::setw(4)  << "ID"
                  << std::setw(10) << "SKU"
                  << std::setw(28) << "Name"
                  << std::setw(16) << "Category"
                  << std::right << std::setw(9) << "Price"
                  << std::setw(8) << "Qty"
                  << std::setw(8) << "Disc%" << "\n";
        Console::separator(83);
        for (const auto& p : list) std::cout << p << "\n";
    }

    // ---------- screens ----------
    void screenLogin();
    void screenAdminMenu();
    void screenCashierMenu();
    void screenBuyerMenu();

    void screenInventory();
    void screenAddProduct();
    void screenSearch();
    void screenLowStock();
    void screenUsers();
    void screenReports();
    void screenCheckout();

    void addToCart();
    void viewCart();

public:
    explicit Application(SupermarketService& s) : svc(s) {}
    void run();
};

} // namespace smp
