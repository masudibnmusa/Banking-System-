#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define FILENAME "bank_accounts.dat"
#define MAX_ACCOUNTS 1000
#define MIN_ACCOUNT_NUMBER 1000
#define MAX_ACCOUNT_NUMBER 9999
#define MIN_BALANCE 0
#define MAX_NAME_LENGTH 50

// Platform-specific clear screen
#ifdef _WIN32
    #define CLEAR_SCREEN "cls"
#else
    #define CLEAR_SCREEN "clear"
#endif

// Account structure
typedef struct {
    int account_number;
    char name[MAX_NAME_LENGTH];
    double balance;
    char password[20];
} Account;

// Function prototypes
void createAccount();
void depositMoney();
void withdrawMoney();
void checkBalance();
void transferFunds();
void displayAllAccounts();
long findAccount(int account_number);
int authenticateAccount(int account_number);
int getIntInput(const char *prompt, int min, int max);
double getDoubleInput(const char *prompt, double min, double max);
void getStringInput(const char *prompt, char *buffer, int max_length);
void clearInputBuffer();
void initializeFile();
void getPasswordInput(const char *prompt, char *buffer, int max_length);
void clearScreen();
void pauseScreen();

int main() {
    int choice;

    clearScreen();
    printf("=== Welcome to Banking System ===\n");
    printf("Initializing...\n");

    // Initialize file if it doesn't exist
    initializeFile();

    // Initialize random seed once
    srand(time(NULL));

    pauseScreen();

    while (1) {
        clearScreen();
        printf("\n=== Main Menu ===\n");
        printf("1. Create New Account\n");
        printf("2. Deposit Money\n");
        printf("3. Withdraw Money\n");
        printf("4. Check Balance\n");
        printf("5. Transfer Funds\n");
        printf("6. Display All Accounts\n");
        printf("7. Exit\n");
        printf("\nEnter your choice: ");

        if (scanf("%d", &choice) != 1) {
            clearInputBuffer();
            printf("\nInvalid input! Please enter a number.\n");
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
                displayAllAccounts();
                pauseScreen();
                break;
            case 7:
                clearScreen();
                printf("\n=== Thank You ===\n");
                printf("Thank you for using our banking system!\n");
                printf("Have a great day!\n\n");
                exit(0);
            default:
                printf("\nInvalid choice! Please try again.\n");
                pauseScreen();
        }
    }

    return 0;
}

void clearScreen() {
    system(CLEAR_SCREEN);
}

void pauseScreen() {
    printf("\nPress Enter to continue...");
    clearInputBuffer();
}

void initializeFile() {
    FILE *file = fopen(FILENAME, "ab");
    if (file != NULL) {
        fclose(file);
        printf("Bank data file initialized successfully.\n");
    } else {
        printf("Error initializing bank data file.\n");
    }
}

void createAccount() {
    Account new_account;
    FILE *file;

    printf("=== Create New Account ===\n\n");

    // Generate random account number
    new_account.account_number = MIN_ACCOUNT_NUMBER + (rand() % (MAX_ACCOUNT_NUMBER - MIN_ACCOUNT_NUMBER + 1));

    // Check if account number already exists
    while (findAccount(new_account.account_number) != -1) {
        new_account.account_number = MIN_ACCOUNT_NUMBER + (rand() % (MAX_ACCOUNT_NUMBER - MIN_ACCOUNT_NUMBER + 1));
    }

    getStringInput("Enter account holder name: ", new_account.name, MAX_NAME_LENGTH);

    // Set initial balance
    new_account.balance = 0.0;

    getPasswordInput("Set account password: ", new_account.password, 20);

    // Open file in append binary mode
    file = fopen(FILENAME, "ab");
    if (file == NULL) {
        printf("\nError: Unable to open file!\n");
        return;
    }

    // Write account to file
    if (fwrite(&new_account, sizeof(Account), 1, file) == 1) {
        printf("\n");
        printf("========================================\n");
        printf("   Account Created Successfully!\n");
        printf("========================================\n");
        printf("Account Number: %d\n", new_account.account_number);
        printf("Account Holder: %s\n", new_account.name);
        printf("Initial Balance: $%.2f\n", new_account.balance);
        printf("========================================\n");
        printf("\n*** IMPORTANT: Please remember your account number and password! ***\n");
    } else {
        printf("\nError: Unable to create account!\n");
    }

    fclose(file);
}

void depositMoney() {
    int account_number;
    double amount;
    Account account;
    FILE *file;
    long position;

    printf("=== Deposit Money ===\n\n");

    account_number = getIntInput("Enter account number: ", MIN_ACCOUNT_NUMBER, MAX_ACCOUNT_NUMBER);

    if (!authenticateAccount(account_number)) {
        printf("\nAuthentication failed! Access denied.\n");
        return;
    }

    position = findAccount(account_number);
    if (position == -1) {
        printf("\nError: Account not found!\n");
        return;
    }

    amount = getDoubleInput("\nEnter amount to deposit: $", 0.01, 1000000.0);

    // Open file for reading and writing
    file = fopen(FILENAME, "r+b");
    if (file == NULL) {
        printf("\nError: Unable to open file!\n");
        return;
    }

    // Move to the account position
    fseek(file, position, SEEK_SET);
    fread(&account, sizeof(Account), 1, file);

    double old_balance = account.balance;

    // Update balance
    account.balance += amount;

    // Write back to file
    fseek(file, position, SEEK_SET);
    fwrite(&account, sizeof(Account), 1, file);

    fclose(file);

    printf("\n");
    printf("========================================\n");
    printf("     Deposit Successful!\n");
    printf("========================================\n");
    printf("Amount Deposited: $%.2f\n", amount);
    printf("Previous Balance: $%.2f\n", old_balance);
    printf("New Balance:      $%.2f\n", account.balance);
    printf("========================================\n");
}

void withdrawMoney() {
    int account_number;
    double amount;
    Account account;
    FILE *file;
    long position;

    printf("=== Withdraw Money ===\n\n");

    account_number = getIntInput("Enter account number: ", MIN_ACCOUNT_NUMBER, MAX_ACCOUNT_NUMBER);

    if (!authenticateAccount(account_number)) {
        printf("\nAuthentication failed! Access denied.\n");
        return;
    }

    position = findAccount(account_number);
    if (position == -1) {
        printf("\nError: Account not found!\n");
        return;
    }

    // Open file for reading
    file = fopen(FILENAME, "rb");
    if (file == NULL) {
        printf("\nError: Unable to open file!\n");
        return;
    }

    fseek(file, position, SEEK_SET);
    fread(&account, sizeof(Account), 1, file);
    fclose(file);

    printf("\nCurrent balance: $%.2f\n", account.balance);
    amount = getDoubleInput("Enter amount to withdraw: $", 0.01, account.balance);

    if (amount > account.balance) {
        printf("\nError: Insufficient funds!\n");
        return;
    }

    // Open file for reading and writing
    file = fopen(FILENAME, "r+b");
    if (file == NULL) {
        printf("\nError: Unable to open file!\n");
        return;
    }

    double old_balance = account.balance;

    // Update balance
    account.balance -= amount;

    // Write back to file
    fseek(file, position, SEEK_SET);
    fwrite(&account, sizeof(Account), 1, file);

    fclose(file);

    printf("\n");
    printf("========================================\n");
    printf("    Withdrawal Successful!\n");
    printf("========================================\n");
    printf("Amount Withdrawn: $%.2f\n", amount);
    printf("Previous Balance: $%.2f\n", old_balance);
    printf("New Balance:      $%.2f\n", account.balance);
    printf("========================================\n");
}

void checkBalance() {
    int account_number;
    Account account;
    FILE *file;
    long position;

    printf("=== Check Balance ===\n\n");

    account_number = getIntInput("Enter account number: ", MIN_ACCOUNT_NUMBER, MAX_ACCOUNT_NUMBER);

    if (!authenticateAccount(account_number)) {
        printf("\nAuthentication failed! Access denied.\n");
        return;
    }

    position = findAccount(account_number);
    if (position == -1) {
        printf("\nError: Account not found!\n");
        return;
    }

    file = fopen(FILENAME, "rb");
    if (file == NULL) {
        printf("\nError: Unable to open file!\n");
        return;
    }

    fseek(file, position, SEEK_SET);
    fread(&account, sizeof(Account), 1, file);
    fclose(file);

    printf("\n");
    printf("========================================\n");
    printf("      Account Information\n");
    printf("========================================\n");
    printf("Account Number:  %d\n", account.account_number);
    printf("Account Holder:  %s\n", account.name);
    printf("Current Balance: $%.2f\n", account.balance);
    printf("========================================\n");
}

void transferFunds() {
    int from_account, to_account;
    double amount;
    Account from_acc, to_acc;
    FILE *file;
    long from_position, to_position;

    printf("=== Transfer Funds ===\n\n");

    // Source account
    from_account = getIntInput("Enter your account number: ", MIN_ACCOUNT_NUMBER, MAX_ACCOUNT_NUMBER);

    if (!authenticateAccount(from_account)) {
        printf("\nAuthentication failed! Access denied.\n");
        return;
    }

    from_position = findAccount(from_account);
    if (from_position == -1) {
        printf("\nError: Your account not found!\n");
        return;
    }

    // Destination account
    to_account = getIntInput("\nEnter destination account number: ", MIN_ACCOUNT_NUMBER, MAX_ACCOUNT_NUMBER);

    to_position = findAccount(to_account);
    if (to_position == -1) {
        printf("\nError: Destination account not found!\n");
        return;
    }

    if (from_account == to_account) {
        printf("\nError: Cannot transfer to the same account!\n");
        return;
    }

    // Read source account
    file = fopen(FILENAME, "rb");
    if (file == NULL) {
        printf("\nError: Unable to open file!\n");
        return;
    }

    fseek(file, from_position, SEEK_SET);
    fread(&from_acc, sizeof(Account), 1, file);
    fclose(file);

    printf("\nYour current balance: $%.2f\n", from_acc.balance);
    amount = getDoubleInput("Enter amount to transfer: $", 0.01, from_acc.balance);

    if (amount > from_acc.balance) {
        printf("\nError: Insufficient funds!\n");
        return;
    }

    // Read destination account
    file = fopen(FILENAME, "rb");
    if (file == NULL) {
        printf("\nError: Unable to open file!\n");
        return;
    }

    fseek(file, to_position, SEEK_SET);
    fread(&to_acc, sizeof(Account), 1, file);
    fclose(file);

    double old_balance = from_acc.balance;

    // Update balances
    from_acc.balance -= amount;
    to_acc.balance += amount;

    // Write back to file
    file = fopen(FILENAME, "r+b");
    if (file == NULL) {
        printf("\nError: Unable to open file!\n");
        return;
    }

    fseek(file, from_position, SEEK_SET);
    fwrite(&from_acc, sizeof(Account), 1, file);

    fseek(file, to_position, SEEK_SET);
    fwrite(&to_acc, sizeof(Account), 1, file);

    fclose(file);

    printf("\n");
    printf("========================================\n");
    printf("     Transfer Successful!\n");
    printf("========================================\n");
    printf("Amount Transferred: $%.2f\n", amount);
    printf("From Account:       %d\n", from_account);
    printf("To Account:         %d (%s)\n", to_account, to_acc.name);
    printf("Previous Balance:   $%.2f\n", old_balance);
    printf("New Balance:        $%.2f\n", from_acc.balance);
    printf("========================================\n");
}

void displayAllAccounts() {
    Account account;
    FILE *file;
    int count = 0;
    double total_balance = 0.0;

    printf("=== All Accounts ===\n\n");

    file = fopen(FILENAME, "rb");
    if (file == NULL) {
        printf("Error: Unable to open file!\n");
        return;
    }

    printf("%-15s %-30s %-15s\n", "Account Number", "Account Holder", "Balance");
    printf("----------------------------------------------------------------\n");

    while (fread(&account, sizeof(Account), 1, file) == 1) {
        printf("%-15d %-30s $%-14.2f\n",
               account.account_number, account.name, account.balance);
        count++;
        total_balance += account.balance;
    }

    fclose(file);

    printf("----------------------------------------------------------------\n");

    if (count == 0) {
        printf("No accounts found in the system.\n");
    } else {
        printf("Total Accounts: %d\n", count);
        printf("Total Balance:  $%.2f\n", total_balance);
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

int authenticateAccount(int account_number) {
    char password[20];
    Account account;
    FILE *file;
    long position;

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

    getPasswordInput("Enter password: ", password, 20);

    if (strcmp(account.password, password) == 0) {
        return 1;
    } else {
        return 0;
    }
}

// Input helper functions
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
        printf("Invalid input! Please enter an amount between $%.2f and $%.2f.\n", min, max);
    }
}

void getStringInput(const char *prompt, char *buffer, int max_length) {
    printf("%s", prompt);
    fgets(buffer, max_length, stdin);

    // Remove newline character
    buffer[strcspn(buffer, "\n")] = '\0';

    // Remove leading/trailing whitespace
    char *start = buffer;
    while (isspace((unsigned char)*start)) start++;

    char *end = buffer + strlen(buffer) - 1;
    while (end > start && isspace((unsigned char)*end)) end--;

    *(end + 1) = '\0';

    if (start != buffer) {
        memmove(buffer, start, end - start + 2);
    }
}

void getPasswordInput(const char *prompt, char *buffer, int max_length) {
    printf("%s", prompt);
    fgets(buffer, max_length, stdin);

    // Remove newline character
    buffer[strcspn(buffer, "\n")] = '\0';
}

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}
