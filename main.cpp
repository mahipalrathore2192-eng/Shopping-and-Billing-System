#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <fstream>
#include <ctime>

using namespace std;


// 1. ABSTRACTION & POLYMORPHISM (Base Class)

class Product {
protected:
    int id;
    string name;
    double price;
    int stock;

public:
    Product(int id, string name, double price, int stock)
        : id(id), name(name), price(price), stock(stock) {}

    virtual ~Product() {}

    // Pure Virtual Function for Polymorphic Tax Calculation (GST)
    virtual double getTaxRate() const = 0;
    virtual string getCategory() const = 0;

    // Encapsulation: Getters & Setters
    int getId() const { return id; }
    string getName() const { return name; }
    double getPrice() const { return price; }
    int getStock() const { return stock; }

    void reduceStock(int qty) {
        if (qty <= stock) stock -= qty;
    }

    void restoreStock(int qty) {
        stock += qty;
    }

    void updateStock(int newStock) {
        stock = newStock;
    }

    virtual void display() const {
        cout << left << setw(6) << id
             << setw(22) << name
             << setw(14) << getCategory()
             << "Rs. " << setw(10) << fixed << setprecision(2) << price
             << setw(8) << stock
             << (getTaxRate() * 100) << "%" << endl;
    }
};


// 2. INHERITANCE (Derived Product Types)

class Electronics : public Product {
public:
    Electronics(int id, string name, double price, int stock)
        : Product(id, name, price, stock) {}

    double getTaxRate() const override { return 0.18; } // 18% GST
    string getCategory() const override { return "Electronics"; }
};

class Grocery : public Product {
public:
    Grocery(int id, string name, double price, int stock)
        : Product(id, name, price, stock) {}

    double getTaxRate() const override { return 0.05; } // 5% GST
    string getCategory() const override { return "Grocery"; }
};

// 3. CART ITEM & OPERATOR OVERLOADING

struct CartItem {
    Product* product;
    int quantity;

    double getSubtotal() const {
        return product->getPrice() * quantity;
    }

    double getTaxAmount() const {
        return getSubtotal() * product->getTaxRate();
    }

    // Operator Overloading (+) to easily merge quantities of the same item
    CartItem& operator+(int extraQty) {
        this->quantity += extraQty;
        return *this;
    }
};


// MODULE 1: PRODUCT CATALOGUE MANAGER

class Catalogue {
private:
    vector<Product*> products;

public:
    Catalogue() {
        // Pre-loaded sample inventory
        products.push_back(new Electronics(101, "ANC Headphones", 2499.00, 15));
        products.push_back(new Electronics(102, "Mechanical Keyboard", 3200.00, 10));
        products.push_back(new Grocery(201, "Whey Protein 1kg", 1850.00, 20));
        products.push_back(new Grocery(202, "Rolled Oats 1kg", 220.00, 40));
    }

    ~Catalogue() {
        for (Product* p : products) {
            delete p;
        }
    }

    void addProduct() {
        int id, stock, type;
        string name;
        double price;

        cout << "\nSelect Category (1. Electronics [18% GST] | 2. Grocery [5% GST]): ";
        cin >> type;
        cout << "Enter Product ID: ";
        cin >> id;

        if (findProduct(id) != nullptr) {
            cout << "[!] Product ID already exists!\n";
            return;
        }

        cin.ignore();
        cout << "Enter Product Name: ";
        getline(cin, name);
        cout << "Enter Price (Rs.): ";
        cin >> price;
        cout << "Enter Initial Stock: ";
        cin >> stock;

        if (type == 1)
            products.push_back(new Electronics(id, name, price, stock));
        else
            products.push_back(new Grocery(id, name, price, stock));

        cout << "[+] Product added to catalogue successfully!\n";
    }

    void displayCatalogue() const {
        cout << "\n-----------------------------------------------------------------------\n";
        cout << left << setw(6) << "ID" << setw(22) << "Name" << setw(14) << "Category"
             << setw(14) << "Price" << setw(8) << "Stock" << "GST" << endl;
        cout << "-----------------------------------------------------------------------\n";
        for (const Product* p : products) {
            p->display(); // Polymorphic call
        }
        cout << "-----------------------------------------------------------------------\n";
    }

    Product* findProduct(int id) {
        for (Product* p : products) {
            if (p->getId() == id) return p;
        }
        return nullptr;
    }
};


// MODULE 2: SHOPPING CART MANAGER

class ShoppingCart {
private:
    vector<CartItem> items;

public:
    void addItem(Catalogue& catalogue) {
        int id, qty;
        catalogue.displayCatalogue();
        cout << "Enter Product ID to add: ";
        cin >> id;

        Product* prod = catalogue.findProduct(id);
        if (!prod) {
            cout << "[!] Invalid Product ID!\n";
            return;
        }

        cout << "Enter Quantity: ";
        cin >> qty;

        if (qty <= 0 || qty > prod->getStock()) {
            cout << "[!] Insufficient stock! Available: " << prod->getStock() << "\n";
            return;
        }

        prod->reduceStock(qty);

        // Check if already in cart
        for (auto& item : items) {
            if (item.product->getId() == id) {
                item + qty; // Using overloaded '+' operator
                cout << "[+] Updated quantity in cart!\n";
                return;
            }
        }

        items.push_back({prod, qty});
        cout << "[+] Item added to cart!\n";
    }

    void removeItem() {
        if (items.empty()) {
            cout << "\n[!] Cart is already empty!\n";
            return;
        }

        viewCart();
        int id;
        cout << "Enter Product ID to remove from cart: ";
        cin >> id;

        for (size_t i = 0; i < items.size(); ++i) {
            if (items[i].product->getId() == id) {
                items[i].product->restoreStock(items[i].quantity);
                items.erase(items.begin() + i);
                cout << "[-] Item removed from cart and stock restored!\n";
                return;
            }
        }
        cout << "[!] Item not found in cart!\n";
    }

    void viewCart() const {
        if (items.empty()) {
            cout << "\n[!] Your cart is empty.\n";
            return;
        }
        cout << "\n==================== YOUR SHOPPING CART ====================\n";
        cout << left << setw(6) << "ID" << setw(22) << "Item" << setw(8) << "Qty"
             << setw(14) << "Unit Price" << "Subtotal\n";
        cout << "------------------------------------------------------------\n";
        for (const auto& item : items) {
            cout << left << setw(6) << item.product->getId()
                 << setw(22) << item.product->getName()
                 << setw(8) << item.quantity
                 << "Rs. " << setw(10) << fixed << setprecision(2) << item.product->getPrice()
                 << "Rs. " << item.getSubtotal() << endl;
        }
        cout << "============================================================\n";
    }

    bool isEmpty() const { return items.empty(); }
    const vector<CartItem>& getItems() const { return items; }
    void clearCart() { items.clear(); }
};


// MODULE 3 & 4: BILLING & INVOICE GENERATOR

class BillingSystem {
public:
    static void generateInvoice(ShoppingCart& cart, string customerName, string couponCode) {
        if (cart.isEmpty()) {
            cout << "\n[!] Cannot generate invoice. Cart is empty!\n";
            return;
        }

        double subtotal = 0.0;
        double totalTax = 0.0;

        for (const auto& item : cart.getItems()) {
            subtotal += item.getSubtotal();
            totalTax += item.getTaxAmount();
        }

        // Module 3: Discount Logic
        double discount = 0.0;
        if (couponCode == "SAVE10" || subtotal >= 5000.0) {
            discount = subtotal * 0.10; // 10% Discount
        }

        double grandTotal = (subtotal - discount) + totalTax;

        // Get Current Timestamp
        time_t now = time(0);
        char* dt = ctime(&now);

        // Module 4: Print Invoice to Console AND Save to File
        ofstream file("Invoice.txt");
        auto printBoth = [&](ostream& out) {
            out << "\n============================================================\n";
            out << "                   JAIPUR SMART MART - INVOICE              \n";
            out << "============================================================\n";
            out << "Customer Name : " << customerName << "\n";
            out << "Date & Time   : " << dt;
            out << "------------------------------------------------------------\n";
            out << left << setw(20) << "Item" << setw(6) << "Qty"
                << setw(12) << "Rate" << setw(10) << "GST" << "Amount\n";
            out << "------------------------------------------------------------\n";

            for (const auto& item : cart.getItems()) {
                out << left << setw(20) << item.product->getName()
                    << setw(6) << item.quantity
                    << setw(12) << fixed << setprecision(2) << item.product->getPrice()
                    << setw(10) << item.getTaxAmount()
                    << item.getSubtotal() << "\n";
            }

            out << "------------------------------------------------------------\n";
            out << right << setw(46) << "Subtotal : Rs. " << subtotal << "\n";
            out << right << setw(46) << "Discount : -Rs. " << discount << "\n";
            out << right << setw(46) << "Total GST : +Rs. " << totalTax << "\n";
            out << "============================================================\n";
            out << right << setw(46) << "GRAND TOTAL : Rs. " << grandTotal << "\n";
            out << "============================================================\n";
            out << "            Thank you for shopping with us!                 \n";
        };

        printBoth(cout);
        if (file.is_open()) {
            printBoth(file);
            file.close();
            cout << "\n[Success] Invoice saved to 'Invoice.txt' for printing!\n";
        }

        cart.clearCart();
    }
};

// MAIN MENU DRIVER

int main() {
    Catalogue storeCatalogue;
    ShoppingCart userCart;
    int choice;

    while (true) {
        cout << "\n=== SHOPPING & BILLING SYSTEM (OOPs LAB) ===\n";
        cout << "1. View Product Catalogue (Module 1)\n";
        cout << "2. Add New Product to Catalogue (Module 1)\n";
        cout << "3. Add Item to Cart (Module 2)\n";
        cout << "4. Remove Item from Cart (Module 2)\n";
        cout << "5. View Cart (Module 2)\n";
        cout << "6. Checkout & Generate Invoice (Module 3 & 4)\n";
        cout << "7. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                storeCatalogue.displayCatalogue();
                break;
            case 2:
                storeCatalogue.addProduct();
                break;
            case 3:
                userCart.addItem(storeCatalogue);
                break;
            case 4:
                userCart.removeItem();
                break;
            case 5:
                userCart.viewCart();
                break;
            case 6: {
                if (userCart.isEmpty()) {
                    cout << "\n[!] Add items to cart before checking out!\n";
                    break;
                }
                string name, coupon;
                cin.ignore();
                cout << "Enter Customer Name: ";
                getline(cin, name);
                cout << "Enter Promo Code (Type 'SAVE10' for 10% off, or 'NONE'): ";
                cin >> coupon;
                BillingSystem::generateInvoice(userCart, name, coupon);
                break;
            }
            case 7:
                cout << "Exiting system. Good luck with your viva!\n";
                return 0;
            default:
                cout << "[!] Invalid choice. Try again.\n";
        }
    }
}