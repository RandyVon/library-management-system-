#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structure for Book
typedef struct Book {
    int id;
    char title[100];
    char author[100];
    int isBorrowed;
    struct Book* next;
} Book;

Book* head = NULL; // Head of the linked list

// Function to add a book
void addBook(int id, char* title, char* author) {
    Book* newBook = (Book*)malloc(sizeof(Book));
    newBook->id = id;
    strcpy(newBook->title, title);
    strcpy(newBook->author, author);
    newBook->isBorrowed = 0;
    newBook->next = head;
    head = newBook;
    printf("Book added successfully!\n");
}

// Function to display all books
void displayBooks() {
    Book* temp = head;
    if (!temp) {
        printf("No books in the library.\n");
        return;
    }
    printf("\nLibrary Books:\n");
    while (temp) {
        printf("ID: %d, Title: %s, Author: %s, Status: %s\n", temp->id, temp->title, temp->author, temp->isBorrowed ? "Borrowed" : "Available");
        temp = temp->next;
    }
}

// Function to search a book by ID
Book* searchBook(int id) {
    Book* temp = head;
    while (temp) {
        if (temp->id == id)
            return temp;
        temp = temp->next;
    }
    return NULL;
}

// Function to borrow a book
void borrowBook(int id) {
    Book* book = searchBook(id);
    if (!book) {
        printf("Book not found!\n");
        return;
    }
    if (book->isBorrowed) {
        printf("Book is already borrowed.\n");
        return;
    }
    book->isBorrowed = 1;
    printf("Book borrowed successfully!\n");
}

// Function to return a book
void returnBook(int id) {
    Book* book = searchBook(id);
    if (!book) {
        printf("Book not found!\n");
        return;
    }
    if (!book->isBorrowed) {
        printf("Book is already available in the library.\n");
        return;
    }
    book->isBorrowed = 0;
    printf("Book returned successfully!\n");
}

// Function to delete a book
void deleteBook(int id) {
    Book *temp = head, *prev = NULL;
    while (temp && temp->id != id) {
        prev = temp;
        temp = temp->next;
    }
    if (!temp) {
        printf("Book not found!\n");
        return;
    }
    if (prev)
        prev->next = temp->next;
    else
        head = temp->next;
    free(temp);
    printf("Book deleted successfully!\n");
}

// Function to edit book details
void editBook(int id) {
    Book* book = searchBook(id);
    if (!book) {
        printf("Book not found!\n");
        return;
    }
    printf("Enter new title: ");
    getchar();
    fgets(book->title, sizeof(book->title), stdin);
    book->title[strcspn(book->title, "\n")] = 0;
    printf("Enter new author: ");
    fgets(book->author, sizeof(book->author), stdin);
    book->author[strcspn(book->author, "\n")] = 0;
    printf("Book details updated successfully!\n");
}

// Function to count available books
void countAvailableBooks() {
    int count = 0;
    Book* temp = head;
    while (temp) {
        if (!temp->isBorrowed)
            count++;
        temp = temp->next;
    }
    printf("Total available books: %d\n", count);
}

// Main function
int main() {
    int choice, id;
    char title[100], author[100];

    while (1) {
        printf("\nLibrary Management System\n");
        printf("1. Add Book\n2. Display Books\n3. Borrow Book\n4. Return Book\n5. Delete Book\n6. Edit Book\n7. Count Available Books\n8. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar();
        switch (choice) {
            case 1:
                printf("Enter Book ID: ");
                scanf("%d", &id);
                getchar();
                printf("Enter Book Title: ");
                fgets(title, sizeof(title), stdin);
                title[strcspn(title, "\n")] = 0;
                printf("Enter Book Author: ");
                fgets(author, sizeof(author), stdin);
                author[strcspn(author, "\n")] = 0;
                addBook(id, title, author);
                break;
            case 2:
                displayBooks();
                break;
            case 3:
                printf("Enter Book ID to borrow: ");
                scanf("%d", &id);
                borrowBook(id);
                break;
            case 4:
                printf("Enter Book ID to return: ");
                scanf("%d", &id);
                returnBook(id);
                break;
            case 5:
                printf("Enter Book ID to delete: ");
                scanf("%d", &id);
                deleteBook(id);
                break;
            case 6:
                printf("Enter Book ID to edit: ");
                scanf("%d", &id);
                editBook(id);
                break;
            case 7:
                countAvailableBooks();
                break;
            case 8:
                printf("Exiting program...\n");
                return 0;
            default:
                printf("Invalid choice!\n");
        }
    }
    return 0;
}
