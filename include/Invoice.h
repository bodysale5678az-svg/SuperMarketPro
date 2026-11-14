#pragma once
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <iomanip>
#include "Product.h"

namespace smp {

/** One line of a cart / invoice. */
struct CartItem {
    Product product;
    int     quantity;

    double lineTotal() const { return product.finalPrice() * quantity; }
};

/**
 * Invoice — an immutable snapshot of a completed sale.
 *
 * Shows composition: an invoice owns its line items and computes the
 * totals itself, so historical bills never change when prices change.
 */
class Invoice {
public:
    int                       id;
    std::string               timestamp;
    std::string               cashierName;
    std::vector<CartItem>     items;
    double                    subtotal;
    double                    discountTotal;
    double                    grandTotal;

    Invoice() : id(0), subtotal(0), discountTotal(0), grandTotal(0) {}

    /** Recompute totals from the stored items. */
    void recalculate() {
        subtotal = discountTotal = grandTotal = 0.0;
        for (const auto& it : items) {
            double gross = it.product.getPrice() * it.quantity;
            subtotal      += gross;
            discountTotal += gross - it.lineTotal();
            grandTotal    += it.lineTotal();
        }
    }

    void print(std::ostream& os) const {
        os << "\n================= INVOICE #" << id << " =================\n";
        os << "Date    : " << timestamp << "\n";
        os << "Cashier : " << cashierName << "\n";
        os << "---------------------------------------------------------\n";
        os << std::left << std::setw(26) << "Item"
           << std::right << std::setw(6)  << "Qty"
           << std::setw(11) << "Unit"
           << std::setw(12) << "Total" << "\n";
        os << "---------------------------------------------------------\n";
        for (const auto& it : items) {
            os << std::left << std::setw(26) << it.product.getName().substr(0, 25)
               << std::right << std::setw(6) << it.quantity
               << std::setw(11) << std::fixed << std::setprecision(2) << it.product.finalPrice()
               << std::setw(12) << it.lineTotal() << "\n";
        }
        os << "---------------------------------------------------------\n";
        os << std::right << std::setw(45) << "Subtotal : " << std::setw(10) << subtotal << "\n";
        os << std::right << std::setw(45) << "Discount : " << std::setw(10) << discountTotal << "\n";
        os << std::right << std::setw(45) << "TOTAL    : " << std::setw(10) << grandTotal << "\n";
        os << "=========================================================\n";
    }

    /** Persisted as: id|timestamp|cashier|subtotal|discount|total|sku:qty;sku:qty */
    std::string toRecord() const {
        std::ostringstream o;
        o << id << '|' << timestamp << '|' << cashierName << '|'
          << std::fixed << std::setprecision(2)
          << subtotal << '|' << discountTotal << '|' << grandTotal << '|';
        for (size_t i = 0; i < items.size(); ++i) {
            if (i) o << ';';
            o << items[i].product.getSku() << ':' << items[i].quantity;
        }
        return o.str();
    }
};

} // namespace smp
