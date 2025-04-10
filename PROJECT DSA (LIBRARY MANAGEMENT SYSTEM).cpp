#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 


typedef struct Book {
    int id;
    char title[100];
    char author[100];
    int isBorrowed;
    struct Book* next;
} Book;


typedef struct User {
    int id;
    char username[50];
    char password[50];
    char name[100];
    char type[20]; 
    int borrowLimit;
    int currentlyBorrowed;
    struct User* next;
} User;


typedef struct BorrowRecord {
    int bookId;
    int userId;
    char dueDate[20];
    struct BorrowRecord* next;
} BorrowRecord;

Book* head = NULL;
User* userHead = NULL;
BorrowRecord* recordHead = NULL;

int loggedInUserId = -1;
char loggedInUserType[20] = "";
char loggedInUsername[50] = "";
char loggedInName[100] = "";

const char* USER_FILE = "users.dat";
const char* BOOK_FILE = "books.dat";
const char* BORROW_FILE = "borrow_records.dat";

void saveUsersToFile();
void saveBooksToFile();
void saveBorrowRecordsToFile();
void loadUsersFromFile();
void loadBooksFromFile();
void loadBorrowRecordsFromFile();
void clearScreen();
void displayMainMenu();
void addBook(int id, char* title, char* author);
void displayBooks();
Book* searchBook(int id);
void deleteBook(int id);
void editBook(int id);
void borrowBookWithUser();
void returnBookWithUser();
void viewMyBorrowedBooks();
void viewMyAccount();
int loginUser();
void logoutUser();
void cleanupMemory();

void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void displayMainMenu() {
    clearScreen();
    
    printf("Login successful! Welcome, %s.\n\n", loggedInUsername);
    printf("===== Library Management System =====\n");
    printf("Logged in as: %s (%s)\n", loggedInName, loggedInUserType);
    
    if (strcmp(loggedInUserType, "Faculty") == 0) {
        printf("\n--- Admin Functions ---\n");
        printf("1. Add New Book\n");
        printf("2. Edit Book\n");
        printf("3. Delete Book\n");
    }
    
    printf("\n--- Book Functions ---\n");
    printf("4. View All Books\n");
    printf("5. Borrow a Book\n");
    printf("6. Return a Book\n");
    printf("7. View My Borrowed Books\n");
    
    printf("\n--- Account Functions ---\n");
    printf("8. View My Account\n");
    printf("9. Logout\n");
    printf("10. Exit\n");
    
    printf("Enter your choice: ");
}

void addBook(int id, char* title, char* author) {
    Book* newBook = (Book*)malloc(sizeof(Book));
    if (!newBook) {
        printf("Memory allocation failed!\n");
        return;
    }
    
    newBook->id = id;
    strcpy(newBook->title, title);
    strcpy(newBook->author, author);
    newBook->isBorrowed = 0;
    newBook->next = head;
    head = newBook;
    
    clearScreen();
    displayMainMenu();
    printf("\nBook added successfully!\n");
    
    saveBooksToFile();
}

void displayBooks() {
    Book* temp = head;
    
    clearScreen();
    displayMainMenu();
    
    if (!temp) {
        printf("\nNo books in the library.\n");
        return;
    }
    
    printf("\nLibrary Books:\n");
    printf("%-5s %-40s %-30s %-10s\n", "ID", "Title", "Author", "Status");
    printf("------------------------------------------------------------------\n");
    
    while (temp) {
        printf("%-5d %-40s %-30s %-10s\n", 
               temp->id, temp->title, temp->author, 
               temp->isBorrowed ? "Borrowed" : "Available");
        temp = temp->next;
    }
}

Book* searchBook(int id) {
    Book* temp = head;
    while (temp) {
        if (temp->id == id)
            return temp;
        temp = temp->next;
    }
    return NULL;
}

void deleteBook(int id) {
    Book *temp = head, *prev = NULL;
    
    while (temp && temp->id != id) {
        prev = temp;
        temp = temp->next;
    }
    
    clearScreen();
    displayMainMenu();
    
    if (!temp) {
        printf("\nBook not found!\n");
        return;
    }
    
    if (temp->isBorrowed) {
        printf("\nCannot delete a book that is currently borrowed!\n");
        return;
    }
    
    if (prev)
        prev->next = temp->next;
    else
        head = temp->next;
    free(temp);
    printf("\nBook deleted successfully!\n");
    
    saveBooksToFile();
}

void editBook(int id) {
    Book* book = searchBook(id);
    
    clearScreen();
    displayMainMenu();
    
    if (!book) {
        printf("\nBook not found!\n");
        return;
    }
    
    if (book->isBorrowed) {
        printf("\nCannot edit a book that is currently borrowed!\n");
        return;
    }
    
    printf("\nEnter new title (current: %s): ", book->title);
    getchar();
    fgets(book->title, sizeof(book->title), stdin);
    book->title[strcspn(book->title, "\n")] = 0;
    
    printf("Enter new author (current: %s): ", book->author);
    fgets(book->author, sizeof(book->author), stdin);
    book->author[strcspn(book->author, "\n")] = 0;
    
    printf("\nBook details updated successfully!\n");
    
    saveBooksToFile();
}

User* getLoggedInUser() {
    User* temp = userHead;
    while (temp) {
        if (temp->id == loggedInUserId)
            return temp;
        temp = temp->next;
    }
    return NULL;
}

int loginUser() {
    char username[50], password[50];
    
    clearScreen();
    printf("===== Library Management System =====\n");
    printf("\n===== User Login =====\n");
    printf("Enter Username: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = 0;
    
    printf("Enter Password: ");
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = 0;
    
    User* temp = userHead;
    while (temp) {
        if (strcmp(temp->username, username) == 0 && strcmp(temp->password, password) == 0) {
            loggedInUserId = temp->id;
            strcpy(loggedInUserType, temp->type);
            strcpy(loggedInUsername, temp->username);
            strcpy(loggedInName, temp->name);
            
            displayMainMenu();
            return 1;
        }
        temp = temp->next;
    }
    
    printf("\nInvalid username or password. Please try again.\n");
    sleep(2);
    return 0;
}

void logoutUser() {
    if (loggedInUserId == -1) {
        printf("\nNo user is currently logged in.\n");
        return;
    }
    
    loggedInUserId = -1;
    strcpy(loggedInUserType, "");
    strcpy(loggedInUsername, "");
    strcpy(loggedInName, "");
    
    clearScreen();
    printf("\nLogout successful. Thank you for using the Library System!\n");
    sleep(1);
}

void borrowBookWithUser() {
    clearScreen();
    displayMainMenu();
    
    if (loggedInUserId == -1) {
        printf("\nError: No user logged in.\n");
        return;
    }
    
    User* user = getLoggedInUser();
    if (!user) {
        printf("\nError: User account not found.\n");
        return;
    }
    
    if (user->currentlyBorrowed >= user->borrowLimit) {
        printf("\nYou have reached your borrowing limit (%d books).\n", user->borrowLimit);
        return;
    }
    
    printf("\nAvailable Books:\n");
    printf("%-5s %-40s %-30s\n", "ID", "Title", "Author");
    printf("-------------------------------------------------------\n");
    
    Book* temp = head;
    int availableCount = 0;
    
    while (temp) {
        if (!temp->isBorrowed) {
            printf("%-5d %-40s %-30s\n", temp->id, temp->title, temp->author);
            availableCount++;
        }
        temp = temp->next;
    }
    
    if (availableCount == 0) {
        printf("\nNo books available for borrowing.\n");
        return;
    }
    
    int bookId;
    printf("\nEnter Book ID to borrow: ");
    scanf("%d", &bookId);
    getchar();
    
    Book* book = searchBook(bookId);
    if (!book) {
        printf("\nBook not found!\n");
        return;
    }
    
    if (book->isBorrowed) {
        printf("\nThis book is already borrowed.\n");
        return;
    }
    
    char dueDate[20];
    printf("Enter Due Date (DD/MM/YYYY): ");
    fgets(dueDate, sizeof(dueDate), stdin);
    dueDate[strcspn(dueDate, "\n")] = 0;
    
    book->isBorrowed = 1;
    
    user->currentlyBorrowed++;
    
    BorrowRecord* newRecord = (BorrowRecord*)malloc(sizeof(BorrowRecord));
    if (!newRecord) {
        printf("\nMemory allocation failed!\n");
        return;
    }
    
    newRecord->bookId = bookId;
    newRecord->userId = loggedInUserId;
    strcpy(newRecord->dueDate, dueDate);
    newRecord->next = recordHead;
    recordHead = newRecord;
    
    saveUsersToFile();
    saveBooksToFile();
    saveBorrowRecordsToFile();
    
    clearScreen();
    displayMainMenu();
    printf("\nBook '%s' borrowed successfully!\n", book->title);
    printf("Due date: %s\n", dueDate);
    printf("You now have %d/%d books borrowed.\n", user->currentlyBorrowed, user->borrowLimit);
}

void returnBookWithUser() {
    clearScreen();
    displayMainMenu();
    
    if (loggedInUserId == -1) {
        printf("\nError: No user logged in.\n");
        return;
    }
    
    printf("\nBooks you have borrowed:\n");
    printf("%-5s %-40s %-30s %-15s\n", "ID", "Title", "Author", "Due Date");
    printf("--------------------------------------------------------------------------\n");
    
    BorrowRecord *record = recordHead, *prev = NULL, *toDelete = NULL;
    Book* book;
    int borrowedCount = 0;
    
    while (record) {
        if (record->userId == loggedInUserId) {
            book = searchBook(record->bookId);
            if (book) {
                printf("%-5d %-40s %-30s %-15s\n", 
                       book->id, book->title, book->author, record->dueDate);
                borrowedCount++;
            }
        }
        record = record->next;
    }
    
    if (borrowedCount == 0) {
        printf("\nYou haven't borrowed any books.\n");
        return;
    }
    
    int bookId;
    printf("\nEnter Book ID to return: ");
    scanf("%d", &bookId);
    getchar();
    
    book = searchBook(bookId);
    if (!book) {
        printf("\nBook not found!\n");
        return;
    }
    
    record = recordHead;
    prev = NULL;
    int foundRecord = 0;
    
    while (record) {
        if (record->bookId == bookId && record->userId == loggedInUserId) {
            foundRecord = 1;
            toDelete = record;
            break;
        }
        prev = record;
        record = record->next;
    }
    
    if (!foundRecord) {
        printf("\nYou haven't borrowed this book.\n");
        return;
    }
    
    book->isBorrowed = 0;
    
    User* user = getLoggedInUser();
    if (user) {
        user->currentlyBorrowed--;
    }
    
    if (prev) {
        prev->next = toDelete->next;
    } else {
        recordHead = toDelete->next;
    }
    free(toDelete);
    
    saveUsersToFile();
    saveBooksToFile();
    saveBorrowRecordsToFile();
    
    clearScreen();
    displayMainMenu();
    printf("\nBook '%s' returned successfully!\n", book->title);
    if (user) {
        printf("You now have %d/%d books borrowed.\n", user->currentlyBorrowed, user->borrowLimit);
    }
}

void viewMyBorrowedBooks() {
    clearScreen();
    displayMainMenu();
    
    if (loggedInUserId == -1) {
        printf("\nError: No user logged in.\n");
        return;
    }
    
    User* user = getLoggedInUser();
    if (!user) {
        printf("\nError: User account not found.\n");
        return;
    }
    
    printf("\n===== Your Borrowed Books =====\n");
    printf("User: %s (%s)\n", user->name, user->type);
    printf("Borrowing Status: %d/%d books\n\n", user->currentlyBorrowed, user->borrowLimit);
    
    printf("%-5s %-40s %-30s %-15s\n", "ID", "Title", "Author", "Due Date");
    printf("--------------------------------------------------------------------------\n");
    
    BorrowRecord* record = recordHead;
    Book* book;
    int found = 0;
    
    while (record) {
        if (record->userId == loggedInUserId) {
            book = searchBook(record->bookId);
            if (book) {
                printf("%-5d %-40s %-30s %-15s\n", 
                       book->id, book->title, book->author, record->dueDate);
                found = 1;
            }
        }
        record = record->next;
    }
    
    if (!found) {
        printf("\nYou haven't borrowed any books yet.\n");
    }
}

void viewMyAccount() {
    clearScreen();
    displayMainMenu();
    
    if (loggedInUserId == -1) {
        printf("\nError: No user logged in.\n");
        return;
    }
    
    User* user = getLoggedInUser();
    if (!user) {
        printf("\nError: User account not found.\n");
        return;
    }
    
    printf("\n===== Your Account Details =====\n");
    printf("User ID: %d\n", user->id);
    printf("Username: %s\n", user->username);
    printf("Name: %s\n", user->name);
    printf("User Type: %s\n", user->type);
    printf("Borrowing Limit: %d books\n", user->borrowLimit);
    printf("Currently Borrowed: %d books\n", user->currentlyBorrowed);
}

void saveUsersToFile() {
    FILE* file = fopen(USER_FILE, "wb");
    if (!file) {
        printf("Error: Could not open users file for writing.\n");
        return;
    }
    
    User* temp = userHead;
    while (temp) {
        fwrite(temp, sizeof(User), 1, file);
        temp = temp->next;
    }
    
    fclose(file);
}

void saveBooksToFile() {
    FILE* file = fopen(BOOK_FILE, "wb");
    if (!file) {
        printf("Error: Could not open books file for writing.\n");
        return;
    }
    
    Book* temp = head;
    while (temp) {
        fwrite(temp, sizeof(Book), 1, file);
        temp = temp->next;
    }
    
    fclose(file);
}

void saveBorrowRecordsToFile() {
    FILE* file = fopen(BORROW_FILE, "wb");
    if (!file) {
        printf("Error: Could not open borrow records file for writing.\n");
        return;
    }
    
    BorrowRecord* temp = recordHead;
    while (temp) {
        fwrite(temp, sizeof(BorrowRecord), 1, file);
        temp = temp->next;
    }
    
    fclose(file);
}

void loadUsersFromFile() {
    FILE* file = fopen(USER_FILE, "rb");
    if (!file) {
        User* defaultUser = (User*)malloc(sizeof(User));
        if (defaultUser) {
            defaultUser->id = 1;
            strcpy(defaultUser->username, "abcd");
            strcpy(defaultUser->password, "1234");
            strcpy(defaultUser->name, "ABCD");
            strcpy(defaultUser->type, "Faculty");
            defaultUser->borrowLimit = 5;
            defaultUser->currentlyBorrowed = 0;
            defaultUser->next = NULL;
            userHead = defaultUser;
            saveUsersToFile();
        }
        return;
    }
    
    User *temp = userHead, *next;
    while (temp) {
        next = temp->next;
        free(temp);
        temp = next;
    }
    userHead = NULL;
    
    User userData;
    User* lastNode = NULL;
    
    while (fread(&userData, sizeof(User), 1, file)) {
        User* newUser = (User*)malloc(sizeof(User));
        if (!newUser) {
            printf("Memory allocation failed!\n");
            continue;
        }
        
        *newUser = userData;
        newUser->next = NULL;
        
        if (lastNode) {
            lastNode->next = newUser;
            lastNode = newUser;
        } else {
            userHead = newUser;
            lastNode = newUser;
        }
    }
    
    fclose(file);
}

void loadBooksFromFile() {
    FILE* file = fopen(BOOK_FILE, "rb");
    if (!file) {
        return;
    }
    
    Book *temp = head, *next;
    while (temp) {
        next = temp->next;
        free(temp);
        temp = next;
    }
    head = NULL;
    
    Book bookData;
    Book* lastNode = NULL;
    
    while (fread(&bookData, sizeof(Book), 1, file)) {
        Book* newBook = (Book*)malloc(sizeof(Book));
        if (!newBook) {
            printf("Memory allocation failed!\n");
            continue;
        }
        
        *newBook = bookData;
        newBook->next = NULL;
        
        if (lastNode) {
            lastNode->next = newBook;
            lastNode = newBook;
        } else {
            head = newBook;
            lastNode = newBook;
        }
    }
    
    fclose(file);
}

void loadBorrowRecordsFromFile() {
    FILE* file = fopen(BORROW_FILE, "rb");
    if (!file) {
        return;
    }
    
    BorrowRecord *temp = recordHead, *next;
    while (temp) {
        next = temp->next;
        free(temp);
        temp = next;
    }
    recordHead = NULL;
    
    BorrowRecord recordData;
    BorrowRecord* lastNode = NULL;
    
    while (fread(&recordData, sizeof(BorrowRecord), 1, file)) {
        BorrowRecord* newRecord = (BorrowRecord*)malloc(sizeof(BorrowRecord));
        if (!newRecord) {
            printf("Memory allocation failed!\n");
            continue;
        }
        
        *newRecord = recordData;
        newRecord->next = NULL;
        
        if (lastNode) {
            lastNode->next = newRecord;
            lastNode = newRecord;
        } else {
            recordHead = newRecord;
            lastNode = newRecord;
        }
    }
    
    fclose(file);
}

void cleanupMemory() {
    Book *bookTemp = head, *bookNext;
    while (bookTemp) {
        bookNext = bookTemp->next;
        free(bookTemp);
        bookTemp = bookNext;
    }
    
    User *userTemp = userHead, *userNext;
    while (userTemp) {
        userNext = userTemp->next;
        free(userTemp);
        userTemp = userNext;
    }
    
    BorrowRecord *recordTemp = recordHead, *recordNext;
    while (recordTemp) {
        recordNext = recordTemp->next;
        free(recordTemp);
        recordTemp = recordNext;
    }
}

void initializeProgramData() {
    loadUsersFromFile();
    loadBooksFromFile();
    loadBorrowRecordsFromFile();
}

int main() {
    int choice;
    
    initializeProgramData();
    
    while (1) {
        if (loggedInUserId == -1) {
            if (!loginUser()) {
                continue;
            }
        } else {
            scanf("%d", &choice);
            getchar();
            
            switch (choice) {
                case 1:
                    if (strcmp(loggedInUserType, "Faculty") == 0) {
                        int id;
                        char title[100], author[100];
                        
                        clearScreen();
                        displayMainMenu();
                        printf("\nEnter Book ID: ");
                        scanf("%d", &id);
                        getchar();
                        
                        if (searchBook(id)) {
                            printf("\nA book with this ID already exists. Please use a different ID.\n");
                            break;
                        }
                        
                        printf("Enter Book Title: ");
                        fgets(title, sizeof(title), stdin);
                        title[strcspn(title, "\n")] = 0;
                        
                        printf("Enter Book Author: ");
                        fgets(author, sizeof(author), stdin);
                        author[strcspn(author, "\n")] = 0;
                        
                        addBook(id, title, author);
                    } else {
                        clearScreen();
                        displayMainMenu();
                        printf("\nAccess denied. Only Faculty members can add books.\n");
                    }
                    break;
                    
                case 2:
                    if (strcmp(loggedInUserType, "Faculty") == 0) {
                        int id;
                        clearScreen();
                        displayMainMenu();
                        printf("\nEnter Book ID to edit: ");
                        scanf("%d", &id);
                        getchar();
                        editBook(id);
                    } else {
                        clearScreen();
                        displayMainMenu();
                        printf("\nAccess denied. Only Faculty members can edit books.\n");
                    }
                    break;
                    
                case 3:
                    if (strcmp(loggedInUserType, "Faculty") == 0) {
                        int id;
                        clearScreen();
                        displayMainMenu();
                        printf("\nEnter Book ID to delete: ");
                        scanf("%d", &id);
                        getchar();
                        deleteBook(id);
                    } else {
                        clearScreen();
                        displayMainMenu();
                        printf("\nAccess denied. Only Faculty members can delete books.\n");
                    }
                    break;
                    
                case 4:
                    displayBooks();
                    break;
                    
                case 5:
                    borrowBookWithUser();
                    break;
                    
                case 6:
                    returnBookWithUser();
                    break;
                    
                case 7:
                    viewMyBorrowedBooks();
                    break;
                    
                case 8:
                    viewMyAccount();
                    break;
                    
                case 9:
                    logoutUser();
                    break;
                    
                case 10:
                    clearScreen();
                    printf("\nThank you for using the Library System. Goodbye!\n");
                    saveUsersToFile();
                    saveBooksToFile();
                    saveBorrowRecordsToFile();
                    cleanupMemory();
                    return 0;
                    
                default:
                    clearScreen();
                    displayMainMenu();
                    printf("\nInvalid choice. Please try again.\n");
            }
        }
    }
    
    return 0;
}
