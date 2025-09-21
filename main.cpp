#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <utility>
#include <limits> // Required for numeric_limits

// Forward declarations to resolve circular dependencies if needed later
class Book;
class Customer;
class Staff;
class Loan;
class Library;

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

    int getISBN() const {
        return ISBN;
    }

    const std::string& getTitle() const {
        return title;
    }

    const std::string& getAuthor() const {
        return author;
    }

    bool isBookAvailable() const {
        return isAvailable;
    }

    void setAvailability(bool available) {
        isAvailable = available;
    }
};

// ===================================================================
// Person Hierarchy
// ===================================================================
class Person{
protected: // Use protected so derived classes can access if needed, but it's better to use public getters
    std::string name;
    int id;
public:
    Person(std::string name, int id): name(std::move(name)), id(id){}
    virtual ~Person() = default; // Virtual destructor for base class

    const std::string& getName() const {
        return name;
    }

    int getID() const {
        return id;
    }
};

class Customer: public Person{
    std::vector<Book*> borrowedbooks;
public:
    Customer(std::string name, int id): Person(name, id){}

    const std::vector<Book*>& getBorrowedBooks() const {
        return borrowedbooks;
    }

    // Simplified borrowBook as per our discussion
    void borrowBook(Book* book){
        borrowedbooks.push_back(book);
    }

    // Implementation for returning a book
    void returnBook(Book* b){
        // Find the book in the vector and remove it
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

    const std::string& getBookName() const {
        return book->getTitle();
    }

    const std::string& getCustomerName() const {
        return customer->getName();
    }

    const std::string& getDueDate() const {
        return dueDate;
    }
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

    void getLoanInfo(const Loan& loan){
        std::cout << "\n  Book: " << loan.getBookName()
                  << "\n  Customer: " << loan.getCustomerName()
                  << "\n  Due Date: " << loan.getDueDate();
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
    // --- Data Management ---
    void addBook(int isbn, const std::string& title, const std::string& author) {
        books.push_back(std::make_unique<Book>(isbn, title, author));
    }

    void registerCustomer(int id, const std::string& name) {
        customers.push_back(std::make_unique<Customer>(name, id));
    }

    // --- Finders ---
    Book* findBookByISBN(int isbn) {
        for (const auto& book : books) {
            if (book->getISBN() == isbn) {
                return book.get();
            }
        }
        return nullptr;
    }

    Customer* findCustomerByID(int id) {
        for (const auto& customer : customers){
            if (customer->getID() == id) {
                return customer.get();
            }
        }
        return nullptr;
    }
    
    // --- Getters for UI ---
    const std::vector<std::unique_ptr<Book>>& getAllBooks() const { return books; }
    const std::vector<std::unique_ptr<Customer>>& getAllCustomers() const { return customers; }


    // --- Core Business Logic ---
    bool issueBook(int isbn, int customerId) {
        Book* book = findBookByISBN(isbn);
        Customer* customer = findCustomerByID(customerId);

        if (book && customer && book->isBookAvailable()) {
            book->setAvailability(false);
            customer->borrowBook(book);
            // In a real app, you'd calculate a proper due date
            loans.push_back(std::make_unique<Loan>(book, customer, "2025-10-21"));
            return true;
        }
        return false;
    }

    bool returnBook(int isbn) {
        Book* book = findBookByISBN(isbn);

        // Check 1: Book must exist and must be currently on loan.
        if (!book || book->isBookAvailable()) {
            return false;
        }

        // Find the loan record associated with this book.
        Loan* loan_to_remove = nullptr;
        auto it = std::find_if(loans.begin(), loans.end(), [&](const auto& loan){
            return loan->getBook() == book;
        });

        if (it != loans.end()) {
            // Get the customer from the loan record
            Customer* customer = (*it)->getCustomer();

            // Execute the return process
            customer->returnBook(book);
            book->setAvailability(true);
            
            // Remove the loan from the main ledger
            loans.erase(it);

            return true;
        }
        
        // If no loan record was found (data inconsistency), fail the operation.
        return false;
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

    // --- Pre-populate data for testing ---
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
        
        // Input validation
        if(std::cin.fail()){
            std::cout << "Invalid input. Please enter a number.\n";
            std::cin.clear();
            clearInputBuffer();
            choice = 0; // Reset choice to re-enter loop
            continue;
        }

        clearInputBuffer(); // Consume the rest of the line

        switch (choice) {
            case 1: {
                std::cout << "\n--- All Books in Library ---";
                for (const auto& book : library.getAllBooks()) {
                    ui.getBookInfo(*book);
                }
                std::cout << "\n----------------------------\n";
                break;
            }
            case 2: {
                std::cout << "\n--- Registered Customers ---";
                for (const auto& customer : library.getAllCustomers()) {
                    ui.getCustomerInfo(*customer);
                }
                std::cout << "\n---------------------------\n";
                break;
            }
            case 3: {
                int isbn, custId;
                std::cout << "Enter Book ISBN to issue: ";
                std::cin >> isbn;
                std::cout << "Enter Customer ID: ";
                std::cin >> custId;
                if(library.issueBook(isbn, custId)) {
                    std::cout << "Book issued successfully!\n";
                } else {
                    std::cout << "Failed to issue book. Check ISBN, Customer ID, or book availability.\n";
                }
                clearInputBuffer();
                break;
            }
            case 4: {
                int isbn;
                std::cout << "Enter Book ISBN to return: ";
                std::cin >> isbn;
                if(library.returnBook(isbn)) {
                    std::cout << "Book returned successfully!\n";
                } else {
                    std::cout << "Failed to return book. Check if the ISBN is correct and the book is on loan.\n";
                }
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