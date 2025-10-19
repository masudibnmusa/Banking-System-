# Professional Banking Management System

![C Language](https://img.shields.io/badge/Language-C-blue)
![Platform](https://img.shields.io/badge/Platform-Linux%20%7C%20Windows%20%7C%20macOS-green)
![License](https://img.shields.io/badge/License-MIT-yellow)

A sophisticated banking system implemented in C featuring comprehensive account management, transaction logging, administrative controls, and a professional user interface with color-coded output.

## ✨ Features

### 🔐 Account Management
- **Account Creation** with auto-generated 6-digit account numbers
- **Secure Authentication** with password protection and login attempts tracking
- **Account Status Management** (Active/Suspended/Closed)
- **Complete Profile Management** with email and phone validation
- **Password Change** functionality with confirmation

### 💰 Financial Operations
- **Deposit & Withdrawal** with real-time balance updates
- **Fund Transfers** between accounts with confirmation prompts
- **Balance Inquiry** with detailed account information
- **Transaction Limits** and validation checks

### 📊 Reporting & Analytics
- **Transaction History** with detailed logging
- **Account Statements** generation in text format
- **Admin Dashboard** for system overview
- **Real-time Balance Tracking**

### 🛡️ Security Features
- **Password Authentication** with attempt limiting
- **Input Validation** and sanitization
- **Transaction Logging** for audit trails
- **Secure File Operations**
- **Data Persistence** with binary file storage

### 🎨 User Experience
- **Color-coded Interface** with ANSI colors
- **Cross-platform Compatibility** (Windows/Linux/macOS)
- **Professional UI** with headers and separators
- **Clear Navigation** and menu system

## 🏗️ System Architecture

### Data Structures
```c
typedef struct {
    int account_number;
    char name[MAX_NAME_LENGTH];
    char email[MAX_NAME_LENGTH];
    char phone[20];
    double balance;
    char password[PASSWORD_LENGTH];
    AccountStatus status;
    time_t created_date;
    time_t last_accessed;
    int failed_login_attempts;
} Account;

typedef struct {
    int transaction_id;
    int account_number;
    TransactionType type;
    double amount;
    double balance_after;
    int related_account;
    time_t timestamp;
    char description[100];
} Transaction;
