// SupermarketService persistence implementation.
#include "SupermarketService.h"
#include "IO.h"
#include <filesystem>
#include <algorithm>

namespace smp {

namespace {
    std::vector<std::string> split(const std::string& s, char sep) {
        std::vector<std::string> out;
        std::string tok;
        std::stringstream ss(s);
        while (std::getline(ss, tok, sep)) out.push_back(tok);
        return out;
    }
}

void SupermarketService::save() const {
    namespace fs = std::filesystem;
    fs::path dir(dataDir);

    // products
    if (products.empty()) {
        std::error_code ec;
        fs::remove(dir / "products.db", ec);
    } else {
        std::vector<std::string> lines;
        for (const auto& p : products) lines.push_back(p.toRecord());
        io::writeLines(dir / "products.db", lines);
    }

    // users
    {
        std::vector<std::string> lines;
        for (const auto& u : users) lines.push_back(u->toRecord());
        io::writeLines(dir / "users.db", lines);
    }

    // invoices
    if (invoices.empty()) {
        std::error_code ec;
        fs::remove(dir / "invoices.db", ec);
    } else {
        std::vector<std::string> lines;
        for (const auto& i : invoices) lines.push_back(i.toRecord());
        io::writeLines(dir / "invoices.db", lines);
    }
}

void SupermarketService::load() {
    namespace fs = std::filesystem;
    fs::path dir(dataDir);

    products.clear();
    users.clear();
    invoices.clear();
    nextProductId = nextUserId = nextInvoiceId = 1;

    // products
    for (const auto& line : io::readLines(dir / "products.db")) {
        try {
            Product p = Product::fromRecord(line);
            nextProductId = std::max(nextProductId, p.getId() + 1);
            products.push_back(p);
        } catch (...) { /* skip corrupt line */ }
    }

    // users
    for (const auto& line : io::readLines(dir / "users.db")) {
        auto u = userFromRecord(line);
        if (u) {
            nextUserId = std::max(nextUserId, u->getId() + 1);
            users.push_back(u);
        }
    }

    // invoices — rebuild CartItem by looking up the current product for its SKU
    for (const auto& line : io::readLines(dir / "invoices.db")) {
        auto f = split(line, '|');
        if (f.size() < 7) continue;
        Invoice inv;
        try {
            inv.id          = std::stoi(f[0]);
            inv.timestamp   = f[1];
            inv.cashierName = f[2];
            inv.subtotal    = std::stod(f[3]);
            inv.discountTotal = std::stod(f[4]);
            inv.grandTotal  = std::stod(f[5]);

            for (const auto& pair : split(f[6], ';')) {
                auto pq = split(pair, ':');
                if (pq.size() != 2) continue;
                Product* p = findProductBySku(pq[0]);
                if (p) inv.items.push_back(CartItem{ *p, std::stoi(pq[1]) });
            }
            nextInvoiceId = std::max(nextInvoiceId, inv.id + 1);
            invoices.push_back(inv);
        } catch (...) { /* skip corrupt invoice */ }
    }
}

} // namespace smp
