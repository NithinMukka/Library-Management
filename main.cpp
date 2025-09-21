#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <utility>

class Book{
    int ISBN;
    std::string title;
    std::string author;
    bool isAvailable;
    public:
    Book(int id, std::string bookname, std::string authorname): ISBN(std::move(id)), title(std::move(bookname)), author(std::move(authorname)) {}

    const int& getISBN(){
        return ISBN;
    }

    const std::string& getTitle(){
        return title;
    }

    const std::string& getAuthor(){
        return author;
    }

    const bool& isBookAvailable(){
        return isAvailable;
    }
};

class Person{
    std::string name;
    int id;
    public:
    Person(std::string name, int id): name(std::move(name)), id(std::move(id)){}

    const std::string& getName(){
        return name;
    }

    const int& getID(){
        return id;
    }
};

class Customer: public Person{
    std::vector<std::unique_ptr<Book>> borrowedbooks; 
    public:
    Customer(std::string name, int id): Person(name, id){}

    const std::vector<std::unique_ptr<Book>>& getBorrowedBooks(){
        return borrowedbooks;
    }

    bool borrowBook(std::unique_ptr<Book> book){
        if(book->isBookAvailable()){
            borrowedbooks.push_back(book);
            return true;
        }
        return false;
    }

    std::unique_ptr<Book> returnBook(std::unique_ptr<Book> b){
        auto it = std::find_if(borrowedbooks.begin(), borrowedbooks.end(),
            [&](const std::unique_ptr<Book>& book) {
                std::string lowerBookName = book->getTitle();
                std::transform(lowerBookName.begin(), lowerBookName.end(), lowerBookName.begin(), ::tolower);
                return lowerBookName == b->getTitle();
            });

        if (it != borrowedbooks.end()) {
            std::unique_ptr<Book> foundItem = std::move(*it);
            borrowedbooks.erase(it);
            return foundItem;
        }
        return nullptr;
    }
};

class Staff: public Person{
    public:
    Staff(std::string name, int id): Person(name, id) {}
};

class Loan{
    Book* book;
    Customer* customer;
    std::string dueDate;
    public:
    Loan(std::unique_ptr<Book> b, std::unique_ptr<Customer> c, std::string date): dueDate(std::move(date)){
        book = b.get();
        customer = c.get();
    }

    const std::string& getBookName(){
        return book->getTitle();
    }

    const std::string& getCustomerName(){
        return customer->getName();
    }

    const std::string& getDueDate(){
        return dueDate;
    }
};

class ConsoleUI{
    public:
    void getBookInfo(std::unique_ptr<Book> book){
        std::cout << "\nBook ID: " << book->getISBN();
        std::cout << "\nBook Title: " << book->getTitle();
        std::cout << "\nBook Author: " << book->getAuthor();
        std::cout << "\nBook Availibility: " << (book->isBookAvailable() == true) ? "Available " : "Not Available";
    }

    void getCustomerInfo(std::unique_ptr<Customer> customer){
        std::cout << "\nCustomer ID: " << customer->getID();
        std::cout << "\nCustomer currently borrowed books are: ";
        if(customer->getBorrowedBooks().size() > 0){
            for(const auto &book: customer->getBorrowedBooks()){
                std::cout << book->getTitle() << " ";
            }
        }
        else std::cout << "\nNone";
    }

    void getLoanInfo(std::unique_ptr<Loan> loan){
        std::cout << "\nBorrowed book: " << loan->getBookName();
        std::cout << "\nBorrowed customer: " << loan->getCustomerName();
        std::cout << "\nDue date " << loan->getDueDate();
    }

    void getStaffInfo(std::unique_ptr<Staff> staff){
        std::cout << "\nStaff ID: " << staff->getID();
        std::cout << "\nStaff name: " << staff->getName();
    }
};

class Library{
    std::vector<std::unique_ptr<Book>> books;
    std::vector<std::unique_ptr<Staff>> staff;
    std::vector<std::unique_ptr<Customer>> customers;
    std::vector<std::unique_ptr<Loan>> loans;
    ConsoleUI view;
    public:
    
};