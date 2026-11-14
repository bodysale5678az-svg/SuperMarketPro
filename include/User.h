#pragma once
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <memory>
#include <vector>

namespace smp {

/**
 * User — abstract base class for every account in the system.
 *
 * Demonstrates abstraction + polymorphism: each concrete role knows
 * its own permission set and how it is displayed.
 */
class User {
protected:
    int         id;
    std::string username;
    std::string password;    // NOTE: plain-text only for a teaching project.
    std::string fullName;

public:
    User(int id, std::string username, std::string password, std::string fullName)
        : id(id), username(std::move(username)),
          password(std::move(password)), fullName(std::move(fullName)) {}

    virtual ~User() = default;

    int getId() const { return id; }
    const std::string& getUsername() const { return username; }
    const std::string& getFullName() const { return fullName; }

    bool checkPassword(const std::string& p) const { return p == password; }

    // ---- pure virtual: every role must answer ----
    virtual std::string role() const = 0;
    virtual bool canManageInventory() const = 0;
    virtual bool canViewReports() const = 0;

    /** Serialise as: type|id|username|password|fullName */
    static std::vector<std::string> splitRecord(const std::string& line) {
        std::vector<std::string> parts;
        std::stringstream ss(line);
        std::string tok;
        while (std::getline(ss, tok, '|')) parts.push_back(tok);
        return parts;
    }

    virtual std::string toRecord() const {
        return role() + "|" + std::to_string(id) + "|" + username + "|" +
               password + "|" + fullName;
    }

    friend std::ostream& operator<<(std::ostream& os, const User& u) {
        os << std::left << std::setw(6) << u.id
           << std::setw(14) << u.role()
           << std::setw(18) << u.username
           << u.fullName;
        return os;
    }
};

/** Administrator: full control over inventory, users and reports. */
class Admin : public User {
public:
    using User::User;
    std::string role() const override { return "ADMIN"; }
    bool canManageInventory() const override { return true; }
    bool canViewReports()     const override { return true; }
};

/** Cashier: can sell products and print bills, but cannot edit inventory. */
class Cashier : public User {
public:
    using User::User;
    std::string role() const override { return "CASHIER"; }
    bool canManageInventory() const override { return false; }
    bool canViewReports()     const override { return true; }
};

/** Buyer: browse and purchase only. */
class Buyer : public User {
public:
    using User::User;
    std::string role() const override { return "BUYER"; }
    bool canManageInventory() const override { return false; }
    bool canViewReports()     const override { return false; }
};

/** Factory: rebuild the right concrete type from a saved record. */
inline std::shared_ptr<User> userFromRecord(const std::string& line) {
    auto f = User::splitRecord(line);
    if (f.size() < 5) return nullptr;

    const std::string& type = f[0];
    int id = std::stoi(f[1]);
    const std::string& un = f[2];
    const std::string& pw = f[3];
    const std::string& nm = f[4];

    if (type == "ADMIN")   return std::make_shared<Admin>(id, un, pw, nm);
    if (type == "CASHIER") return std::make_shared<Cashier>(id, un, pw, nm);
    if (type == "BUYER")   return std::make_shared<Buyer>(id, un, pw, nm);
    return nullptr;
}

} // namespace smp
