# Library Management System

A simple, console-based Library Management System written in modern C++. This project is designed to demonstrate key Object-Oriented Programming (OOP) principles in a practical and understandable way. All interactions are handled through the command line.

## Description

This application simulates the core functionalities of a library. It allows a user to manage books and customers, issue books, and handle returns. The primary goal of this project is to create a well-structured, robust application that serves as a clear example of C++ best practices for memory management, class design, and the separation of concerns.

## Features

- **Book Management**: Add new books to the library and view a list of all books with their current status (Available or On Loan).
- **Customer Management**: Register new customers and view a list of all registered customers along with the books they have borrowed.
- **Issue/Loan System**: Issue available books to registered customers. The system validates the book and customer IDs and checks the book's availability.
- **Return System**: Process the return of a book, verifying that the correct customer is returning it.
- **Clear User Interface**: A simple and intuitive command-line menu for easy interaction.
- **Robust Error Handling**: Provides clear, specific feedback to the user for invalid operations (e.g., book not found, customer does not exist, book is already on loan).

## Object-Oriented Programming (OOP) Concepts Applied

This project heavily utilizes fundamental OOP concepts:

- **Inheritance**: The `Customer` and `Staff` classes inherit from a common base class, `Person`, to promote code reuse and establish a clear "is-a" relationship.
- **Encapsulation**: Class data members are kept `private` to protect the internal state. Access and modifications are controlled through `public` member functions (getters and setters), ensuring data integrity.
- **Abstraction**: The classes (`Book`, `Customer`, `Library`) model real-world entities, hiding complex implementation details behind simple, high-level interfaces.
- **Polymorphism**: A `virtual` destructor is used in the `Person` base class to ensure that derived class destructors are called correctly, preventing memory leaks in more complex scenarios.
- **Separation of Concerns**:
  - The `Library` class acts as the core engine, containing all business logic.
  - The `ConsoleUI` class is responsible only for displaying information, cleanly separating the user interface from the core logic.

## Design and Memory Management

- **Ownership Model**: The `Library` class is the sole owner of all `Book`, `Customer`, and `Staff` objects. This is enforced using `std::vector<std::unique_ptr<T>>`, which ensures that resources are automatically deallocated when the `Library` object is destroyed, preventing memory leaks.
- **Non-Owning References**: When relationships are formed (e.g., a `Loan` connects a `Customer` and a `Book`), raw pointers (`Book*`, `Customer*`) are used. These act as temporary, non-owning references, ensuring there is only one owner for each object.

## How to Compile and Run

This project is written in standard C++ and should be compatible with any modern C++ compiler (like g++ or Clang).

1.  **Prerequisites**: You need a C++ compiler that supports the C++17 standard or newer.

2.  **Save the Code**: Save the complete code into a single file named `main.cpp`.

3.  **Compile**: Open your terminal or command prompt, navigate to the directory where you saved the file, and run the following command:
    ```sh
    g++ -std=c++17 -o library_management main.cpp
    ```

4.  **Run**: Execute the compiled program:
    ```sh
    ./library_management
    ```

## How to Use

Once the application is running, you will be presented with a menu of options:

- **1. List All Books**: Displays the details and current status of every book in the library.
- **2. List All Customers**: Displays the details of every registered customer and the books they have borrowed.
- **3. Issue a Book**: Prompts you to enter a book's ISBN and a customer's ID to process a loan.
- **4. Return a Book**: Prompts you to enter the ISBN of the book being returned and the ID of the customer returning it.
- **9. Exit**: Closes the application.

## Future Improvements

- **File I/O**: Implement functionality to save the library's state (books, customers, loans) to a file and load it back on startup.
- **Due Dates and Fines**: Add a `Date` class to properly manage due dates and automatically calculate fines for overdue books.
- **Book Reservations**: Allow customers to reserve a book that is currently on loan.
- **Advanced Search**: Implement more detailed search functionality (e.g., find book by title or author, find customer by name).
- **Staff Roles**: Expand the `Staff` class to include different roles and permissions.
