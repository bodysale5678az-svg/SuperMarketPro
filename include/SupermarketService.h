#pragma once
#include <vector>
#include <memory>
#include <string>
#include <functional>
#include <stdexcept>
#include <ctime>
#include <sstream>
#include <algorithm>
#include <cctype>
#include "Product.h"
#include "User.h"
#include "Invoice.h"

namespace smp {

/**
 * SupermarketService — the single source of truth for business rules.
 *
 * UI code never touches storage directly; it goes through this class.
 * This keeps the domain logic testable and independent of the interface.
 */
class SupermarketService {
private:
    std::vector<Product>          products;
    std::vector<std::shared_ptr<User>> users;
    std::vector<Invoice>          invoices;
    int nextProductId = 1;
    int nextUserId    = 1;
    int nextInvoiceId = 1;

    std::string dataDir;

    std::string nowTimestamp() const {
        std::time_t t = std::time(nullptr);
        char buf[32];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&t));
        return buf;
    }

    Product* findProductBySku(const std::string& sku) {
        for (auto& p : products)
            if (p.getSku() == sku) return &p;
        return nullptr;
    }

public:
    explicit SupermarketService(std::string dir = "data") : dataDir(std::move(dir)) {}

    // ---------- users ----------
    const std::vector<std::shared_ptr<User>>& getUsers() const { return users; }

    std::shared_ptr<User> authenticate(const std::string& username,
                                       const std::string& password) const {
        for (const auto& u : users)
            if (u->getUsername() == username && u->checkPassword(password))
                return u;
        return nullptr;
    }

    void addUser(std::shared_ptr<User> u) {
        for (const auto& x : users)
            if (x->getUsername() == u->getUsername())
                throw std::runtime_error("Username already exists");
        users.push_back(std::move(u));
    }

    std::shared_ptr<User> createUser(const std::string& type,
                                     const std::string& username,
                                     const std::string& password,
                                     const std::string& fullName) {
        auto id = nextUserId++;
        std::shared_ptr<User> u;
        if      (type == "ADMIN")   u = std::make_shared<Admin>(id, username, password, fullName);
        else if (type == "CASHIER") u = std::make_shared<Cashier>(id, username, password, fullName);
        else                        u = std::make_shared<Buyer>(id, username, password, fullName);
        addUser(u);
        return u;
    }

    bool removeUser(int id) {
        for (auto it = users.begin(); it != users.end(); ++it) {
            if ((*it)->getId() == id) {
                users.erase(it);
                return true;
            }
        }
        return false;
    }

    // ---------- products ----------
    const std::vector<Product>& getProducts() const { return products; }

    Product& addProduct(const std::string& sku, const std::string& name,
                        const std::string& category, double price,
                        int quantity, double discount = 0.0) {
        if (sku.empty())  throw std::invalid_argument("SKU required");
        if (findProductBySku(sku)) throw std::runtime_error("SKU already exists: " + sku);
        Product p(nextProductId++, sku, name, category, price, quantity, discount);
        products.push_back(p);
        return products.back();
    }

    Product* getProductBySku(const std::string& sku) { return findProductBySku(sku); }

    std::vector<Product> searchByName(const std::string& needle) const {
        std::vector<Product> out;
        std::string low;
        for (char c : needle) low += std::tolower((unsigned char)c);

        for (const auto& p : products) {
            std::string n;
            for (char c : p.getName()) n += std::tolower((unsigned char)c);
            if (n.find(low) != std::string::npos) out.push_back(p);
        }
        return out;
    }

    bool removeProduct(const std::string& sku) {
        for (auto it = products.begin(); it != products.end(); ++it) {
            if (it->getSku() == sku) {
                products.erase(it);
                return true;
            }
        }
        return false;
    }

    /** Products whose stock is at or below the threshold. */
    std::vector<Product> lowStock(int threshold = 5) const {
        std::vector<Product> out;
        for (const auto& p : products)
            if (p.getQuantity() <= threshold) out.push_back(p);
        return out;
    }

    // ---------- sales ----------
    const std::vector<Invoice>& getInvoices() const { return invoices; }

    /**
     * Finalise a sale: validates stock, reduces it, builds an invoice.
     * All-or-nothing: the cart is checked completely before mutating stock.
     */
    Invoice checkout(std::vector<CartItem> cart, const std::string& cashierName) {
        if (cart.empty()) throw std::runtime_error("Cart is empty");

        // 1) validate everything first (no partial sales)
        for (const auto& item : cart) {
            Product* p = findProductBySku(item.product.getSku());
            if (!p) throw std::runtime_error("Unknown product: " + item.product.getSku());
            if (!p->canFulfil(item.quantity))
                throw std::runtime_error("Not enough stock for: " + p->getName());
        }

        // 2) build the invoice from the CURRENT stored products (not stale copies)
        Invoice inv;
        inv.id          = nextInvoiceId++;
        inv.timestamp   = nowTimestamp();
        inv.cashierName = cashierName;

        for (auto& item : cart) {
            Product* p = findProductBySku(item.product.getSku());
            p->reduceStock(item.quantity);
            inv.items.push_back(CartItem{ *p, item.quantity });
        }

        inv.recalculate();
        invoices.push_back(inv);
        save();
        return inv;
    }

    // ---------- persistence ----------
    void save() const;
    void load();
};

} // namespace smp
