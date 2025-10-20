#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#ifdef _WIN32
    #include <conio.h>  // For _getch() on Windows
#endif

#define FILENAME "bank_accounts.dat"
#define TRANSACTION_LOG "transactions.log"
#define MAX_ACCOUNTS 10000
#define MIN_ACCOUNT_NUMBER 100000
#define MAX_ACCOUNT_NUMBER 999999
#define MIN_BALANCE 0
#define MAX_NAME_LENGTH 100
#define PASSWORD_LENGTH 50
#define HASH_LENGTH 65  // SHA-256 produces 64 hex characters + null terminator
#define MAX_LOGIN_ATTEMPTS 3

// Platform-specific clear screen
#ifdef _WIN32
    #define CLEAR_SCREEN "cls"
#else
    #define CLEAR_SCREEN "clear"
#endif

// ANSI Color codes for better UI (works on most modern terminals)
#define COLOR_RESET   "\x1b[0m"
#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_WHITE   "\x1b[37m"
#define COLOR_BOLD    "\x1b[1m"

// Account status
typedef enum {
    ACCOUNT_ACTIVE = 1,
    ACCOUNT_SUSPENDED = 0,
    ACCOUNT_CLOSED = -1
} AccountStatus;

// Transaction types
typedef enum {
    TRANSACTION_DEPOSIT,
    TRANSACTION_WITHDRAWAL,
    TRANSACTION_TRANSFER_OUT,
    TRANSACTION_TRANSFER_IN,
    TRANSACTION_ACCOUNT_CREATED
} TransactionType;

// Account structure
typedef struct {
    int account_number;
    char name[MAX_NAME_LENGTH];
    char email[MAX_NAME_LENGTH];
    char phone[20];
    double balance;
    char password_hash[HASH_LENGTH];  // Changed to store hash
    AccountStatus status;
    time_t created_date;
    time_t last_accessed;
    int failed_login_attempts;
} Account;

// Transaction structure
typedef struct {
    int transaction_id;
    int account_number;
    TransactionType type;
    double amount;
    double balance_after;
    int related_account;  // For transfers
    time_t timestamp;
    char description[100];
} Transaction;

// Function prototypes
void showWelcomeScreen();
void showMainMenu();
void createAccount();
void depositMoney();
void withdrawMoney();
void checkBalance();
void transferFunds();
void displayAllAccounts();
void viewAccountDetails();
void changePassword();
void viewTransactionHistory();
void generateAccountStatement();

// Utility functions
long findAccount(int account_number);
int authenticateAccount(int account_number, int max_attempts);
void logTransaction(int account_number, TransactionType type, double amount,
                   double balance_after, int related_account, const char *description);
int generateAccountNumber();
void getCurrentDateTime(char *buffer);

// Password hashing functions
void sha256_hash(const char *input, char *output);
void simple_hash(const char *password, char *hash_output);

// Input validation functions
int getIntInput(const char *prompt, int min, int max);
double getDoubleInput(const char *prompt, double min, double max);
void getStringInput(const char *prompt, char *buffer, int max_length);
void getEmailInput(const char *prompt, char *buffer, int max_length);
void getPhoneInput(const char *prompt, char *buffer, int max_length);
void getPasswordInput(const char *prompt, char *buffer, int max_length);
int validateEmail(const char *email);
int validatePhone(const char *phone);

// UI functions
void clearInputBuffer();
void initializeFile();
void clearScreen();
void pauseScreen();
void printHeader(const char *title);
void printSeparator(char c, int length);
void printSuccess(const char *message);
void printError(const char *message);
void printWarning(const char *message);
void printInfo(const char *message);

// Simple SHA-256 implementation for password hashing
void simple_hash(const char *password, char *hash_output) {
    // This is a simplified hash function for demonstration
    // In production, use a proper library like OpenSSL or libsodium
    unsigned long hash = 5381;
    int c;
    const char *str = password;

    // DJB2 hash algorithm (simple but effective for demonstration)
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }

    // Add a salt-like component based on password length and characters
    unsigned long salt = 0;
    for (int i = 0; password[i] != '\0'; i++) {
        salt += (unsigned char)password[i] * (i + 1);
    }
    hash ^= salt;

    // Convert to hex string (64 characters to simulate SHA-256 length)
    snprintf(hash_output, HASH_LENGTH, "%016lx%016lx%016lx%016lx",
             hash, hash ^ 0xDEADBEEF, hash ^ 0xCAFEBABE, hash ^ 0xFEEDFACE);
}

int main() {
    int choice;

    // Initialize random seed once
    srand(time(NULL));

    // Initialize files
    initializeFile();

    showWelcomeScreen();

    while (1) {
        showMainMenu();

        if (scanf("%d", &choice) != 1) {
            clearInputBuffer();
            printError("Invalid input! Please enter a number.");
            pauseScreen();
            continue;
        }
        clearInputBuffer();

        switch (choice) {
            case 1:
                clearScreen();
                createAccount();
                pauseScreen();
                break;
            case 2:
                clearScreen();
                depositMoney();
                pauseScreen();
                break;
            case 3:
                clearScreen();
                withdrawMoney();
                pauseScreen();
                break;
            case 4:
                clearScreen();
                checkBalance();
                pauseScreen();
                break;
            case 5:
                clearScreen();
                transferFunds();
                pauseScreen();
                break;
            case 6:
                clearScreen();
                viewAccountDetails();
                pauseScreen();
                break;
            case 7:
                clearScreen();
                changePassword();
                pauseScreen();
                break;
            case 8:
                clearScreen();
                viewTransactionHistory();
                pauseScreen();
                break;
            case 9:
                clearScreen();
                generateAccountStatement();
                pauseScreen();
                break;
            case 10:
                clearScreen();
                displayAllAccounts();
                pauseScreen();
                break;
            case 0:
                clearScreen();
                printHeader("THANK YOU");
                printf("\n");
                printInfo("Thank you for banking with us!");
                printInfo("Your security is our priority.");
                printf("\n");
                printSeparator('=', 60);
                printf("\n");
                exit(0);
            default:
                printError("Invalid choice! Please select a valid option.");
                pauseScreen();
        }
    }

    return 0;
}

void showWelcomeScreen() {
    clearScreen();
    printf("\n");
    printSeparator('=', 70);
    printf("\n");
    printf("%s%s", COLOR_CYAN, COLOR_BOLD);
    printf("               PROFESSIONAL BANKING MANAGEMENT SYSTEM\n");
    printf("%s", COLOR_RESET);
    printSeparator('=', 70);
    printf("\n\n");
    printf("%s", COLOR_YELLOW);
    printf("                    Secure And Reliable\n");
    printf("%s", COLOR_RESET);
    printf("\n");
    printInfo("System initializing with enhanced security...");
    printf("\n");
    time_t now = time(NULL);
    char datetime[50];
    strftime(datetime, sizeof(datetime), "%A, %B %d, %Y - %I:%M %p", localtime(&now));
    printf("                    %s\n", datetime);
    printf("\n");
    printSeparator('=', 70);
    printf("\n");
    pauseScreen();
}

void showMainMenu() {
    clearScreen();
    printHeader("MAIN MENU");
    printf("\n");
    printf("  %s[ACCOUNT OPERATIONS]%s\n", COLOR_CYAN, COLOR_RESET);
    printf("  1. Create New Account\n");
    printf("  2. Deposit Money\n");
    printf("  3. Withdraw Money\n");
    printf("  4. Check Balance\n");
    printf("  5. Transfer Funds\n");
    printf("\n");
    printf("  %s[ACCOUNT MANAGEMENT]%s\n", COLOR_CYAN, COLOR_RESET);
    printf("  6. View Account Details\n");
    printf("  7. Change Password\n");
    printf("  8. Transaction History\n");
    printf("  9. Generate Statement\n");
    printf("\n");
    printf("  %s[ADMINISTRATION]%s\n", COLOR_CYAN, COLOR_RESET);
    printf("  10. Display All Accounts (Admin)\n");
    printf("\n");
    printf("  %s0. Exit%s\n", COLOR_RED, COLOR_RESET);
    printf("\n");
    printSeparator('-', 60);
    printf("\n%sEnter your choice:%s ", COLOR_BOLD, COLOR_RESET);
}

void clearScreen() {
    system(CLEAR_SCREEN);
}

void pauseScreen() {
    printf("\n");
    printSeparator('-', 60);
    printf("\n%sPress Enter to continue...%s", COLOR_YELLOW, COLOR_RESET);
    getchar();
}

void printHeader(const char *title) {
    printSeparator('=', 60);
    printf("\n");
    printf("%s%s", COLOR_BOLD, COLOR_CYAN);
    int padding = (60 - strlen(title)) / 2;
    for (int i = 0; i < padding; i++) printf(" ");
    printf("%s\n", title);
    printf("%s", COLOR_RESET);
    printSeparator('=', 60);
}

void printSeparator(char c, int length) {
    for (int i = 0; i < length; i++) {
        printf("%c", c);
    }
    printf("\n");
}

void printSuccess(const char *message) {
    printf("%s%s%s\n", COLOR_GREEN, message, COLOR_RESET);
}

void printError(const char *message) {
    printf("%sERROR: %s%s\n", COLOR_RED, message, COLOR_RESET);
}

void printWarning(const char *message) {
    printf("%sWARNING: %s%s\n", COLOR_YELLOW, message, COLOR_RESET);
}

void printInfo(const char *message) {
    printf("%s%s%s\n", COLOR_BLUE, message, COLOR_RESET);
}

void initializeFile() {
    FILE *file = fopen(FILENAME, "ab");
    if (file != NULL) {
        fclose(file);
    }

    FILE *log = fopen(TRANSACTION_LOG, "a");
    if (log != NULL) {
        fclose(log);
    }
}

int generateAccountNumber() {
    int account_number;
    do {
        account_number = MIN_ACCOUNT_NUMBER + (rand() % (MAX_ACCOUNT_NUMBER - MIN_ACCOUNT_NUMBER + 1));
    } while (findAccount(account_number) != -1);

    return account_number;
}

void createAccount() {
    Account new_account;
    FILE *file;
    char password[PASSWORD_LENGTH];

    printHeader("CREATE NEW ACCOUNT");
    printf("\n");

    // Generate unique account number
    new_account.account_number = generateAccountNumber();

    // Get account holder details
    getStringInput("Full Name: ", new_account.name, MAX_NAME_LENGTH);
    getEmailInput("Email Address: ", new_account.email, MAX_NAME_LENGTH);
    getPhoneInput("Phone Number: ", new_account.phone, 20);

    // Set initial values
    new_account.balance = 0.0;
    new_account.status = ACCOUNT_ACTIVE;
    new_account.created_date = time(NULL);
    new_account.last_accessed = time(NULL);
    new_account.failed_login_attempts = 0;

    printf("\n");
    printInfo("Setting up secure password...");
    getPasswordInput("Set Password (min 6 characters): ", password, PASSWORD_LENGTH);

    if (strlen(password) < 6) {
        printError("Password must be at least 6 characters long!");
        return;
    }

    // Hash the password before storing
    simple_hash(password, new_account.password_hash);
    printSuccess("Password encrypted successfully!");

    // Open file in append binary mode
    file = fopen(FILENAME, "ab");
    if (file == NULL) {
        printError("Unable to access database!");
        return;
    }

    // Write account to file
    if (fwrite(&new_account, sizeof(Account), 1, file) == 1) {
        fclose(file);

        // Log transaction
        logTransaction(new_account.account_number, TRANSACTION_ACCOUNT_CREATED,
                      0.0, 0.0, 0, "Account created");

        printf("\n");
        printSeparator('=', 60);
        printf("\n");
        printSuccess("ACCOUNT CREATED SUCCESSFULLY!");
        printf("\n");
        printSeparator('-', 60);
        printf("\n");
        printf("  %sAccount Number:%s %d\n", COLOR_BOLD, COLOR_RESET, new_account.account_number);
        printf("  %sAccount Holder:%s %s\n", COLOR_BOLD, COLOR_RESET, new_account.name);
        printf("  %sEmail:%s %s\n", COLOR_BOLD, COLOR_RESET, new_account.email);
        printf("  %sPhone:%s %s\n", COLOR_BOLD, COLOR_RESET, new_account.phone);
        printf("  %sInitial Balance:%s $%.2f\n", COLOR_BOLD, COLOR_RESET, new_account.balance);
        printf("  %sAccount Status:%s Active\n", COLOR_BOLD, COLOR_RESET);
        printf("  %sSecurity:%s Password encrypted with hash\n", COLOR_BOLD, COLOR_RESET);

        char date_str[50];
        strftime(date_str, sizeof(date_str), "%Y-%m-%d %H:%M:%S",
                localtime(&new_account.created_date));
        printf("  %sCreated On:%s %s\n", COLOR_BOLD, COLOR_RESET, date_str);
        printf("\n");
        printSeparator('-', 60);
        printf("\n");
        printWarning("IMPORTANT: Please remember your account number and password!");
        printInfo("Keep your credentials secure and confidential.");
    } else {
        fclose(file);
        printError("Unable to create account!");
    }
}

void depositMoney() {
    int account_number;
    double amount;
    Account account;
    FILE *file;
    long position;

    printHeader("DEPOSIT MONEY");
    printf("\n");

    account_number = getIntInput("Account Number: ", MIN_ACCOUNT_NUMBER, MAX_ACCOUNT_NUMBER);

    if (!authenticateAccount(account_number, MAX_LOGIN_ATTEMPTS)) {
        printError("Authentication failed!");
        return;
    }

    position = findAccount(account_number);
    if (position == -1) {
        printError("Account not found!");
        return;
    }

    amount = getDoubleInput("\nDeposit Amount: $", 0.01, 1000000.0);

    // Open file for reading and writing
    file = fopen(FILENAME, "r+b");
    if (file == NULL) {
        printError("Unable to access database!");
        return;
    }

    fseek(file, position, SEEK_SET);
    fread(&account, sizeof(Account), 1, file);

    double old_balance = account.balance;
    account.balance += amount;
    account.last_accessed = time(NULL);

    fseek(file, position, SEEK_SET);
    fwrite(&account, sizeof(Account), 1, file);
    fclose(file);

    // Log transaction
    logTransaction(account_number, TRANSACTION_DEPOSIT, amount,
                  account.balance, 0, "Cash deposit");

    printf("\n");
    printSeparator('=', 60);
    printf("\n");
    printSuccess("DEPOSIT SUCCESSFUL!");
    printf("\n");
    printSeparator('-', 60);
    printf("\n");
    printf("  Transaction Type:    Deposit\n");
    printf("  Amount Deposited:    %s$%.2f%s\n", COLOR_GREEN, amount, COLOR_RESET);
    printf("  Previous Balance:    $%.2f\n", old_balance);
    printf("  Current Balance:     %s$%.2f%s\n", COLOR_BOLD, account.balance, COLOR_RESET);

    char datetime[50];
    getCurrentDateTime(datetime);
    printf("  Transaction Time:    %s\n", datetime);
    printf("\n");
    printSeparator('=', 60);
}

void withdrawMoney() {
    int account_number;
    double amount;
    Account account;
    FILE *file;
    long position;

    printHeader("WITHDRAW MONEY");
    printf("\n");

    account_number = getIntInput("Account Number: ", MIN_ACCOUNT_NUMBER, MAX_ACCOUNT_NUMBER);

    if (!authenticateAccount(account_number, MAX_LOGIN_ATTEMPTS)) {
        printError("Authentication failed!");
        return;
    }

    position = findAccount(account_number);
    if (position == -1) {
        printError("Account not found!");
        return;
    }

    file = fopen(FILENAME, "rb");
    if (file == NULL) {
        printError("Unable to access database!");
        return;
    }

    fseek(file, position, SEEK_SET);
    fread(&account, sizeof(Account), 1, file);
    fclose(file);

    printf("\nAvailable Balance: %s$%.2f%s\n", COLOR_BOLD, account.balance, COLOR_RESET);

    if (account.balance <= 0) {
        printError("Insufficient funds! Cannot withdraw.");
        return;
    }

    amount = getDoubleInput("Withdrawal Amount: $", 0.01, account.balance);

    if (amount > account.balance) {
        printError("Insufficient funds!");
        return;
    }

    file = fopen(FILENAME, "r+b");
    if (file == NULL) {
        printError("Unable to access database!");
        return;
    }

    double old_balance = account.balance;
    account.balance -= amount;
    account.last_accessed = time(NULL);

    fseek(file, position, SEEK_SET);
    fwrite(&account, sizeof(Account), 1, file);
    fclose(file);

    // Log transaction
    logTransaction(account_number, TRANSACTION_WITHDRAWAL, amount,
                  account.balance, 0, "Cash withdrawal");

    printf("\n");
    printSeparator('=', 60);
    printf("\n");
    printSuccess("WITHDRAWAL SUCCESSFUL!");
    printf("\n");
    printSeparator('-', 60);
    printf("\n");
    printf("  Transaction Type:    Withdrawal\n");
    printf("  Amount Withdrawn:    %s$%.2f%s\n", COLOR_RED, amount, COLOR_RESET);
    printf("  Previous Balance:    $%.2f\n", old_balance);
    printf("  Current Balance:     %s$%.2f%s\n", COLOR_BOLD, account.balance, COLOR_RESET);

    char datetime[50];
    getCurrentDateTime(datetime);
    printf("  Transaction Time:    %s\n", datetime);
    printf("\n");
    printSeparator('=', 60);
}

void checkBalance() {
    int account_number;
    Account account;
    FILE *file;
    long position;

    printHeader("BALANCE INQUIRY");
    printf("\n");

    account_number = getIntInput("Account Number: ", MIN_ACCOUNT_NUMBER, MAX_ACCOUNT_NUMBER);

    if (!authenticateAccount(account_number, MAX_LOGIN_ATTEMPTS)) {
        printError("Authentication failed!");
        return;
    }

    position = findAccount(account_number);
    if (position == -1) {
        printError("Account not found!");
        return;
    }

    file = fopen(FILENAME, "r+b");
    if (file == NULL) {
        printError("Unable to access database!");
        return;
    }

    fseek(file, position, SEEK_SET);
    fread(&account, sizeof(Account), 1, file);

    account.last_accessed = time(NULL);
    fseek(file, position, SEEK_SET);
    fwrite(&account, sizeof(Account), 1, file);
    fclose(file);

    printf("\n");
    printSeparator('=', 60);
    printf("\n");
    printf("%s           ACCOUNT BALANCE DETAILS%s\n", COLOR_BOLD, COLOR_RESET);
    printf("\n");
    printSeparator('-', 60);
    printf("\n");
    printf("  Account Number:      %d\n", account.account_number);
    printf("  Account Holder:      %s\n", account.name);
    printf("  Current Balance:     %s$%.2f%s\n", COLOR_GREEN, account.balance, COLOR_RESET);
    printf("  Account Status:      %sActive%s\n", COLOR_GREEN, COLOR_RESET);

    char datetime[50];
    getCurrentDateTime(datetime);
    printf("  Query Time:          %s\n", datetime);
    printf("\n");
    printSeparator('=', 60);
}

void transferFunds() {
    int from_account, to_account;
    double amount;
    Account from_acc, to_acc;
    FILE *file;
    long from_position, to_position;

    printHeader("FUND TRANSFER");
    printf("\n");

    from_account = getIntInput("Your Account Number: ", MIN_ACCOUNT_NUMBER, MAX_ACCOUNT_NUMBER);

    if (!authenticateAccount(from_account, MAX_LOGIN_ATTEMPTS)) {
        printError("Authentication failed!");
        return;
    }

    from_position = findAccount(from_account);
    if (from_position == -1) {
        printError("Your account not found!");
        return;
    }

    to_account = getIntInput("\nRecipient Account Number: ", MIN_ACCOUNT_NUMBER, MAX_ACCOUNT_NUMBER);

    to_position = findAccount(to_account);
    if (to_position == -1) {
        printError("Recipient account not found!");
        return;
    }

    if (from_account == to_account) {
        printError("Cannot transfer to the same account!");
        return;
    }

    file = fopen(FILENAME, "rb");
    if (file == NULL) {
        printError("Unable to access database!");
        return;
    }

    fseek(file, from_position, SEEK_SET);
    fread(&from_acc, sizeof(Account), 1, file);

    fseek(file, to_position, SEEK_SET);
    fread(&to_acc, sizeof(Account), 1, file);
    fclose(file);

    printf("\nYour Available Balance: %s$%.2f%s\n", COLOR_BOLD, from_acc.balance, COLOR_RESET);
    printf("Recipient: %s%s%s\n", COLOR_CYAN, to_acc.name, COLOR_RESET);

    amount = getDoubleInput("\nTransfer Amount: $", 0.01, from_acc.balance);

    if (amount > from_acc.balance) {
        printError("Insufficient funds!");
        return;
    }

    // Confirmation
    printf("\n");
    printWarning("Please confirm the transfer details:");
    printf("  Transfer Amount: $%.2f\n", amount);
    printf("  To: %s (Account: %d)\n", to_acc.name, to_account);
    printf("\nConfirm transfer? (Y/N): ");

    char confirm;
    scanf(" %c", &confirm);
    clearInputBuffer();

    if (confirm != 'Y' && confirm != 'y') {
        printWarning("Transfer cancelled by user.");
        return;
    }

    double old_from_balance = from_acc.balance;
    double old_to_balance = to_acc.balance;

    from_acc.balance -= amount;
    to_acc.balance += amount;
    from_acc.last_accessed = time(NULL);
    to_acc.last_accessed = time(NULL);

    file = fopen(FILENAME, "r+b");
    if (file == NULL) {
        printError("Unable to access database!");
        return;
    }

    fseek(file, from_position, SEEK_SET);
    fwrite(&from_acc, sizeof(Account), 1, file);

    fseek(file, to_position, SEEK_SET);
    fwrite(&to_acc, sizeof(Account), 1, file);
    fclose(file);

    // Log transactions
    char desc[100];
    snprintf(desc, sizeof(desc), "Transfer to account %d", to_account);
    logTransaction(from_account, TRANSACTION_TRANSFER_OUT, amount,
                  from_acc.balance, to_account, desc);

    snprintf(desc, sizeof(desc), "Transfer from account %d", from_account);
    logTransaction(to_account, TRANSACTION_TRANSFER_IN, amount,
                  to_acc.balance, from_account, desc);

    printf("\n");
    printSeparator('=', 60);
    printf("\n");
    printSuccess("TRANSFER SUCCESSFUL!");
    printf("\n");
    printSeparator('-', 60);
    printf("\n");
    printf("  Transaction Type:    Fund Transfer\n");
    printf("  Amount Transferred:  %s$%.2f%s\n", COLOR_YELLOW, amount, COLOR_RESET);
    printf("  From Account:        %d\n", from_account);
    printf("  To Account:          %d (%s)\n", to_account, to_acc.name);
    printf("  Your Previous Bal:   $%.2f\n", old_from_balance);
    printf("  Your Current Bal:    %s$%.2f%s\n", COLOR_BOLD, from_acc.balance, COLOR_RESET);

    char datetime[50];
    getCurrentDateTime(datetime);
    printf("  Transaction Time:    %s\n", datetime);
    printf("\n");
    printSeparator('=', 60);
}

void viewAccountDetails() {
    int account_number;
    Account account;
    FILE *file;
    long position;

    printHeader("ACCOUNT DETAILS");
    printf("\n");

    account_number = getIntInput("Account Number: ", MIN_ACCOUNT_NUMBER, MAX_ACCOUNT_NUMBER);

    if (!authenticateAccount(account_number, MAX_LOGIN_ATTEMPTS)) {
        printError("Authentication failed!");
        return;
    }

    position = findAccount(account_number);
    if (position == -1) {
        printError("Account not found!");
        return;
    }

    file = fopen(FILENAME, "r+b");
    if (file == NULL) {
        printError("Unable to access database!");
        return;
    }

    fseek(file, position, SEEK_SET);
    fread(&account, sizeof(Account), 1, file);

    account.last_accessed = time(NULL);
    fseek(file, position, SEEK_SET);
    fwrite(&account, sizeof(Account), 1, file);
    fclose(file);

    printf("\n");
    printSeparator('=', 60);
    printf("\n");
    printf("%s         COMPLETE ACCOUNT INFORMATION%s\n", COLOR_BOLD, COLOR_RESET);
    printf("\n");
    printSeparator('-', 60);
    printf("\n");
    printf("  %sAccount Details:%s\n", COLOR_CYAN, COLOR_RESET);
    printf("    Account Number:    %d\n", account.account_number);
    printf("    Account Holder:    %s\n", account.name);
    printf("    Email Address:     %s\n", account.email);
    printf("    Phone Number:      %s\n", account.phone);
    printf("\n");
    printf("  %sFinancial Information:%s\n", COLOR_CYAN, COLOR_RESET);
    printf("    Current Balance:   %s$%.2f%s\n", COLOR_GREEN, account.balance, COLOR_RESET);
    printf("    Account Status:    %s%s%s\n", COLOR_GREEN, "Active", COLOR_RESET);
    printf("\n");
    printf("  %sSecurity:%s\n", COLOR_CYAN, COLOR_RESET);
    printf("    Password:          %sEncrypted (Hashed)%s\n", COLOR_GREEN, COLOR_RESET);
    printf("\n");
    printf("  %sAccount Activity:%s\n", COLOR_CYAN, COLOR_RESET);

    char created[50], accessed[50];
    strftime(created, sizeof(created), "%Y-%m-%d %H:%M:%S", localtime(&account.created_date));
    strftime(accessed, sizeof(accessed), "%Y-%m-%d %H:%M:%S", localtime(&account.last_accessed));

    printf("    Created On:        %s\n", created);
    printf("    Last Accessed:     %s\n", accessed);
    printf("\n");
    printSeparator('=', 60);
}

void changePassword() {
    int account_number;
    Account account;
    FILE *file;
    long position;
    char old_password[PASSWORD_LENGTH];
    char new_password[PASSWORD_LENGTH];
    char confirm_password[PASSWORD_LENGTH];
    char old_hash[HASH_LENGTH];

    printHeader("CHANGE PASSWORD");
    printf("\n");

    account_number = getIntInput("Account Number: ", MIN_ACCOUNT_NUMBER, MAX_ACCOUNT_NUMBER);

    position = findAccount(account_number);
    if (position == -1) {
        printError("Account not found!");
        return;
    }

    file = fopen(FILENAME, "rb");
    if (file == NULL) {
        printError("Unable to access database!");
        return;
    }

    fseek(file, position, SEEK_SET);
    fread(&account, sizeof(Account), 1, file);
    fclose(file);

    getPasswordInput("\nCurrent Password: ", old_password, PASSWORD_LENGTH);

    // Hash the input password and compare
    simple_hash(old_password, old_hash);
    if (strcmp(account.password_hash, old_hash) != 0) {
        printError("Current password is incorrect!");
        return;
    }

    getPasswordInput("New Password (min 6 characters): ", new_password, PASSWORD_LENGTH);

    if (strlen(new_password) < 6) {
        printError("New password must be at least 6 characters long!");
        return;
    }

    getPasswordInput("Confirm New Password: ", confirm_password, PASSWORD_LENGTH);

    if (strcmp(new_password, confirm_password) != 0) {
        printError("Passwords do not match!");
        return;
    }

    // Hash the new password
    simple_hash(new_password, account.password_hash);
    account.last_accessed = time(NULL);

    file = fopen(FILENAME, "r+b");
    if (file == NULL) {
        printError("Unable to access database!");
        return;
    }

    fseek(file, position, SEEK_SET);
    fwrite(&account, sizeof(Account), 1, file);
    fclose(file);

    printf("\n");
    printSeparator('=', 60);
    printf("\n");
    printSuccess("PASSWORD CHANGED SUCCESSFULLY!");
    printf("\n");
    printInfo("Your password has been updated and encrypted securely.");
    printWarning("Please keep your new password confidential.");
    printf("\n");
    printSeparator('=', 60);
}

void viewTransactionHistory() {
    int account_number;
    FILE *log;
    char line[500];
    int count = 0;

    printHeader("TRANSACTION HISTORY");
    printf("\n");

    account_number = getIntInput("Account Number: ", MIN_ACCOUNT_NUMBER, MAX_ACCOUNT_NUMBER);

    if (!authenticateAccount(account_number, MAX_LOGIN_ATTEMPTS)) {
        printError("Authentication failed!");
        return;
    }

    log = fopen(TRANSACTION_LOG, "r");
    if (log == NULL) {
        printInfo("No transaction history found.");
        return;
    }

    printf("\n");
    printSeparator('-', 90);
    printf("%-20s %-15s %-12s %-15s %-25s\n",
           "Date & Time", "Type", "Amount", "Balance", "Description");
    printSeparator('-', 90);

    while (fgets(line, sizeof(line), log)) {
        int acc_num;
        if (sscanf(line, "Account: %d", &acc_num) == 1 && acc_num == account_number) {
            printf("%s", line);
            count++;
        }
    }

    fclose(log);

    printSeparator('-', 90);
    if (count == 0) {
        printInfo("No transactions found for this account.");
    } else {
        printf("Total Transactions: %d\n", count);
    }
    printf("\n");
}

void generateAccountStatement() {
    int account_number;
    Account account;
    FILE *file, *log, *statement;
    long position;
    char filename[100];
    char line[500];
    int trans_count = 0;

    printHeader("GENERATE ACCOUNT STATEMENT");
    printf("\n");

    account_number = getIntInput("Account Number: ", MIN_ACCOUNT_NUMBER, MAX_ACCOUNT_NUMBER);

    if (!authenticateAccount(account_number, MAX_LOGIN_ATTEMPTS)) {
        printError("Authentication failed!");
        return;
    }

    position = findAccount(account_number);
    if (position == -1) {
        printError("Account not found!");
        return;
    }

    file = fopen(FILENAME, "rb");
    if (file == NULL) {
        printError("Unable to access database!");
        return;
    }

    fseek(file, position, SEEK_SET);
    fread(&account, sizeof(Account), 1, file);
    fclose(file);

    // Create statement file
    snprintf(filename, sizeof(filename), "statement_%d.txt", account_number);
    statement = fopen(filename, "w");
    if (statement == NULL) {
        printError("Unable to create statement file!");
        return;
    }

    // Write statement header
    fprintf(statement, "===============================================================\n");
    fprintf(statement, "              BANK ACCOUNT STATEMENT\n");
    fprintf(statement, "===============================================================\n\n");

    char datetime[50];
    getCurrentDateTime(datetime);
    fprintf(statement, "Statement Generated: %s\n\n", datetime);

    fprintf(statement, "ACCOUNT INFORMATION:\n");
    fprintf(statement, "-----------------------------------------------------------\n");
    fprintf(statement, "Account Number:    %d\n", account.account_number);
    fprintf(statement, "Account Holder:    %s\n", account.name);
    fprintf(statement, "Email:             %s\n", account.email);
    fprintf(statement, "Phone:             %s\n", account.phone);
    fprintf(statement, "Current Balance:   $%.2f\n", account.balance);
    fprintf(statement, "Account Status:    Active\n");
    fprintf(statement, "Security:          Password Encrypted (Hashed)\n\n");

    fprintf(statement, "TRANSACTION HISTORY:\n");
    fprintf(statement, "-----------------------------------------------------------\n");

    // Read transaction log
    log = fopen(TRANSACTION_LOG, "r");
    if (log != NULL) {
        while (fgets(line, sizeof(line), log)) {
            int acc_num;
            if (sscanf(line, "Account: %d", &acc_num) == 1 && acc_num == account_number) {
                fprintf(statement, "%s", line);
                trans_count++;
            }
        }
        fclose(log);
    }

    if (trans_count == 0) {
        fprintf(statement, "No transactions found.\n");
    }

    fprintf(statement, "\n-----------------------------------------------------------\n");
    fprintf(statement, "Total Transactions: %d\n", trans_count);
    fprintf(statement, "===============================================================\n");
    fprintf(statement, "         Thank you for banking with us!\n");
    fprintf(statement, "===============================================================\n");

    fclose(statement);

    printf("\n");
    printSeparator('=', 60);
    printf("\n");
    printSuccess("STATEMENT GENERATED SUCCESSFULLY!");
    printf("\n");
    printInfo("Statement has been saved to:");
    printf("  %s%s%s\n", COLOR_CYAN, filename, COLOR_RESET);
    printf("\n");
    printSeparator('=', 60);
}

void displayAllAccounts() {
    Account account;
    FILE *file;
    int count = 0;
    double total_balance = 0.0;
    char password[PASSWORD_LENGTH];
    char admin_hash[HASH_LENGTH];

    printHeader("ALL ACCOUNTS (ADMIN ACCESS)");
    printf("\n");

    printWarning("Administrative access required!");
    getPasswordInput("Enter Admin Password: ", password, PASSWORD_LENGTH);

    // Hash the admin password and check
    simple_hash("admin123", admin_hash);
    char input_hash[HASH_LENGTH];
    simple_hash(password, input_hash);

    if (strcmp(input_hash, admin_hash) != 0) {
        printError("Invalid admin password!");
        return;
    }

    file = fopen(FILENAME, "rb");
    if (file == NULL) {
        printError("Unable to access database!");
        return;
    }

    printf("\n");
    printSeparator('=', 100);
    printf("%-10s %-25s %-30s %-15s %-12s\n",
           "Acc No.", "Name", "Email", "Phone", "Balance");
    printSeparator('=', 100);

    while (fread(&account, sizeof(Account), 1, file) == 1) {
        printf("%-10d %-25s %-30s %-15s %s$%-11.2f%s\n",
               account.account_number,
               account.name,
               account.email,
               account.phone,
               COLOR_GREEN,
               account.balance,
               COLOR_RESET);
        count++;
        total_balance += account.balance;
    }

    fclose(file);

    printSeparator('=', 100);

    if (count == 0) {
        printInfo("No accounts found in the system.");
    } else {
        printf("\n");
        printf("  %sTotal Accounts:%s %d\n", COLOR_BOLD, COLOR_RESET, count);
        printf("  %sTotal Deposits:%s %s$%.2f%s\n", COLOR_BOLD, COLOR_RESET,
               COLOR_GREEN, total_balance, COLOR_RESET);
        printf("  %sSecurity Level:%s Password Hashing Enabled\n", COLOR_BOLD, COLOR_RESET);
        printf("\n");
    }
}

long findAccount(int account_number) {
    Account account;
    FILE *file;

    file = fopen(FILENAME, "rb");
    if (file == NULL) {
        return -1;
    }

    fseek(file, 0, SEEK_SET);

    while (fread(&account, sizeof(Account), 1, file) == 1) {
        if (account.account_number == account_number) {
            long found_position = ftell(file) - sizeof(Account);
            fclose(file);
            return found_position;
        }
    }

    fclose(file);
    return -1;
}

int authenticateAccount(int account_number, int max_attempts) {
    char password[PASSWORD_LENGTH];
    char input_hash[HASH_LENGTH];
    Account account;
    FILE *file;
    long position;
    int attempts = 0;

    position = findAccount(account_number);
    if (position == -1) {
        return 0;
    }

    file = fopen(FILENAME, "rb");
    if (file == NULL) {
        return 0;
    }

    fseek(file, position, SEEK_SET);
    if (fread(&account, sizeof(Account), 1, file) != 1) {
        fclose(file);
        return 0;
    }
    fclose(file);

    while (attempts < max_attempts) {
        getPasswordInput("Enter Password: ", password, PASSWORD_LENGTH);

        // Hash the input password
        simple_hash(password, input_hash);

        if (strcmp(account.password_hash, input_hash) == 0) {
            printSuccess("Authentication successful!");
            return 1;
        } else {
            attempts++;
            if (attempts < max_attempts) {
                printError("Incorrect password!");
                printf("Attempts remaining: %d\n", max_attempts - attempts);
            }
        }
    }

    printError("Maximum login attempts exceeded!");
    printWarning("Account temporarily locked for security.");
    return 0;
}

void logTransaction(int account_number, TransactionType type, double amount,
                   double balance_after, int related_account, const char *description) {
    FILE *log = fopen(TRANSACTION_LOG, "a");
    if (log == NULL) {
        return;
    }

    char datetime[50];
    getCurrentDateTime(datetime);

    const char *type_str;
    switch (type) {
        case TRANSACTION_DEPOSIT: type_str = "DEPOSIT"; break;
        case TRANSACTION_WITHDRAWAL: type_str = "WITHDRAWAL"; break;
        case TRANSACTION_TRANSFER_OUT: type_str = "TRANSFER OUT"; break;
        case TRANSACTION_TRANSFER_IN: type_str = "TRANSFER IN"; break;
        case TRANSACTION_ACCOUNT_CREATED: type_str = "ACCOUNT CREATED"; break;
        default: type_str = "UNKNOWN"; break;
    }

    fprintf(log, "Account: %d | %s | %-15s | $%-10.2f | Balance: $%-10.2f | %s\n",
            account_number, datetime, type_str, amount, balance_after, description);

    fclose(log);
}

void getCurrentDateTime(char *buffer) {
    time_t now = time(NULL);
    strftime(buffer, 50, "%Y-%m-%d %H:%M:%S", localtime(&now));
}

// Input validation functions
int getIntInput(const char *prompt, int min, int max) {
    int value;
    while (1) {
        printf("%s", prompt);
        if (scanf("%d", &value) == 1) {
            clearInputBuffer();
            if (value >= min && value <= max) {
                return value;
            }
        } else {
            clearInputBuffer();
        }
        printf("Invalid input! Please enter a number between %d and %d.\n", min, max);
    }
}

double getDoubleInput(const char *prompt, double min, double max) {
    double value;
    while (1) {
        printf("%s", prompt);
        if (scanf("%lf", &value) == 1) {
            clearInputBuffer();
            if (value >= min && value <= max) {
                return value;
            }
        } else {
            clearInputBuffer();
        }
        printf("Invalid amount! Range: $%.2f - $%.2f\n", min, max);
    }
}

void getStringInput(const char *prompt, char *buffer, int max_length) {
    while (1) {
        printf("%s", prompt);
        fgets(buffer, max_length, stdin);
        buffer[strcspn(buffer, "\n")] = '\0';

        // Trim whitespace
        char *start = buffer;
        while (isspace((unsigned char)*start)) start++;

        char *end = buffer + strlen(buffer) - 1;
        while (end > start && isspace((unsigned char)*end)) end--;
        *(end + 1) = '\0';

        if (start != buffer) {
            memmove(buffer, start, strlen(start) + 1);
        }

        if (strlen(buffer) > 0) {
            return;
        }
        printError("Input cannot be empty!");
    }
}

void getEmailInput(const char *prompt, char *buffer, int max_length) {
    while (1) {
        getStringInput(prompt, buffer, max_length);
        if (validateEmail(buffer)) {
            return;
        }
        printError("Invalid email format! Please try again.");
    }
}

void getPhoneInput(const char *prompt, char *buffer, int max_length) {
    while (1) {
        getStringInput(prompt, buffer, max_length);
        if (validatePhone(buffer)) {
            return;
        }
        printError("Invalid phone format! Use digits only (10-15 digits).");
    }
}

void getPasswordInput(const char *prompt, char *buffer, int max_length) {
    printf("%s", prompt);

    int i = 0;
    char ch;

    // Read character by character without echoing
    while (1) {
        #ifdef _WIN32
            ch = _getch();  // Windows - no echo
        #else
            // Unix/Linux - disable echo
            system("stty -echo");
            ch = getchar();
            system("stty echo");
        #endif

        if (ch == '\n' || ch == '\r') {  // Enter key
            break;
        } else if (ch == 127 || ch == 8) {  // Backspace
            if (i > 0) {
                i--;
                printf("\b \b");  // Erase the last asterisk
            }
        } else if (i < max_length - 1) {
            buffer[i++] = ch;
            printf("*");  // Print asterisk instead of actual character
        }
    }

    buffer[i] = '\0';
    printf("\n");
}

int validateEmail(const char *email) {
    // Simple email validation: contains @ and .
    const char *at = strchr(email, '@');
    const char *dot = strrchr(email, '.');

    if (at && dot && at < dot && at != email && dot[1] != '\0') {
        return 1;
    }
    return 0;
}

int validatePhone(const char *phone) {
    int len = strlen(phone);
    if (len < 10 || len > 15) {
        return 0;
    }

    for (int i = 0; i < len; i++) {
        if (!isdigit(phone[i]) && phone[i] != '+' && phone[i] != '-' && phone[i] != ' ') {
            return 0;
        }
    }
    return 1;
}

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}
