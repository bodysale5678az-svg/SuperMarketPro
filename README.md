# 🛒 SuperMarket Pro

A **supermarket management system** written from scratch in **modern C++17** — clean OOP architecture, persistent storage, role-based access and a full point-of-sale flow.

> Built as a learning project to practise real object-oriented design in C++: abstraction, inheritance, encapsulation, composition and separation of concerns.

---

## ✨ Features

### 🧑‍💼 Role-based access
| Role | Inventory | Sell | Reports | Manage users |
|---|:---:|:---:|:---:|:---:|
| **Admin** | ✅ | ✅ | ✅ | ✅ |
| **Cashier** | ❌ | ✅ | ✅ | ❌ |
| **Buyer** | ❌ | ❌ | ❌ | ❌ |

### 📦 Inventory
- Add, update and delete products
- Update price, stock, discount and category independently
- **Search by name** (case-insensitive)
- **Low-stock report** with configurable threshold
- Validation on every setter (no negative prices or stock)

### 🧾 Point of sale
- Shopping cart with quantity merging
- Stock is validated **before** any mutation (all-or-nothing checkout)
- Printed invoice with subtotal, discounts and grand total
- Sales history with revenue and item totals

### 💾 Persistence
- Plain-text pipe-delimited records in `./data`
- Automatic save after every mutation
- Data is rebuilt on startup; corrupt lines are skipped safely

---

## 🏗️ Architecture

```
src/
 ├── main.cpp                 → entry point, first-run seeding
 ├── Application.cpp          → menus & screen flow (UI layer)
 └── SupermarketService.cpp   → persistence implementation

include/
 ├── Product.h               → product entity + validation
 ├── User.h                  → abstract User, Admin / Cashier / Buyer
 ├── Invoice.h               → immutable sale snapshot
 ├── SupermarketService.h    → business logic (single source of truth)
 ├── Application.h           → UI interface
 ├── Console.h               → console input helpers
 └── IO.h                    → file helpers
```

### Design principles applied

| Principle | Where |
|---|---|
| **Abstraction** | `User` is an abstract class with pure-virtual role capabilities |
| **Inheritance & polymorphism** | `Admin`, `Cashier` and `Buyer` override `canManageInventory()` etc. |
| **Encapsulation** | Every `Product` field is private, exposed through validated setters |
| **Composition** | `Invoice` owns `CartItem`s; the service owns products, users and invoices |
| **Separation of concerns** | UI (`Application`) never touches storage — only the service does |
| **RAII / smart pointers** | Users are held in `std::shared_ptr` |

---

## 🚀 Build & Run

### Requirements
- A C++17 compiler (MSVC, GCC or Clang)
- CMake 3.16+

### With CMake
```bash
cmake -B build
cmake --build build --config Release
./build/SuperMarketPro
```

### With MSVC directly
```bash
cl /std:c++17 /EHsc /Iinclude src\*.cpp /Fe:SuperMarketPro.exe
```

> Uses `std::filesystem`, so **C++17** is required.

---

## 🔑 Default accounts

On first run the application seeds two accounts:

| Username | Password | Role |
|---|---|---|
| `admin` | `admin123` | Administrator |
| `cashier` | `cash123` | Cashier |

You can create more users from **Admin → Manage users**.

---

## 📁 Data files

| File | Contents |
|---|---|
| `data/products.db` | Inventory records |
| `data/users.db` | Accounts |
| `data/invoices.db` | Sales history |

These files are created automatically and are excluded from version control.

---

## 🖥️ Sample session

```
╔══════════════════════════════════════════╗
║       SUPERMARKET PRO  -  LOGIN          ║
╚══════════════════════════════════════════╝

Username: admin
Password: admin123

  ✓ Welcome, Default Administrator (ADMIN)
```

---

## 🗺️ Roadmap

- [ ] Unit tests with Catch2
- [ ] Password hashing (currently plain text — teaching only)
- [ ] CSV import / export
- [ ] Product categories as an enum
- [ ] Daily / monthly sales charts

---

## 👤 Author

**Mahmoud Saleh** — [@bodysale5678az-svg](https://github.com/bodysale5678az-svg)
