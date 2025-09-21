#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <utility>
#include <limits>

// Forward declarations
class Book;
class Customer;

// ===================================================================
// Book Class
// ===================================================================
class Book{
    int ISBN;
    std::string title;
    std::string author;
    bool isAvailable;
public:
    Book(int id, std::string bookname, std::string authorname):
        ISBN(id), title(std::move(bookname)), author(std::move(authorname)), isAvailable(true) {}

    int getISBN() const { return ISBN; }
    const std::string& getTitle() const { return title; }
    const std::string& getAuthor() const { return author; }
    bool isBookAvailable() const { return isAvailable; }
    void setAvailability(bool available) { isAvailable = available; }
};

// ===================================================================
// Person Hierarchy
// ===================================================================
class Person{
protected:
    std::string name;
    int id;
public:
    Person(std::string name, int id): name(std::move(name)), id(id){}
    virtual ~Person() = default;
    const std::string& getName() const { return name; }
    int getID() const { return id; }
};

class Customer: public Person{
    std::vector<Book*> borrowedbooks;
public:
    Customer(std::string name, int id): Person(name, id){}

    const std::vector<Book*>& getBorrowedBooks() const { return borrowedbooks; }
    void borrowBook(Book* book){ borrowedbooks.push_back(book); }
    void returnBook(Book* b){
        auto it = std::find(borrowedbooks.begin(), borrowedbooks.end(), b);
        if (it != borrowedbooks.end()) {
            borrowedbooks.erase(it);
        }
    }
};

class Staff: public Person{
public:
    Staff(std::string name, int id): Person(name, id) {}
};

// ===================================================================
// Loan Class
// ===================================================================
class Loan{
    Book* book;
    Customer* customer;
    std::string dueDate;
public:
    Loan(Book* b, Customer* c, std::string date)
        : book(b), customer(c), dueDate(std::move(date)) {}

    Book* getBook() const { return book; }
    Customer* getCustomer() const { return customer; }
    const std::string& getBookName() const { return book->getTitle(); }
    const std::string& getCustomerName() const { return customer->getName(); }
    const std::string& getDueDate() const { return dueDate; }
};

// ===================================================================
// ConsoleUI Class
// ===================================================================
class ConsoleUI{
public:
    void getBookInfo(const Book& book){
        std::cout << "\n  ISBN: " << book.getISBN()
                  << "\n  Title: " << book.getTitle()
                  << "\n  Author: " << book.getAuthor()
                  << "\n  Status: " << (book.isBookAvailable() ? "Available" : "On Loan");
    }

    void getCustomerInfo(const Customer& customer){
        std::cout << "\n  ID: " << customer.getID()
                  << "\n  Name: " << customer.getName();
        const auto& books = customer.getBorrowedBooks();
        if(!books.empty()){
            std::cout << "\n  Borrowed Books:";
            for(const auto* book : books){
                std::cout << "\n    - " << book->getTitle();
            }
        } else {
            std::cout << "\n  No books currently borrowed.";
        }
    }
};

// ===================================================================
// Library Class - The Core Engine
// ===================================================================
class Library{
    std::vector<std::unique_ptr<Book>> books;
    std::vector<std::unique_ptr<Staff>> staff;
    std::vector<std::unique_ptr<Customer>> customers;
    std::vector<std::unique_ptr<Loan>> loans;

public:
    void addBook(int isbn, const std::string& title, const std::string& author) {
        books.push_back(std::make_unique<Book>(isbn, title, author));
    }

    void registerCustomer(int id, const std::string& name) {
        customers.push_back(std::make_unique<Customer>(name, id));
    }

    Book* findBookByISBN(int isbn) {
        for (const auto& book : books) { if (book->getISBN() == isbn) return book.get(); }
        return nullptr;
    }

    Customer* findCustomerByID(int id) {
        for (const auto& customer : customers){ if (customer->getID() == id) return customer.get(); }
        return nullptr;
    }
    
    const std::vector<std::unique_ptr<Book>>& getAllBooks() const { return books; }
    const std::vector<std::unique_ptr<Customer>>& getAllCustomers() const { return customers; }

    // --- REVISED Core Business Logic ---
    bool issueBook(int isbn, int customerId) {
        Book* book = findBookByISBN(isbn);
        if (!book) {
            std::cout << "Error: Book with ISBN " << isbn << " not found.\n";
            return false;
        }

        Customer* customer = findCustomerByID(customerId);
        if (!customer) {
            std::cout << "Error: Customer with ID " << customerId << " not found.\n";
            return false;
        }

        if (!book->isBookAvailable()) {
            std::cout << "Error: Book '" << book->getTitle() << "' is currently on loan.\n";
            return false;
        }

        book->setAvailability(false);
        customer->borrowBook(book);
        loans.push_back(std::make_unique<Loan>(book, customer, "2025-10-21"));
        return true;
    }

    bool returnBook(int isbn, int customerId) {
        Book* book = findBookByISBN(isbn);
        if (!book) {
            std::cout << "Error: Book with ISBN " << isbn << " not found.\n";
            return false;
        }

        Customer* customer = findCustomerByID(customerId);
        if (!customer) {
            std::cout << "Error: Customer with ID " << customerId << " not found.\n";
            return false;
        }
        
        if (book->isBookAvailable()) {
            std::cout << "Error: Book '" << book->getTitle() << "' is not currently on loan.\n";
            return false;
        }

        auto it = std::find_if(loans.begin(), loans.end(), [&](const auto& loan){
            return loan->getBook() == book && loan->getCustomer() == customer;
        });

        if (it != loans.end()) {
            customer->returnBook(book);
            book->setAvailability(true);
            loans.erase(it);
            return true;
        } else {
            std::cout << "Error: No record found of customer " << customer->getName() 
                      << " borrowing the book '" << book->getTitle() << "'.\n";
            return false;
        }
    }
};

// ===================================================================
// Main Application
// ===================================================================
void clearInputBuffer() {
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int main() {
    Library library;
    ConsoleUI ui;

    library.addBook(101, "The Hobbit", "J.R.R. Tolkien");
    library.addBook(102, "1984", "George Orwell");
    library.addBook(103, "Dune", "Frank Herbert");
    library.registerCustomer(1, "Alice");
    library.registerCustomer(2, "Bob");

    int choice = 0;
    while (choice != 9) {
        std::cout << "\n\n===== Library Management System =====\n";
        std::cout << "1. List All Books\n";
        std::cout << "2. List All Customers\n";
        std::cout << "3. Issue a Book\n";
        std::cout << "4. Return a Book\n";
        std::cout << "9. Exit\n";
        std::cout << "=====================================\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;
        
        if(std::cin.fail()){
            std::cout << "Invalid input. Please enter a number.\n";
            std::cin.clear();
            clearInputBuffer();
            choice = 0;
            continue;
        }
        clearInputBuffer();

        switch (choice) {
            case 1: {
                std::cout << "\n--- All Books in Library ---";
                for (const auto& book : library.getAllBooks()) { ui.getBookInfo(*book); }
                std::cout << "\n----------------------------\n";
                break;
            }
            case 2: {
                std::cout << "\n--- Registered Customers ---";
                for (const auto& customer : library.getAllCustomers()) { ui.getCustomerInfo(*customer); }
                std::cout << "\n---------------------------\n";
                break;
            }
            case 3: {
                int isbn, custId;
                std::cout << "Enter Book ISBN to issue: "; std::cin >> isbn;
                std::cout << "Enter Customer ID: "; std::cin >> custId;
                if(library.issueBook(isbn, custId)) {
                    std::cout << "Book issued successfully!\n";
                } // Error messages are now handled inside the function
                clearInputBuffer();
                break;
            }
            case 4: { // REVISED CASE 4
                int isbn, custId;
                std::cout << "Enter Book ISBN to return: "; std::cin >> isbn;
                std::cout << "Enter your Customer ID: "; std::cin >> custId;
                if(library.returnBook(isbn, custId)) {
                    std::cout << "Book returned successfully!\n";
                } // Error messages are now handled inside the function
                clearInputBuffer();
                break;
            }
            case 9:
                std::cout << "Exiting system. Goodbye!\n";
                break;
            default:
                std::cout << "Invalid choice. Please try again.\n";
                break;
        }
    }

    return 0;
}