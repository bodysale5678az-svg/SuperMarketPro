#include "Application.h"
#include <iomanip>
#include <sstream>

namespace smp {

// ============================ LOGIN ============================
void Application::screenLogin() {
    currentUser.reset();

    while (!currentUser) {
        Console::clearScreen();
        std::cout << "╔══════════════════════════════════════════╗\n";
        std::cout << "║       SUPERMARKET PRO  -  LOGIN          ║\n";
        std::cout << "╚══════════════════════════════════════════╝\n\n";

        std::string user = Console::readLine("Username: ");
        std::string pass = Console::readLine("Password: ");

        currentUser = svc.authenticate(user, pass);
        if (!currentUser) {
            std::cout << "\n  ✗ Invalid credentials. Try again.\n";
            Console::pause();
        }
    }

    std::cout << "\n  ✓ Welcome, " << currentUser->getFullName()
              << " (" << currentUser->role() << ")\n";
    Console::pause();
}

// ============================ ROUTER ============================
void Application::run() {
    while (true) {
        screenLogin();
        if (!currentUser) continue;

        if      (currentUser->role() == "ADMIN")   screenAdminMenu();
        else if (currentUser->role() == "CASHIER") screenCashierMenu();
        else                                       screenBuyerMenu();
    }
}

// ============================ ADMIN ============================
void Application::screenAdminMenu() {
    while (true) {
        Console::clearScreen();
        std::cout << "=== ADMIN PANEL — " << currentUser->getFullName() << " ===\n";
        std::cout << "1. Inventory (add / edit / delete)\n";
        std::cout << "2. Search products\n";
        std::cout << "3. Low-stock report\n";
        std::cout << "4. Manage users\n";
        std::cout << "5. Sales reports\n";
        std::cout << "0. Logout\n";

        int c = Console::readInt("\nChoice: ");
        switch (c) {
            case 1: screenInventory();   break;
            case 2: screenSearch();      break;
            case 3: screenLowStock();    break;
            case 4: screenUsers();       break;
            case 5: screenReports();     break;
            case 0: return;
            default: break;
        }
    }
}

// ============================ INVENTORY ============================
void Application::screenInventory() {
    while (true) {
        Console::clearScreen();
        Console::header("INVENTORY");
        printProductTable(svc.getProducts());

        std::cout << "\n1. Add product\n2. Update price / stock\n3. Delete product\n0. Back\n";
        int c = Console::readInt("Choice: ");

        if (c == 0) return;

        if (c == 1) {
            screenAddProduct();
        } else if (c == 2) {
            std::string sku = Console::readLine("SKU to update: ");
            Product* p = svc.getProductBySku(sku);
            if (!p) { std::cout << "  ! Not found\n"; Console::pause(); continue; }

            std::cout << "1. Price  2. Stock  3. Discount  4. Category\n";
            int f = Console::readInt("Field: ");
            try {
                if (f == 1)      p->setPrice(Console::readDouble("New price: "));
                else if (f == 2) p->setQuantity(Console::readInt("New quantity: "));
                else if (f == 3) p->setDiscount(Console::readDouble("New discount %: "));
                else if (f == 4) p->setCategory(Console::readLine("New category: "));
                svc.save();
                std::cout << "  ✓ Updated\n";
            } catch (const std::exception& e) {
                std::cout << "  ! " << e.what() << "\n";
            }
            Console::pause();
        } else if (c == 3) {
            std::string sku = Console::readLine("SKU to delete: ");
            if (Console::confirm("Delete " + sku + "?")) {
                if (svc.removeProduct(sku)) { svc.save(); std::cout << "  ✓ Deleted\n"; }
                else std::cout << "  ! Not found\n";
            }
            Console::pause();
        }
    }
}

void Application::screenAddProduct() {
    Console::clearScreen();
    Console::header("ADD PRODUCT");
    try {
        std::string sku  = Console::readLine("SKU       : ");
        std::string name = Console::readLine("Name      : ");
        std::string cat  = Console::readLine("Category  : ");
        double price     = Console::readDouble("Price     : ");
        int qty          = Console::readInt("Quantity  : ");
        double disc      = Console::readDouble("Discount% : ");

        svc.addProduct(sku, name, cat, price, qty, disc);
        svc.save();
        std::cout << "  ✓ Product added\n";
    } catch (const std::exception& e) {
        std::cout << "  ! " << e.what() << "\n";
    }
    Console::pause();
}

void Application::screenSearch() {
    Console::clearScreen();
    Console::header("SEARCH");
    std::string q = Console::readLine("Product name contains: ");
    printProductTable(svc.searchByName(q));
    Console::pause();
}

void Application::screenLowStock() {
    Console::clearScreen();
    Console::header("LOW STOCK (<= threshold)");
    int th = Console::readInt("Threshold (default 5): ");
    if (th < 0) th = 5;
    printProductTable(svc.lowStock(th));
    Console::pause();
}

// ============================ USERS ============================
void Application::screenUsers() {
    while (true) {
        Console::clearScreen();
        Console::header("USERS");
        std::cout << std::left << std::setw(6) << "ID"
                  << std::setw(14) << "Role"
                  << std::setw(18) << "Username" << "Full name\n";
        Console::separator(60);
        for (const auto& u : svc.getUsers()) std::cout << *u << "\n";

        std::cout << "\n1. Add user\n2. Remove user\n0. Back\n";
        int c = Console::readInt("Choice: ");
        if (c == 0) return;

        if (c == 1) {
            std::string type = Console::readLine("Type (ADMIN/CASHIER/BUYER): ");
            std::string un   = Console::readLine("Username: ");
            std::string pw   = Console::readLine("Password: ");
            std::string nm   = Console::readLine("Full name: ");
            try {
                svc.createUser(type, un, pw, nm);
                svc.save();
                std::cout << "  ✓ User created\n";
            } catch (const std::exception& e) {
                std::cout << "  ! " << e.what() << "\n";
            }
            Console::pause();
        } else if (c == 2) {
            int id = Console::readInt("User ID to remove: ");
            if (id == currentUser->getId()) {
                std::cout << "  ! Cannot remove yourself\n";
            } else if (svc.removeUser(id)) {
                svc.save();
                std::cout << "  ✓ Removed\n";
            } else {
                std::cout << "  ! Not found\n";
            }
            Console::pause();
        }
    }
}

// ============================ REPORTS ============================
void Application::screenReports() {
    Console::clearScreen();
    Console::header("SALES REPORTS");

    double revenue = 0.0;
    int itemCount = 0;
    for (const auto& inv : svc.getInvoices()) {
        revenue += inv.grandTotal;
        for (const auto& it : inv.items) itemCount += it.quantity;
    }

    std::cout << "Invoices issued : " << svc.getInvoices().size() << "\n";
    std::cout << "Items sold      : " << itemCount << "\n";
    std::cout << "Total revenue   : " << std::fixed << std::setprecision(2) << revenue << "\n\n";

    if (!svc.getInvoices().empty()) {
        Console::separator();
        std::cout << std::left << std::setw(6) << "ID"
                  << std::setw(21) << "Date"
                  << std::setw(14) << "Cashier"
                  << std::right << std::setw(10) << "Total\n";
        Console::separator();
        for (const auto& inv : svc.getInvoices()) {
            std::cout << std::left << std::setw(6) << inv.id
                      << std::setw(21) << inv.timestamp
                      << std::setw(14) << inv.cashierName
                      << std::right << std::setw(10) << std::fixed << std::setprecision(2)
                      << inv.grandTotal << "\n";
        }
    }
    Console::pause();
}

// ============================ CASHIER ============================
void Application::screenCashierMenu() {
    cart.clear();
    while (true) {
        Console::clearScreen();
        std::cout << "=== CASHIER PANEL — " << currentUser->getFullName() << " ===\n";
        std::cout << "Cart items: " << cart.size() << "\n\n";
        std::cout << "1. List products\n";
        std::cout << "2. Add to cart\n";
        std::cout << "3. View cart\n";
        std::cout << "4. Checkout\n";
        std::cout << "5. Sales reports\n";
        std::cout << "0. Logout\n";

        int c = Console::readInt("\nChoice: ");
        switch (c) {
            case 1: {
                Console::clearScreen();
                Console::header("PRODUCTS");
                printProductTable(svc.getProducts());
                Console::pause();
                break;
            }
            case 2: addToCart(); break;
            case 3: viewCart();  break;
            case 4: screenCheckout(); break;
            case 5: screenReports(); break;
            case 0: return;
            default: break;
        }
    }
}

void Application::addToCart() {
    Console::clearScreen();
    Console::header("ADD TO CART");
    std::string sku = Console::readLine("SKU: ");
    Product* p = svc.getProductBySku(sku);
    if (!p) { std::cout << "  ! Not found\n"; Console::pause(); return; }

    int qty = Console::readInt("Quantity: ");
    if (!p->canFulfil(qty)) {
        std::cout << "  ! Insufficient stock (available: " << p->getQuantity() << ")\n";
        Console::pause();
        return;
    }

    for (auto& it : cart) {
        if (it.product.getSku() == sku) {
            if (!p->canFulfil(it.quantity + qty)) {
                std::cout << "  ! Total exceeds stock\n";
                Console::pause();
                return;
            }
            it.quantity += qty;
            std::cout << "  ✓ Updated cart\n";
            Console::pause();
            return;
        }
    }

    cart.push_back(CartItem{ *p, qty });
    std::cout << "  ✓ Added to cart\n";
    Console::pause();
}

void Application::viewCart() {
    Console::clearScreen();
    Console::header("CART");
    if (cart.empty()) { std::cout << "  (empty)\n"; Console::pause(); return; }

    double total = 0;
    for (const auto& it : cart) {
        std::cout << std::left << std::setw(28) << it.product.getName()
                  << " x" << std::setw(4) << it.quantity
                  << std::right << std::setw(10) << std::fixed << std::setprecision(2)
                  << it.lineTotal() << "\n";
        total += it.lineTotal();
    }
    Console::separator();
    std::cout << std::right << std::setw(42) << "TOTAL: " << std::setw(10) << total << "\n";
    Console::pause();
}

void Application::screenCheckout() {
    if (cart.empty()) {
        std::cout << "  ! Cart is empty\n";
        Console::pause();
        return;
    }
    try {
        Invoice inv = svc.checkout(cart, currentUser->getFullName());
        cart.clear();
        Console::clearScreen();
        inv.print(std::cout);
        Console::pause();
    } catch (const std::exception& e) {
        std::cout << "  ! Checkout failed: " << e.what() << "\n";
        Console::pause();
    }
}

// ============================ BUYER ============================
void Application::screenBuyerMenu() {
    while (true) {
        Console::clearScreen();
        std::cout << "=== STORE — " << currentUser->getFullName() << " ===\n\n";
        std::cout << "1. Browse products\n";
        std::cout << "2. Search\n";
        std::cout << "0. Logout\n";

        int c = Console::readInt("\nChoice: ");
        switch (c) {
            case 1:
                Console::clearScreen();
                Console::header("PRODUCTS");
                printProductTable(svc.getProducts());
                Console::pause();
                break;
            case 2: screenSearch(); break;
            case 0: return;
            default: break;
        }
    }
}

} // namespace smp
