# Banking System

A comprehensive C-based banking application that simulates core banking operations using file I/O, structures, and secure input handling. This system demonstrates professional C programming practices with robust error handling and data persistence.

## 🚀 Features

- **Account Management**
  - Create new bank accounts with auto-generated account numbers
  - Secure password authentication for all transactions
  - View account information and balances

- **Financial Operations**
  - Deposit money with balance validation
  - Withdraw funds with overdraft protection
  - Transfer funds between accounts
  - Real-time balance checking

- **Security & Validation**
  - Password-protected account access
  - Input sanitization and buffer overflow protection
  - Range validation for all numeric inputs
  - Secure file-based data storage

- **Data Persistence**
  - Binary file storage for account records
  - Random access file operations for efficient updates
  - Persistent data across application sessions

## 🛠️ Technical Implementation

### Core Components
- **Structures**: `Account` structure containing account details
- **File I/O**: Binary file operations with random access
- **Input Handling**: Secure, validated user input functions
- **Error Handling**: Comprehensive error checking and recovery

### Data Structure
```c
typedef struct {
    int account_number;
    char name[MAX_NAME_LENGTH];
    double balance;
    char password[20];
} Account;
