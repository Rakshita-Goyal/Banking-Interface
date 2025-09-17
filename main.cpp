

// #include <iostream>
// #include <sqlite3.h>
// #include <string>
// using namespace std;

// sqlite3* db;

// bool execute(const string& sql) {
//     char* errMsg = 0;
//     int rc = sqlite3_exec(db, sql.c_str(), 0, 0, &errMsg);
//     if (rc != SQLITE_OK) {
//         cerr << "SQL error: " << errMsg << endl;
//         sqlite3_free(errMsg);
//         return false;
//     }
//     return true;
// }

// bool authenticate(const string& accNo, const string& pin) {
//     string sql = "SELECT * FROM accounts WHERE acc_no = '" + accNo + "' AND pin = '" + pin + "';";
//     sqlite3_stmt* stmt;
//     int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0);
//     if (rc != SQLITE_OK) return false;
//     rc = sqlite3_step(stmt);
//     bool success = (rc == SQLITE_ROW);
//     sqlite3_finalize(stmt);
//     return success;
// }

// void init() {
//     sqlite3_open("bank.db", &db);
//     string accTable = "CREATE TABLE IF NOT EXISTS accounts("
//                       "acc_no TEXT PRIMARY KEY, "
//                       "name TEXT, "
//                       "pin TEXT, "
//                       "balance REAL, "
//                       "type TEXT);";

//     string transTable = "CREATE TABLE IF NOT EXISTS transactions("
//                         "id INTEGER PRIMARY KEY AUTOINCREMENT, "
//                         "acc_no TEXT, "
//                         "type TEXT, "
//                         "amount REAL, "
//                         "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP);";

//     execute(accTable);
//     execute(transTable);
// }

// void createAccount() {
//     string accNo, name, pin, type;
//     double amount;
//     cout << "Account Number: ";
//     cin >> accNo;
//     cout << "Holder Name: ";
//     cin.ignore();
//     getline(cin, name);
//     cout << "4-digit PIN: ";
//     cin >> pin;
//     cout << "Initial Deposit: ";
//     cin >> amount;
//     cout << "Type (savings/current): ";
//     cin >> type;

//     string sql = "INSERT INTO accounts VALUES('" + accNo + "', '" + name + "', '" + pin + "', " + to_string(amount) + ", '" + type + "');";
//     if (execute(sql)) {
//         cout << "Account created successfully.\n";
//         string trans = "INSERT INTO transactions (acc_no, type, amount) VALUES('" + accNo + "', 'create', " + to_string(amount) + ");";
//         execute(trans);
//     }
// }

// void deposit() {
//     string accNo, pin;
//     double amt;
//     cout << "Account Number: ";
//     cin >> accNo;
//     cout << "PIN: ";
//     cin >> pin;
//     if (!authenticate(accNo, pin)) {
//         cout << "Authentication failed.\n";
//         return;
//     }

//     cout << "Amount: ";
//     cin >> amt;
//     string sql = "UPDATE accounts SET balance = balance + " + to_string(amt) + " WHERE acc_no = '" + accNo + "';";
//     if (execute(sql)) {
//         cout << "Deposited: $" << amt << endl;
//         string trans = "INSERT INTO transactions (acc_no, type, amount) VALUES('" + accNo + "', 'deposit', " + to_string(amt) + ");";
//         execute(trans);
//     }
// }

// void withdraw() {
//     string accNo, pin;
//     double amt, bal = 0;
//     cout << "Account Number: ";
//     cin >> accNo;
//     cout << "PIN: ";
//     cin >> pin;
//     if (!authenticate(accNo, pin)) {
//         cout << "Authentication failed.\n";
//         return;
//     }

//     cout << "Amount: ";
//     cin >> amt;

//     string sql = "SELECT balance FROM accounts WHERE acc_no = '" + accNo + "';";
//     sqlite3_stmt* stmt;
//     sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0);
//     if (sqlite3_step(stmt) == SQLITE_ROW) {
//         bal = sqlite3_column_double(stmt, 0);
//     }
//     sqlite3_finalize(stmt);

//     if (amt > bal) {
//         cout << "Invalid withdraw amount.\n";
//         return;
//     }

//     sql = "UPDATE accounts SET balance = balance - " + to_string(amt) + " WHERE acc_no = '" + accNo + "';";
//     if (execute(sql)) {
//         cout << "Withdrew: $" << amt << ". New Balance: $" << bal - amt << endl;
//         string trans = "INSERT INTO transactions (acc_no, type, amount) VALUES('" + accNo + "', 'withdraw', " + to_string(amt) + ");";
//         execute(trans);
//     }
// }

// void transfer() {
//     string fromAcc, toAcc, pin;
//     double amt, fromBal = 0;
//     cout << "Sender Account Number: ";
//     cin >> fromAcc;
//     cout << "PIN: ";
//     cin >> pin;
//     if (!authenticate(fromAcc, pin)) {
//         cout << "Authentication failed.\n";
//         return;
//     }
//     cout << "Receiver Account Number: ";
//     cin >> toAcc;
//     cout << "Amount: ";
//     cin >> amt;

//     string checkSql = "SELECT balance FROM accounts WHERE acc_no = '" + fromAcc + "';";
//     sqlite3_stmt* stmt;
//     sqlite3_prepare_v2(db, checkSql.c_str(), -1, &stmt, 0);
//     if (sqlite3_step(stmt) == SQLITE_ROW) {
//         fromBal = sqlite3_column_double(stmt, 0);
//     }
//     sqlite3_finalize(stmt);

//     if (amt > fromBal) {
//         cout << "Insufficient funds.\n";
//         return;
//     }

//     string sql1 = "UPDATE accounts SET balance = balance - " + to_string(amt) + " WHERE acc_no = '" + fromAcc + "';";
//     string sql2 = "UPDATE accounts SET balance = balance + " + to_string(amt) + " WHERE acc_no = '" + toAcc + "';";
//     string trans1 = "INSERT INTO transactions (acc_no, type, amount) VALUES('" + fromAcc + "', 'transfer-out', " + to_string(amt) + ");";
//     string trans2 = "INSERT INTO transactions (acc_no, type, amount) VALUES('" + toAcc + "', 'transfer-in', " + to_string(amt) + ");";

//     if (execute(sql1) && execute(sql2)) {
//         execute(trans1);
//         execute(trans2);
//         cout << "Transferred $" << amt << " from " << fromAcc << " to " << toAcc << ".\n";
//     }
// }

// void viewDetails() {
//     string accNo, pin;
//     cout << "Account Number: ";
//     cin >> accNo;
//     cout << "PIN: ";
//     cin >> pin;
//     if (!authenticate(accNo, pin)) {
//         cout << "Authentication failed.\n";
//         return;
//     }

//     string sql = "SELECT name, balance, type FROM accounts WHERE acc_no = '" + accNo + "';";
//     sqlite3_stmt* stmt;
//     sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0);
//     if (sqlite3_step(stmt) == SQLITE_ROW) {
//         cout << "Account Number: " << accNo << endl;
//         cout << "Account Holder: " << sqlite3_column_text(stmt, 0) << endl;
//         cout << "Balance: $" << sqlite3_column_double(stmt, 1) << endl;
//         cout << "Type: " << sqlite3_column_text(stmt, 2) << endl;
//     }
//     sqlite3_finalize(stmt);
// }

// void viewHistory() {
//     string accNo, pin;
//     cout << "Account Number: ";
//     cin >> accNo;
//     cout << "PIN: ";
//     cin >> pin;
//     if (!authenticate(accNo, pin)) {
//         cout << "Authentication failed.\n";
//         return;
//     }

//     string sql = "SELECT type, amount, timestamp FROM transactions WHERE acc_no = '" + accNo + "' ORDER BY timestamp DESC;";
//     sqlite3_stmt* stmt;
//     sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0);
//     cout << "Transaction History:\n";
//     while (sqlite3_step(stmt) == SQLITE_ROW) {
//         cout << sqlite3_column_text(stmt, 0) << " of $" << sqlite3_column_double(stmt, 1)
//              << " on " << sqlite3_column_text(stmt, 2) << endl;
//     }
//     sqlite3_finalize(stmt);
// }

// int main() {
//     init();
//     while (true) {
//         cout << "\n--- Banking System Menu ---\n"
//              << "1. Create Account\n"
//              << "2. Deposit\n"
//              << "3. Withdraw\n"
//              << "4. Transfer\n"
//              << "5. View Details\n"
//              << "6. View History\n"
//              << "7. Exit\n"
//              << "Choice: ";
//         string choice;
//         cin >> choice;

//         if (choice == "1") createAccount();
//         else if (choice == "2") deposit();
//         else if (choice == "3") withdraw();
//         else if (choice == "4") transfer();
//         else if (choice == "5") viewDetails();
//         else if (choice == "6") viewHistory();
//         else if (choice == "7") break;
//         else cout << "Invalid choice.\n";
//     }
//     sqlite3_close(db);
//     return 0;
// }

#include <iostream>
#include <sqlite3.h>
#include <string>
using namespace std;

class BankSystem {
private:
    sqlite3* db;

    bool execute(const string& sql) {
        char* errMsg = 0;
        int rc = sqlite3_exec(db, sql.c_str(), 0, 0, &errMsg);
        if (rc != SQLITE_OK) {
            cerr << "SQL error: " << errMsg << endl;
            sqlite3_free(errMsg);
            return false;
        }
        return true;
    }

    bool authenticate(const string& accNo, const string& pin) {
        string sql = "SELECT * FROM accounts WHERE acc_no = '" + accNo + "' AND pin = '" + pin + "';";
        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0);
        if (rc != SQLITE_OK) return false;
        rc = sqlite3_step(stmt);
        bool success = (rc == SQLITE_ROW);
        sqlite3_finalize(stmt);
        return success;
    }

public:
    BankSystem() {
        sqlite3_open("bank.db", &db);
        string accTable = "CREATE TABLE IF NOT EXISTS accounts("
                          "acc_no TEXT PRIMARY KEY, "
                          "name TEXT, "
                          "pin TEXT, "
                          "balance int64_t, "
                          "type TEXT);";

        string transTable = "CREATE TABLE IF NOT EXISTS transactions("
                            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                            "acc_no TEXT, "
                            "type TEXT, "
                            "amount REAL, "
                            "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP);";

        execute(accTable);
        execute(transTable);
    }

    ~BankSystem() {
        sqlite3_close(db);
    }

    void createAccount() {
        string accNo, name, pin, type;
        double amount;
        cout << "Account Number: ";
        cin >> accNo;
        cout << "Holder Name: ";
        cin.ignore();
        getline(cin, name);
        cout << "4-digit PIN: ";
        cin >> pin;
        cout << "Initial Deposit: ";
        cin >> amount;
        cout << "Type (savings/current): ";
        cin >> type;

        string sql = "INSERT INTO accounts VALUES('" + accNo + "', '" + name + "', '" + pin + "', " + to_string(amount) + ", '" + type + "');";
        if (execute(sql)) {
            cout << "Account created successfully.\n";
            string trans = "INSERT INTO transactions (acc_no, type, amount) VALUES('" + accNo + "', 'create', " + to_string(amount) + ");";
            execute(trans);
        }
    }

    void deposit() {
        string accNo, pin;
        double amt;
        cout << "Account Number: ";
        cin >> accNo;
        cout << "PIN: ";
        cin >> pin;
        if (!authenticate(accNo, pin)) {
            cout << "Authentication failed.\n";
            return;
        }

        cout << "Amount: ";
        cin >> amt;
        string sql = "UPDATE accounts SET balance = balance + " + to_string(amt) + " WHERE acc_no = '" + accNo + "';";
        if (execute(sql)) {
            cout << "Deposited: $" << amt << endl;
            string trans = "INSERT INTO transactions (acc_no, type, amount) VALUES('" + accNo + "', 'deposit', " + to_string(amt) + ");";
            execute(trans);
        }
    }

    
    void withdraw() {
        string accNo, pin;
        double amt, bal = 0;
        cout << "Account Number: ";
        cin >> accNo;
        cout << "PIN: ";
        cin >> pin;
        if (!authenticate(accNo, pin)) {
            cout << "Authentication failed.\n";
            return;
        }

        cout << "Amount: ";
        cin >> amt;

        string sql = "SELECT balance FROM accounts WHERE acc_no = '" + accNo + "';";
        sqlite3_stmt* stmt;
        sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            bal = sqlite3_column_double(stmt, 0);
        }
        sqlite3_finalize(stmt);

        if (amt > bal) {
            cout << "Invalid withdraw amount.\n";
            return;
        }

        sql = "UPDATE accounts SET balance = balance - " + to_string(amt) + " WHERE acc_no = '" + accNo + "';";
        if (execute(sql)) {
            cout << "Withdrew: $" << amt << ". New Balance: $" << bal - amt << endl;
            string trans = "INSERT INTO transactions (acc_no, type, amount) VALUES('" + accNo + "', 'withdraw', " + to_string(amt) + ");";
            execute(trans);
        }
    }

    // void transfer() {
    //     string fromAcc, toAcc, pin;
    //     double amt, fromBal = 0;
    //     cout << "Sender Account Number: ";
    //     cin >> fromAcc;
    //     cout << "PIN: ";
    //     cin >> pin;
    //     if (!authenticate(fromAcc, pin)) {
    //         cout << "Authentication failed.\n";
    //         return;
    //     }
    //     cout << "Receiver Account Number: ";
    //     cin >> toAcc;
    //     cout << "Amount: ";
    //     cin >> amt;

    //     string checkSql = "SELECT balance FROM accounts WHERE acc_no = '" + fromAcc + "';";
    //     sqlite3_stmt* stmt;
    //     sqlite3_prepare_v2(db, checkSql.c_str(), -1, &stmt, 0);
    //     if (sqlite3_step(stmt) == SQLITE_ROW) {
    //         fromBal = sqlite3_column_double(stmt, 0);
    //     }
    //     sqlite3_finalize(stmt);

    //     if (amt > fromBal) {
    //         cout << "Insufficient funds.\n";
    //         return;
    //     }

    //     string sql1 = "UPDATE accounts SET balance = balance - " + to_string(amt) + " WHERE acc_no = '" + fromAcc + "';";
    //     string sql2 = "UPDATE accounts SET balance = balance + " + to_string(amt) + " WHERE acc_no = '" + toAcc + "';";
    //     string trans1 = "INSERT INTO transactions (acc_no, type, amount) VALUES('" + fromAcc + "', 'transfer-out', " + to_string(amt) + ");";
    //     string trans2 = "INSERT INTO transactions (acc_no, type, amount) VALUES('" + toAcc + "', 'transfer-in', " + to_string(amt) + ");";

    //     if (execute(sql1) && execute(sql2)) {
    //         execute(trans1);
    //         execute(trans2);
    //         cout << "Transferred $" << amt << " from " << fromAcc << " to " << toAcc << ".\n";
    //     }
    // }

    void transfer() {
    int fromAccount, toAccount, pin;
    double amount;

    cout << "Enter your account number: ";
    cin >> fromAccount;
    cout << "Enter PIN: ";
    cin >> pin;

    cout << "Enter receiver account number: ";
    cin >> toAccount;
    cout << "Enter amount to transfer: ";
    cin >> amount;

    sqlite3_stmt* stmt;
    string sql;

    // Start transaction
    sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr);

    // Verify PIN and balance
    sql = "SELECT balance FROM accounts WHERE account_no=" + to_string(fromAccount) + " AND pin=" + to_string(pin) + ";";
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (sqlite3_step(stmt) != SQLITE_ROW) {
        cout << "Invalid account or PIN!\n";
        sqlite3_finalize(stmt);
        sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
        return;
    }

    double balance = sqlite3_column_double(stmt, 0);
    sqlite3_finalize(stmt);

    if (balance < amount) {
        cout << "Insufficient funds!\n";
        sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
        return;
    }

    // Deduct from sender
    sql = "UPDATE accounts SET balance = balance - " + to_string(amount) + " WHERE account_no=" + to_string(fromAccount) + ";";
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        cout << "Error updating sender account!\n";
        sqlite3_finalize(stmt);
        sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
        return;
    }
    sqlite3_finalize(stmt);

    // Add to receiver
    sql = "UPDATE accounts SET balance = balance + " + to_string(amount) + " WHERE account_no=" + to_string(toAccount) + ";";
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        cout << "Error updating receiver account!\n";
        sqlite3_finalize(stmt);
        sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
        return;
    }
    sqlite3_finalize(stmt);

    // Commit transaction
    sqlite3_exec(db, "COMMIT;", nullptr, nullptr, nullptr);

    cout << "Transfer successful!\n";
}


    void viewDetails() {
        string accNo, pin;
        cout << "Account Number: ";
        cin >> accNo;
        cout << "PIN: ";
        cin >> pin;
        if (!authenticate(accNo, pin)) {
            cout << "Authentication failed.\n";
            return;
        }

        string sql = "SELECT name, balance, type FROM accounts WHERE acc_no = '" + accNo + "';";
        sqlite3_stmt* stmt;
        sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            cout << "Account Number: " << accNo << endl;
            cout << "Account Holder: " << sqlite3_column_text(stmt, 0) << endl;
            cout << "Balance: $" << sqlite3_column_double(stmt, 1) << endl;
            cout << "Type: " << sqlite3_column_text(stmt, 2) << endl;
        }
        sqlite3_finalize(stmt);
    }

    void viewHistory() {
        string accNo, pin;
        cout << "Account Number: ";
        cin >> accNo;
        cout << "PIN: ";
        cin >> pin;
        if (!authenticate(accNo, pin)) {
            cout << "Authentication failed.\n";
            return;
        }

        string sql = "SELECT type, amount, timestamp FROM transactions WHERE acc_no = '" + accNo + "' ORDER BY timestamp DESC;";
        sqlite3_stmt* stmt;
        sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0);
        cout << "Transaction History:\n";
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            cout << sqlite3_column_text(stmt, 0) << " of $" << sqlite3_column_double(stmt, 1)
                 << " on " << sqlite3_column_text(stmt, 2) << endl;
        }
        sqlite3_finalize(stmt);
    }
};

int main() {
    BankSystem bank;
    while (true) {
        cout << "\n--- Banking System Menu ---\n"
             << "1. Create Account\n"
             << "2. Deposit\n"
             << "3. Withdraw\n"
             << "4. Transfer\n"
             << "5. View Details\n"
             << "6. View History\n"
             << "7. Exit\n"
             << "Choice: ";
        string choice;
        cin >> choice;

        if (choice == "1") bank.createAccount();
        else if (choice == "2") bank.deposit();
        else if (choice == "3") bank.withdraw();
        else if (choice == "4") bank.transfer();
        else if (choice == "5") bank.viewDetails();
        else if (choice == "6") bank.viewHistory();
        else if (choice == "7") break;
        else cout << "Invalid choice.\n";
    }
    return 0;
}

