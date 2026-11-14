#pragma once
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>

namespace smp {

/**
 * Product — represents an item in the supermarket inventory.
 *
 * Demonstrates encapsulation: all data members are private and only
 * accessible through validated setters and const getters.
 */
class Product {
private:
    int         id;
    std::string sku;
    std::string name;
    std::string category;
    double      price;
    int         quantity;
    double      discount;   // percentage 0..100

public:
    Product() : id(0), price(0.0), quantity(0), discount(0.0) {}

    Product(int id, const std::string& sku, const std::string& name,
            const std::string& category, double price, int quantity, double discount = 0.0)
        : id(id), sku(sku), name(name), category(category),
          price(price), quantity(quantity), discount(discount) {}

    // ---- getters ----
    int         getId()       const { return id; }
    const std::string& getSku()      const { return sku; }
    const std::string& getName()     const { return name; }
    const std::string& getCategory() const { return category; }
    double      getPrice()    const { return price; }
    int         getQuantity() const { return quantity; }
    double      getDiscount() const { return discount; }

    /** Final price after discount. */
    double finalPrice() const { return price * (1.0 - discount / 100.0); }

    bool inStock() const { return quantity > 0; }
    bool canFulfil(int qty) const { return qty > 0 && quantity >= qty; }

    // ---- setters with validation ----
    void setPrice(double p) {
        if (p < 0) throw std::invalid_argument("Price cannot be negative");
        price = p;
    }
    void setQuantity(int q) {
        if (q < 0) throw std::invalid_argument("Quantity cannot be negative");
        quantity = q;
    }
    void setDiscount(double d) {
        if (d < 0 || d > 100) throw std::invalid_argument("Discount must be 0..100");
        discount = d;
    }
    void setName(const std::string& n) {
        if (n.empty()) throw std::invalid_argument("Name cannot be empty");
        name = n;
    }
    void setCategory(const std::string& c) { category = c; }

    /** Reduce stock after a sale; throws when insufficient. */
    void reduceStock(int qty) {
        if (!canFulfil(qty))
            throw std::runtime_error("Insufficient stock for product " + name);
        quantity -= qty;
    }

    void addStock(int qty) {
        if (qty <= 0) throw std::invalid_argument("Quantity must be positive");
        quantity += qty;
    }

    /** Serialised as one pipe-delimited line for persistence. */
    std::string toRecord() const {
        std::ostringstream o;
        o << id << '|' << sku << '|' << name << '|' << category << '|'
          << std::fixed << std::setprecision(2) << price << '|'
          << quantity << '|' << std::setprecision(2) << discount;
        return o.str();
    }

    static Product fromRecord(const std::string& line) {
        std::stringstream ss(line);
        std::string tok;
        auto next = [&](std::string& out) { std::getline(ss, out, '|'); };

        std::string sId, sSku, sName, sCat, sPrice, sQty, sDisc;
        next(sId); next(sSku); next(sName); next(sCat);
        next(sPrice); next(sQty); next(sDisc);

        return Product(std::stoi(sId), sSku, sName, sCat,
                       std::stod(sPrice), std::stoi(sQty),
                       sDisc.empty() ? 0.0 : std::stod(sDisc));
    }

    friend std::ostream& operator<<(std::ostream& os, const Product& p) {
        os << std::left << std::setw(4)  << p.id
           << std::setw(10) << p.sku
           << std::setw(28) << p.name
           << std::setw(16) << p.category
           << std::right << std::setw(9) << std::fixed << std::setprecision(2) << p.price
           << std::setw(8)  << p.quantity
           << std::setw(8)  << p.discount;
        if (!p.inStock()) os << "  [OUT OF STOCK]";
        return os;
    }
};

} // namespace smp
