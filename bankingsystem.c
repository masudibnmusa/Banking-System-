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

int main() {
    int choice;

    printf("=== Welcome to Banking System ===\n");

    // Initialize file if it doesn't exist
    initializeFile();

    while (1) {
        printf("\n=== Main Menu ===\n");
        printf("1. Create New Account\n");
        printf("2. Deposit Money\n");
        printf("3. Withdraw Money\n");
        printf("4. Check Balance\n");
        printf("5. Transfer Funds\n");
        printf("6. Display All Accounts\n");
        printf("7. Exit\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input! Please enter a number.\n");
            clearInputBuffer();
            continue;
        }
        clearInputBuffer(); // Clear the buffer after menu input

        switch (choice) {
            case 1:
                createAccount();
                break;
            case 2:
                depositMoney();
                break;
            case 3:
                withdrawMoney();
                break;
            case 4:
                checkBalance();
                break;
            case 5:
                transferFunds();
                break;
            case 6:
                displayAllAccounts();
                break;
            case 7:
                printf("Thank you for using our banking system!\n");
                exit(0);
            default:
                printf("Invalid choice! Please try again.\n");
        }
    }

    return 0;
}

void initializeFile() {
    FILE *file = fopen(FILENAME, "ab");
    if (file != NULL) {
        fclose(file);
        printf("Bank data file initialized.\n");
    } else {
        printf("Error initializing bank data file.\n");
    }
}

void createAccount() {
    Account new_account;
    FILE *file;

    printf("\n=== Create New Account ===\n");

    // Generate random account number
    srand(time(NULL));
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
        printf("Error opening file!\n");
        return;
    }

    // Write account to file
    if (fwrite(&new_account, sizeof(Account), 1, file) == 1) {
        printf("\nAccount created successfully!\n");
        printf("Account Number: %d\n", new_account.account_number);
        printf("Account Holder: %s\n", new_account.name);
        printf("Initial Balance: $%.2f\n", new_account.balance);
        printf("Please remember your account number and password!\n");
    } else {
        printf("Error creating account!\n");
    }

    fclose(file);
}

void depositMoney() {
    int account_number;
    double amount;
    Account account;
    FILE *file;
    long position;

    printf("\n=== Deposit Money ===\n");

    account_number = getIntInput("Enter account number: ", MIN_ACCOUNT_NUMBER, MAX_ACCOUNT_NUMBER);

    if (!authenticateAccount(account_number)) {
        printf("Authentication failed!\n");
        return;
    }

    position = findAccount(account_number);
    if (position == -1) {
        printf("Account not found!\n");
        return;
    }

    amount = getDoubleInput("Enter amount to deposit: $", 0.01, 1000000.0);

    // Open file for reading and writing
    file = fopen(FILENAME, "r+b");
    if (file == NULL) {
        printf("Error opening file!\n");
        return;
    }

    // Move to the account position
    fseek(file, position, SEEK_SET);
    fread(&account, sizeof(Account), 1, file);

    // Update balance
    account.balance += amount;

    // Write back to file
    fseek(file, position, SEEK_SET);
    fwrite(&account, sizeof(Account), 1, file);

    fclose(file);

    printf("Deposit successful!\n");
    printf("New balance: $%.2f\n", account.balance);
}

void withdrawMoney() {
    int account_number;
    double amount;
    Account account;
    FILE *file;
    long position;

    printf("\n=== Withdraw Money ===\n");

    account_number = getIntInput("Enter account number: ", MIN_ACCOUNT_NUMBER, MAX_ACCOUNT_NUMBER);

    if (!authenticateAccount(account_number)) {
        printf("Authentication failed!\n");
        return;
    }

    position = findAccount(account_number);
    if (position == -1) {
        printf("Account not found!\n");
        return;
    }

    // Open file for reading
    file = fopen(FILENAME, "rb");
    if (file == NULL) {
        printf("Error opening file!\n");
        return;
    }

    fseek(file, position, SEEK_SET);
    fread(&account, sizeof(Account), 1, file);
    fclose(file);

    printf("Current balance: $%.2f\n", account.balance);
    amount = getDoubleInput("Enter amount to withdraw: $", 0.01, account.balance);

    if (amount > account.balance) {
        printf("Insufficient funds!\n");
        return;
    }

    // Open file for reading and writing
    file = fopen(FILENAME, "r+b");
    if (file == NULL) {
        printf("Error opening file!\n");
        return;
    }

    // Update balance
    account.balance -= amount;

    // Write back to file
    fseek(file, position, SEEK_SET);
    fwrite(&account, sizeof(Account), 1, file);

    fclose(file);

    printf("Withdrawal successful!\n");
    printf("New balance: $%.2f\n", account.balance);
}

void checkBalance() {
    int account_number;
    Account account;
    FILE *file;
    long position;

    printf("\n=== Check Balance ===\n");

    account_number = getIntInput("Enter account number: ", MIN_ACCOUNT_NUMBER, MAX_ACCOUNT_NUMBER);

    if (!authenticateAccount(account_number)) {
        printf("Authentication failed!\n");
        return;
    }

    position = findAccount(account_number);
    if (position == -1) {
        printf("Account not found!\n");
        return;
    }

    file = fopen(FILENAME, "rb");
    if (file == NULL) {
        printf("Error opening file!\n");
        return;
    }

    fseek(file, position, SEEK_SET);
    fread(&account, sizeof(Account), 1, file);
    fclose(file);

    printf("\nAccount Information:\n");
    printf("Account Number: %d\n", account.account_number);
    printf("Account Holder: %s\n", account.name);
    printf("Current Balance: $%.2f\n", account.balance);
}

void transferFunds() {
    int from_account, to_account;
    double amount;
    Account from_acc, to_acc;
    FILE *file;
    long from_position, to_position;

    printf("\n=== Transfer Funds ===\n");

    // Source account
    from_account = getIntInput("Enter your account number: ", MIN_ACCOUNT_NUMBER, MAX_ACCOUNT_NUMBER);

    if (!authenticateAccount(from_account)) {
        printf("Authentication failed!\n");
        return;
    }

    from_position = findAccount(from_account);
    if (from_position == -1) {
        printf("Your account not found!\n");
        return;
    }

    // Destination account
    to_account = getIntInput("Enter destination account number: ", MIN_ACCOUNT_NUMBER, MAX_ACCOUNT_NUMBER);

    to_position = findAccount(to_account);
    if (to_position == -1) {
        printf("Destination account not found!\n");
        return;
    }

    if (from_account == to_account) {
        printf("Cannot transfer to the same account!\n");
        return;
    }

    // Read source account
    file = fopen(FILENAME, "rb");
    if (file == NULL) {
        printf("Error opening file!\n");
        return;
    }

    fseek(file, from_position, SEEK_SET);
    fread(&from_acc, sizeof(Account), 1, file);
    fclose(file);

    printf("Your current balance: $%.2f\n", from_acc.balance);
    amount = getDoubleInput("Enter amount to transfer: $", 0.01, from_acc.balance);

    if (amount > from_acc.balance) {
        printf("Insufficient funds!\n");
        return;
    }

    // Read destination account
    file = fopen(FILENAME, "rb");
    if (file == NULL) {
        printf("Error opening file!\n");
        return;
    }

    fseek(file, to_position, SEEK_SET);
    fread(&to_acc, sizeof(Account), 1, file);
    fclose(file);

    // Update balances
    from_acc.balance -= amount;
    to_acc.balance += amount;

    // Write back to file
    file = fopen(FILENAME, "r+b");
    if (file == NULL) {
        printf("Error opening file!\n");
        return;
    }

    fseek(file, from_position, SEEK_SET);
    fwrite(&from_acc, sizeof(Account), 1, file);

    fseek(file, to_position, SEEK_SET);
    fwrite(&to_acc, sizeof(Account), 1, file);

    fclose(file);

    printf("Transfer successful!\n");
    printf("Transferred $%.2f from account %d to account %d\n", amount, from_account, to_account);
    printf("Your new balance: $%.2f\n", from_acc.balance);
}

void displayAllAccounts() {
    Account account;
    FILE *file;
    int count = 0;

    printf("\n=== All Accounts ===\n");

    file = fopen(FILENAME, "rb");
    if (file == NULL) {
        printf("Error opening file!\n");
        return;
    }

    printf("\n%-15s %-20s %-15s\n", "Account Number", "Account Holder", "Balance");
    printf("-------------------------------------------------\n");

    while (fread(&account, sizeof(Account), 1, file) == 1) {
        printf("%-15d %-20s $%-14.2f\n",
               account.account_number, account.name, account.balance);
        count++;
    }

    fclose(file);

    if (count == 0) {
        printf("No accounts found.\n");
    } else {
        printf("\nTotal accounts: %d\n", count);
    }
}

long findAccount(int account_number) {
    Account account;
    FILE *file;
    long position = 0;

    file = fopen(FILENAME, "rb");
    if (file == NULL) {
        return -1;
    }

    // Start from beginning
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
        printf("Incorrect password!\n");
        return 0;
    }
}

// Secure input functions
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
