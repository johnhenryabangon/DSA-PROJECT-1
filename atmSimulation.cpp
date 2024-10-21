#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <cstdlib>
#include <conio.h>

using namespace std;

const int MAX_ACCOUNTS = 100; // MAXIMUM NUMBER OF ACCOUNTS
const int MIN_DEPOSIT = 5000; // MINIMUM DEPOSIT
const int PIN_LENGTH = 6;
const int ACCOUNT_NUMBER_LENGTH = 5; // EXPECTED LENGTH FOR THE ACCOUNG NUMBER
const int CONTACT_NUMBER_LENGTH = 11; // MAX LENGTH OF CONTACT NUMBER

struct Account {
    int accountNumber;
    string accountName;
    string birthday;
    string contactNumber;
    int balance;
    string pinCode; // ENCRYPTED PIN CODE
};

Account accounts[MAX_ACCOUNTS];
int accountCount = 0;

// FUNCTION TO CHECK IF CONTACT NUMBER IS VALID
bool isValidContactNumber(const string& contactNumber){
    if (contactNumber.length() != CONTACT_NUMBER_LENGTH){
        return false;
    }
    for (char ch : contactNumber){
        if (!isdigit(ch)){
            return false;
        }
    }
    return true;
}

// FUNCTION FOR ENCRYPTION
string encryptPin(string pin) {
    char key = 'K'; // Simple key for XOR encryption
    string encryptedPin = pin;
    for (size_t i = 0; i < pin.size(); i++) {
        encryptedPin[i] = pin[i] ^ key;
    }
    return encryptedPin;
}

//  FUNCTION TO CHECK IF AN ACCOUNT EXIST BY ACCOUNT NUMNBER
int findAccount(int accountNumber){
    for (int i=0; i<accountCount; i++){
        if (accounts[i].accountNumber == accountNumber){
            return i;
        }
    }
    return -1;
}

// Function to create ATM card (file) on the flash drive - for individual account
bool createATMCardFile(const Account &account) {
    string usbPath = "D:/";
    string filename = to_string(account.accountNumber) + "_card.txt";
    ofstream cardFile(usbPath + filename);

    cout << "\n===========================================" << endl;
    cout << "         CREATING ATM CARD FILE           " << endl;
    cout << "===========================================\n" << endl;

    if (cardFile) {
        // Write account details to the file
        cardFile << account.accountNumber << endl;
        cardFile << account.accountName << endl;
        cardFile << account.birthday << endl;
        cardFile << account.contactNumber << endl;
        cardFile << account.pinCode << endl;
        cardFile << account.balance << endl;
        cardFile.close();

        cout << "ATM card file created successfully at: " << endl;
        return true;
    } else {
        // Error message
        cout << "Error: Unable to create ATM card file." << endl;
        cout << "There is no card detected." << endl;
        return false;
    }

    cout << "\n===========================================" << endl;
    system("pause");  }

// FUNCTION TO READ ATM CARD FILE
bool readATMCardFile(int accountNumber, const string &enteredPin, Account &loadedAccount) {
    string usbPath = "D:/";
    string filename = to_string(accountNumber) + "_card.txt";
    ifstream cardFile(usbPath + filename);

    if (!cardFile) {
        cout << "\nATM card not found. Please insert the correct card.\n\n" << endl;
        return false;
    }

    cardFile >> loadedAccount.accountNumber;
    cardFile.ignore(); // To skip newline after number
    getline(cardFile, loadedAccount.accountName);
    getline(cardFile, loadedAccount.birthday);
    getline(cardFile, loadedAccount.contactNumber);
    cardFile >> loadedAccount.pinCode;
    cardFile >> loadedAccount.balance;

    if (encryptPin(enteredPin) == loadedAccount.pinCode) {
        return true;
    } else {
        cout << "Incorrect PIN." << endl;
        return false;
    }
}

// Function to update the ATM card (file) after transactions
void updateATMCardFile(const Account &account) {
    string usbPath = "D:/";
    string filename = to_string(account.accountNumber) + "_card.txt";
    ofstream cardFile(usbPath + filename);

    cout << "\n===========================================" << endl;
    cout << "           UPDATING ATM CARD FILE         " << endl;
    cout << "===========================================\n" << endl;

    if (cardFile) {
        cardFile << account.accountNumber << endl;
        cardFile << account.accountName << endl;
        cardFile << account.birthday << endl;
        cardFile << account.contactNumber << endl;
        cardFile << account.pinCode << endl;
        cardFile << account.balance << endl;
        cardFile.close();

        cout << "ATM card file updated successfully" << endl;
    } else {
        cout << "Error: Unable to update ATM card file." << endl;
    }

    cout << "\n===========================================" << endl;
    system("pause");
}

// FUNCTION TO SAVE ALL ACCOUNTS - list of all accounts
void saveAccountsToFile() {
    ofstream outFile("D:/accounts_data.txt");

    if (outFile.is_open()) {
        outFile << accountCount << endl;

        for (int i = 0; i < accountCount; i++) {
            outFile << accounts[i].accountNumber << endl;
            outFile << accounts[i].accountName << endl;
            outFile << accounts[i].birthday << endl;
            outFile << accounts[i].contactNumber << endl;
            outFile << accounts[i].pinCode << endl;
            outFile << accounts[i].balance << endl;
        }

        outFile.close();
        cout << "\n===========================================" << endl;
        cout << "         ACCOUNTS SAVED SUCCESSFULLY       " << endl;
        cout << "===========================================\n" << endl;
    } else {
        cout << "\n===========================================" << endl;
        cout << "          ERROR: FAILED TO SAVE DATA       " << endl;
        cout << "===========================================\n" << endl;
        cout << "There was an issue saving the account.\n" << endl;
    }

    system("pause");
}

// FUNCTION TO LOAD ACCOUNTS
void loadAccountsFromFile() {
    ifstream inFile("D:/accounts_data.txt");
    if (inFile) {
        inFile >> accountCount;
        inFile.ignore();
        for (int i = 0; i < accountCount; i++) {
            inFile >> accounts[i].accountNumber;
            inFile.ignore();
            getline(inFile, accounts[i].accountName);
            getline(inFile, accounts[i].birthday);
            getline(inFile, accounts[i].contactNumber);
            getline(inFile, accounts[i].pinCode);
            inFile >> accounts[i].balance;
        }
        inFile.close();
        cout << "Accounts loaded successfully." << endl;
    } else {
        cout << "No previous account data found. Starting." << endl;
    }
}

// FUNCTION TO SECURELY INPUT PIN
string getHiddenPin() {
    string pin;
    char ch;
    while (pin.length() < PIN_LENGTH) {
        ch = getch(); // Get individual character without showing it on the console
        if (ch >= '0' && ch <= '9') {
            pin.push_back(ch);
            cout << '*'; // Display a '*' instead of the character
        } else if (ch == '\b' && !pin.empty()) {  // Handle backspace
            pin.pop_back();
            cout << "\b \b"; // Erase last '*'
        }
    }
    cout << endl;
    return pin;
}

// REGISTRATION MODULE - ENROLL NEW ACCOUNTS
void registerAccount() {
    if (accountCount >= MAX_ACCOUNTS) {
        cout << "\n==========================================" << endl;
        cout << "     Cannot create more accounts.          " << endl;
        cout << "       Maximum limit reached.              " << endl;
        cout << "==========================================\n" << endl;
        system("pause");
        return;
    }

    system("cls");
    Account newAccount;
    string accountNumberStr;

    // ADD: Option to cancel the registration before starting
    char confirm;

    cout << "\n==========================================" << endl;
    cout << "        WELCOME TO POWER BANK              " << endl;
    cout << "==========================================\n" << endl;

    cout << "ATM Account Registration" << endl;
    cout << "Do you want to continue with registration?" << endl;
    cout << " (1 to proceed / 0 to cancel): ";
    cin >> confirm;

    if (tolower(confirm) == '0') {  // If the user chooses to cancel
        cout << "\n==========================================" << endl;
        cout << "     Registration canceled. Returning      " << endl;
        cout << "         to main menu.                     " << endl;
        cout << "==========================================\n" << endl;
        system("pause");
        return;  // Exit the function, returning to main menu
    } else if (tolower(confirm) == '1') {

        // Proceed with registration if user confirms
        do {
            cout << "\nEnter Account Number (exactly 5 digits): ";
            cin >> accountNumberStr;

            // Check if the input is exactly 5 digits
            if (accountNumberStr.length() != ACCOUNT_NUMBER_LENGTH || !isdigit(accountNumberStr[0])) {
                cout << "Error: Account number must be exactly 5 digits. Try again." << endl;
            }
            else if (findAccount(stoi(accountNumberStr)) != -1) {  // Check for uniqueness
                cout << "Error: Account number is already registered. Try another one." << endl;
            }
            else {
                newAccount.accountNumber = stoi(accountNumberStr); // Convert the valid string to integer
                break;
            }
        } while (true);  // Repeat until a valid, unique account number is entered

        // Continue with other account registration details...
        cout << "Enter Account Name: ";
        cin.ignore();  // Clear the input buffer
        getline(cin, newAccount.accountName);

        cout << "Enter Birthday (dd/mm/yyyy): ";
        getline(cin, newAccount.birthday);

        string contactNumber;
        do {
            cout << "Enter Contact Number (11 digits): ";
            getline(cin, contactNumber);

            if (!isValidContactNumber(contactNumber)) {
                cout << "Invalid contact number. Please enter a valid 11-digit number." << endl;
            }
        } while (!isValidContactNumber(contactNumber));

        newAccount.contactNumber = contactNumber;

        // Ensure a valid deposit amount
        while (true) {
            cout << "Enter Initial Deposit (minimum " << MIN_DEPOSIT << "): ";
            cin >> newAccount.balance;

            if (newAccount.balance < MIN_DEPOSIT) {
                cout << "Initial deposit must be at least " << MIN_DEPOSIT << ". Try again." << endl;
            } else {
                break;
            }
        }

        system("pause");

        string pin;
        cout << "Set a 6-digit PIN: ";
        pin = getHiddenPin();  // Capture hidden PIN input
        newAccount.pinCode = encryptPin(pin);

        // Add account to the list and save to file
        accounts[accountCount++] = newAccount;
        if (createATMCardFile(newAccount)) {
            system("cls");
            cout << "\n==========================================" << endl;
            cout << "      Account registered successfully!     " << endl;
            cout << " Enjoy powerful transactions with POWER BANK!" << endl;
            cout << "==========================================\n" << endl;
        } else {
            // If file creation failed
            accountCount--; // Reduce account count since registration failed
            cout << "\n==========================================" << endl;
            cout << "       Error registering account.          " << endl;
            cout << "  Please try registering again later.      " << endl;
            cout << "==========================================\n" << endl;
        }

    } else {
        cout << "\n==========================================" << endl;
        cout << "       Error. Returning to main menu.      " << endl;
        cout << "==========================================\n" << endl;
        system("pause");
        return;
    }

    cout << "IMPORTANT: Ensure you exit the program properly to save your account data." << endl;
    cout << "Failure to exit properly may result in data loss.\n" << endl;
    system("pause");
}


// FUNCTION TO DELETE ACCOUNT
void deleteAccount() {
    int accountNumber;
    system("cls");

    cout << "\n==========================================" << endl;
    cout << "          DELETE ATM ACCOUNT              " << endl;
    cout << "==========================================\n" << endl;

    cout << "Enter the account number to delete: ";
    cin >> accountNumber;

    int accountIndex = findAccount(accountNumber);

    if (accountIndex == -1) {
        cout << "\n==========================================" << endl;
        cout << "            ACCOUNT NOT FOUND             " << endl;
        cout << "==========================================\n" << endl;
        system("pause");
        return;
    }

    // Display account details before deletion
    cout << "\nAccount Found:\n";
    cout << "Account Number : " << accounts[accountIndex].accountNumber << endl;
    cout << "Account Name   : " << accounts[accountIndex].accountName << endl;
    cout << "Balance        : PHP " << accounts[accountIndex].balance << "\n" << endl;

    // Confirm deletion
    char confirm;
    cout << "Are you sure you want to delete this account? (1 to confirm / 0 to cancel): ";
    cin >> confirm;

    if (tolower(confirm) == '0') {
        cout << "\nDeletion canceled. Returning to main menu.\n" << endl;
        system("pause");
        return;
    } else if (tolower(confirm) == '1') {
        for (int i = accountIndex; i < accountCount - 1; i++) {
            accounts[i] = accounts[i + 1];
        }
        accountCount--;

        cout << "\n==========================================" << endl;
        cout << "         ACCOUNT DELETED SUCCESSFULLY     " << endl;
        cout << "==========================================\n" << endl;

        // Delete ATM card individual file
        string filename = "D:/" + to_string(accountNumber) + "_card.txt";
        if (remove(filename.c_str()) != 0) {
            cout << "Error deleting ATM card file." << endl;
        } else {
            cout << "ATM card file deleted successfully." << endl;
        }

        // Save updated accounts to file
        saveAccountsToFile();
    } else {
        cout << "\nInvalid input. Returning to main menu.\n" << endl;
    }

    system("pause");
}

// CHANGE PIN
void changePin(int accountIndex) {
    string newPin, confirmPin;
    system("cls");

    cout << "\n==========================================" << endl;
    cout << "         POWER BANK - CHANGE PIN          " << endl;
    cout << "==========================================\n" << endl;

    // Ensure the user inputs a valid 6-digit PIN
    do {
        cout << "Enter new 6-digit PIN: ";
        newPin = getHiddenPin();  // Capture hidden PIN input

        if (newPin.length() != 6 || !isdigit(newPin[0])) {
            cout << "\n==========================================" << endl;
            cout << "         ERROR: INVALID PIN LENGTH        " << endl;
            cout << "==========================================\n" << endl;
            cout << "PIN must be exactly 6 digits. Please try again.\n" << endl;
        }
    } while (newPin.length() != 6 || !isdigit(newPin[0]));

    // Confirm the new PIN to avoid mistakes
    cout << "\nConfirm new 6-digit PIN: ";
    confirmPin = getHiddenPin();

    if (newPin != confirmPin) {
        cout << "\n==========================================" << endl;
        cout << "          ERROR: PINS DO NOT MATCH        " << endl;
        cout << "==========================================\n" << endl;
        cout << "PIN change failed. Please try again.\n" << endl;
    } else {
        // Update the PIN if valid and confirmed
        accounts[accountIndex].pinCode = encryptPin(newPin);
        updateATMCardFile(accounts[accountIndex]);

        cout << "\n==========================================" << endl;
        cout << "           PIN CHANGED SUCCESSFULLY        " << endl;
        cout << "==========================================\n" << endl;
    }

    system("pause");
}

// BALANCE INQUIRY
void balanceInquiry(int accountIndex) {
    system("cls");

    cout << "\n==========================================" << endl;
    cout << "          POWER BANK SAVINGS INQUIRY       " << endl;
    cout << "==========================================\n" << endl;

    cout << "Account Details" << endl;
    cout << "------------------------------------------" << endl;
    cout << "Account Number : " << accounts[accountIndex].accountNumber << endl;
    cout << "Account Name   : " << accounts[accountIndex].accountName << endl;
    cout << "Current Balance: PHP " << accounts[accountIndex].balance << endl;
    cout << "------------------------------------------\n" << endl;

    cout << "Note: Please make sure to properly exit the system to ensure data is saved." << endl;

    cout << "\n==========================================" << endl;
    cout << "           Thank you for using            " << endl;
    cout << "          POWER BANK ATM SERVICE          " << endl;
    cout << "==========================================" << endl;

    system("pause");
}

// WITHDRAW MONEY
void withdrawMoney(int accountIndex) {
    int amount;
    system("cls");

    cout << "\n==========================================" << endl;
    cout << "          POWER BANK - WITHDRAWAL          " << endl;
    cout << "==========================================\n" << endl;

    cout << "Account Number : " << accounts[accountIndex].accountNumber << endl;
    cout << "Account Name   : " << accounts[accountIndex].accountName << endl;
    cout << "------------------------------------------" << endl;
    cout << "Current Balance: PHP " << accounts[accountIndex].balance << endl;
    cout << "------------------------------------------" << endl;

    cout << "\nEnter amount to withdraw: PHP ";
    cin >> amount;

    system("pause");

    // Check for sufficient funds
    if (amount > accounts[accountIndex].balance) {
        cout << "\n==========================================" << endl;
        cout << "         ERROR: INSUFFICIENT FUNDS         " << endl;
        cout << "==========================================" << endl;
        cout << "You do not have enough balance to withdraw that amount.\n" << endl;
    } else {
        // Successful withdrawal
        accounts[accountIndex].balance -= amount;
        updateATMCardFile(accounts[accountIndex]);

        cout << "\n==========================================" << endl;
        cout << "       WITHDRAWAL SUCCESSFUL               " << endl;
        cout << "==========================================" << endl;
        cout << "Amount Withdrawn: PHP " << amount << endl;
        cout << "New Balance     : PHP " << accounts[accountIndex].balance << endl;
        cout << "==========================================\n" << endl;
    }

    cout << "Please take your cash. Thank you for using POWER BANK." << endl;

    system("pause");
}
// DEPOSIT MONEY
void depositMoney(int accountIndex) {
    int amount;
    system("cls");

    cout << "\n==========================================" << endl;
    cout << "           POWER BANK - DEPOSIT           " << endl;
    cout << "==========================================\n" << endl;

    cout << "Account Number : " << accounts[accountIndex].accountNumber << endl;
    cout << "Account Name   : " << accounts[accountIndex].accountName << endl;
    cout << "------------------------------------------" << endl;
    cout << "Current Balance: PHP " << accounts[accountIndex].balance << endl;
    cout << "------------------------------------------" << endl;

    cout << "\nEnter amount to deposit: PHP ";
    cin >> amount;

    // Validate deposit amount
    if (amount <= 0) {
        cout << "\n==========================================" << endl;
        cout << "           ERROR: INVALID AMOUNT           " << endl;
        cout << "==========================================\n" << endl;
        cout << "Deposit amount must be greater than 0.\n" << endl;
    } else {
        accounts[accountIndex].balance += amount;
        updateATMCardFile(accounts[accountIndex]);  // Update the file after deposit

        cout << "\n==========================================" << endl;
        cout << "        DEPOSIT SUCCESSFUL                 " << endl;
        cout << "==========================================\n" << endl;
        cout << "Amount Deposited: PHP " << amount << endl;
        cout << "New Balance     : PHP " << accounts[accountIndex].balance << endl;
        cout << "==========================================\n" << endl;
    }

    system("pause");
}

// FUNCTION FOR FUND TRANSFER
void fundTransfer(int accountIndex) {
    int targetAccountNumber, transferAmount;

    // Display transfer fund menu
    system("cls");
    cout << "\n===========================================" << endl;
    cout << "         POWER BANK - FUND TRANSFER        " << endl;
    cout << "===========================================\n" << endl;

    // Ask for the target account number
    cout << "Enter target account number for transfer: ";
    cin >> targetAccountNumber;

    // Find the target account
    int targetIndex = findAccount(targetAccountNumber);
    if (targetIndex == -1) {
        cout << "\n===========================================" << endl;
        cout << "           ERROR: ACCOUNT NOT FOUND        " << endl;
        cout << "===========================================\n" << endl;
        cout << "The target account number does not exist. Please try again.\n" << endl;
        system("pause");
        return;
    }

    cout << "Enter amount to transfer: ";
    cin >> transferAmount;

    cout << "\n-------------------------------------------" << endl;

    // Check if the source account has enough funds
    if (transferAmount > accounts[accountIndex].balance) {
        cout << "\n===========================================" << endl;
        cout << "          ERROR: INSUFFICIENT FUNDS         " << endl;
        cout << "===========================================\n" << endl;
        cout << "You do not have enough balance for this transaction.\n" << endl;
        system("pause");
        return;
    }

    // Perform the transfer
    accounts[accountIndex].balance -= transferAmount;
    accounts[targetIndex].balance += transferAmount;

    updateATMCardFile(accounts[accountIndex]);
    updateATMCardFile(accounts[targetIndex]);

    cout << "\n===========================================" << endl;
    cout << "            TRANSFER SUCCESSFUL!            " << endl;
    cout << "===========================================\n" << endl;
    cout << "Amount Transferred: " << transferAmount << endl;
    cout << "Your new balance: " << accounts[accountIndex].balance << endl;
    cout << "Target account new balance: " << accounts[targetIndex].balance << "\n" << endl;

    system("pause");
}


// FUNCTION TO SIMULATE ATM CARD INSERTION
int verifyCard(int accountNumber, string enteredPin) {
    Account loadedAccount;
    if (readATMCardFile(accountNumber, enteredPin, loadedAccount)) {
        // Update the internal list with the loaded account details
        int accountIndex = findAccount(accountNumber);
        if (accountIndex != -1) {
            accounts[accountIndex] = loadedAccount;
            return accountIndex;
        }
    }
    return -1;
}

// MAIN MENU FOR ATM
void atmMenu() {
    int accountNumber;
    string pin;
    system("cls");

    cout << "\n==========================================" << endl;
    cout << "     Welcome to POWER BANK ATM MACHINE       " << endl;
    cout << "===========================================\n" << endl;

    cout << "\nPlease insert your card (enter account number): ";
    cin >> accountNumber;

    cout << "Enter your PIN: ";
    pin = getHiddenPin();  // Capture hidden PIN input

    int accountIndex = verifyCard(accountNumber, pin);
    if (accountIndex == -1) return; // Invalid card or PIN

    int choice;
    do {
        system("cls");
        cout << "\n==========================================" << endl;
        cout << "         POWER BANK ATM MACHINE MENU        " << endl;
        cout << "===========================================\n" << endl;

        cout << "IMPORTANT: Ensure you exit the program properly after every transaction to save your account data." << endl;
        cout << "Failure to exit properly may result in data loss.\n\n";
        system("pause");
        system("cls");

        cout << "\n------------------------------------------" << endl;
        cout << "                ATM MAIN MENU               " << endl;
        cout << "-------------------------------------------\n" << endl;

        cout << "  1. Balance Inquiry\n";
        cout << "  2. Withdraw Money\n";
        cout << "  3. Deposit Money\n";
        cout << "  4. Change PIN\n";
        cout << "  5. Fund Transfer\n";
        cout << "  6. Exit\n";
        cout << "\n------------------------------------------" << endl;
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                system("cls");
                cout <<"\n";
                cout << "==========================================" << endl;
                cout << "             BALANCE INQUIRY              " << endl;
                cout << "==========================================\n" << endl;
                balanceInquiry(accountIndex);
                break;
            case 2:
                system("cls");
                cout << "\n==========================================" << endl;
                cout << "             WITHDRAW MONEY                " << endl;
                cout << "==========================================\n" << endl;
                withdrawMoney(accountIndex);
                break;
            case 3:
                system("cls");
                cout << "\n==========================================" << endl;
                cout << "             DEPOSIT MONEY                 " << endl;
                cout << "==========================================\n" << endl;
                depositMoney(accountIndex);
                break;
            case 4:
                system("cls");
                cout << "\n==========================================" << endl;
                cout << "              CHANGE PIN                   " << endl;
                cout << "==========================================\n" << endl;
                changePin(accountIndex);
                break;
            case 5:
                system("cls");
                cout << "\n==========================================" << endl;
                cout << "            FUND TRANSFER                  " << endl;
                cout << "==========================================\n" << endl;
                fundTransfer(accountIndex);
                break;
            case 6:
                system("cls");
                cout << "\n============================================" << endl;
                cout << " Thank you for using POWER BANK ATM MACHINE " << endl;
                cout << "============================================\n" << endl;
                break;
            default:
                cout << "\n------------------------------------------" << endl;
                cout << "             Invalid choice." << endl;
                cout << "------------------------------------------\n" << endl;
        }
        system("pause");
    } while (choice != 6);
}

// MAIN FUNCTION
int main() {
    // Load account data from file at startup
    loadAccountsFromFile();

    int option;
    do {
        system("cls");
        cout << "\n==========================================" << endl;
        cout << "         WELCOME TO POWER BANK            " << endl;
        cout << "==========================================\n" << endl;

        cout << "           Main Menu Options:             " << endl;
        cout << "------------------------------------------\n" << endl;
        cout << "  1. Register Account" << endl;
        cout << "  2. Access ATM" << endl;
        cout << "  3. Delete Account" << endl;  // Added delete account option
        cout << "  4. Exit" << endl;
        cout << "\n------------------------------------------" << endl;
        cout << "Enter choice: ";
        cin >> option;

        switch (option) {
            case 1:
                system("cls");
                cout << "\n==========================================" << endl;
                cout << "            REGISTER ACCOUNT              " << endl;
                cout << "==========================================\n" << endl;
                registerAccount();
                break;
            case 2:
                system("cls");
                atmMenu();
                break;
            case 3:
                system("cls");
                cout << "\n==========================================" << endl;
                cout << "             DELETE ACCOUNT               " << endl;
                cout << "==========================================\n" << endl;
                deleteAccount(); // Call the new deleteAccount function
                break;
            case 4:
                system("cls");
                // Save account data to file before exiting
                saveAccountsToFile();
                cout << "\n==========================================" << endl;
                cout << "    Exiting POWER BANK ATM MACHINE        " << endl;
                cout << "==========================================\n" << endl;
                break;
            default:
                cout << "\n------------------------------------------" << endl;
                cout << "            Invalid choice. Try again." << endl;
                cout << "------------------------------------------\n" << endl;
        }
        system("pause");
    } while (option != 4);

    return 0;
}
