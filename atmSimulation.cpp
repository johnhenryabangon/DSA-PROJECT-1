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

ACcount accounts[MAX_ACCOUNTS];
int accountCount = 0;

// FUNCTION TO CHECK IF CONTACT NUMBER IS VALID
bool isValidContactNumber(const string& contact number){
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
string encyptPin(string pin){
    char key = 'K';
    string encryptedPin = pin;
    for (size_t i =0; i < pin.size(); i++){
        encryptedPin[i] = pin[i] ^ key;
    }
    return encryptedPin;
}

//  FUNCTION TO CHECK IF AN ACCOUNT EXIST BY ACCOUNT NUMNBER
int findAccount(int accountNumber){
    for (int i=0; i<acountCount; i++){
        if (accounts[i].accountNumber == accountNumber){
            return i;
        }
    }
    return -1;
}

void createATMCARDFile(const Account &account){
    string filename = to_string(acount.accountNumber) + "_card.txt";
    ofstream cardFile(filename);

    if (cardFile) {
        cardFile << account.accountNumber << endl;
        cardFile << account.accountName << endl;
        cardFile << account.birthday << endl;
        cardFile << account.contactNumber << endl;
        cardFile << account.pinCode << endl;
        cardFile << account.balance << endl;
        cardFile.close();
        cout << "ATM card (file) created successfully." << end;
    } else {
        cout << "Error creating ATM card (file)." << endl;
    }
}

// FUNCTION TO READ ATM CARD FILE
bool readATMCardFile(int accountNumber, const string &enteredPin, Account &loadedAccount){
    string filename = to_string(accountNumber) + "_card.txt";
    ifstream cardFile(filename);

    if(!cardFile){
        cout << "ATM card not found. PLease insert the correct card." << endl;
        return false;
    }

    cardFile >> loadedAccount.accountNumber;
    cardFile.ignore();
    getline(cardFile, loadedAccount.accountName);
    getline(cardFile, loadedAccount.birthday);
    getline(cardFile, loadedAccount.contactNumber);
    cardFile >> loadedAccount.pinCode;
    cardFile >> loadedAccount.balance;

    if (encryptPin(enteredPin) == loadedAccount.pinCode){
        return true;
    } else {
        cout << "Incorrect PIN." << endl;
        return false;
    }
}

// FUNCTION TO UPDATE THE ATM CARD (FILE)
void updateATMCardFile(const Account &account){
    string filename = to_string(account.accountNumber) + "_card.txt";
    ofstream cardFile(filename);

    if(cardFile){
        cardFile << account.accountNumber << endl;
        cardFile << account.accountName << endl;
        cardFile << account.birthday << endl;
        cardFile << account.contactNumber << endl;
        cardFile << account.pinCode << endl;
        cardFile << account.balance << endl;
        cardFile.close();
        cout << "ATM card (file) updated successfully." << endl;
    } else {
        cout << "Error updating ATM card (file)." << endl;
    }
}

// FUNCTION TO SAVE ALL ACCOUNTS
void saveAccountsToFile(){
    ofstream outFile("accounts_data.txt");

    if(outFile){
        outFile << accountCount << endl;
        for (int i = 0; i <accountCount; i++){
            outFile << accounts[i].accountNumber << endl;
            outFile << accounts[i].accountName << endl;
            outFile << accounts[i].birthday << endl;
            outFile << accounts[i].contactNumber << endl;
            outFile << accounts[i].pinCode << endl;
            outFile << accounts[i].balance << endl;
        }
        outFile.close();
        cout << "Accounts saved successfully." << endl;
    } else {
        cout << "Error saving accounts" << endl;
    }
}

// FUNCTION TO LOAD ACCOUNTS
void loadedAccountsFromFile(){
    ifstream inFile("accounts_data.txt");

    if(inFile){
        inFile >> accountCount;
        inFile.ignore();
        for(int i = 0; i < accountCount; i++){
            inFile >> accounts[i]accountNumber;
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
string getHiddenPin(){
    string pin;
    char ch;

    while(pin.length() < PIN_LENGTH){
        ch = getch();
        if(ch >= '0' && ch <= '9'){
            pin.push_back(ch);
            cout << "*";
        } else if {
            pin.po_back();
            cout << "\b \b";
        }
    }
    cout << endl;
    return pin;
}

// REGISTRATION MODULE - ENROLL NEW ACCOUNTS
void registerAccount(){
    if(accountCount >= MAX_ACCOUNTS){
        cout <<"Cannot create more accounts. Maximum limit reached." << endl;
        return;
    }

    system("cls");
    Account newAccount;
    string accountNumberStr;
    do {
        cout << "Enter Account Number (Exactly 5 digits): " << endl;
        cin >> accountNumberStr;

        if(accountNumberStr.length() != ACCOUNT_NUMBER_LENGTH || !isdigit(accountNumberStr[0])){
            cout << "Error: Account number must be exactly 5 digits. Try again." << endl;
        } else {
            newAccount.accountNumber = stoi(accountNumberStr);
            break;
        } while (true);
    }

    cout << "Enter Account Name: " << endl;
    cin.ignore();
    getline(cin, newAccount.accountName);

    cout << "Enter Birthday(dd/mm/yyyy): " << endl;
    getline(cin, newAccount.birthday);

    string contactNumber;
    do(){
        cout << "Enter Contact Number: " << endl;
        getline(cin, contactNumber);

        if(!isValidContactNumber(contactNumber)){
            cout << "Invalid contact number. Please enter a valid 11-digit number." << endl;
        }
    } while (!isValidContactNumber(contactNumber));

    newAccount.contactNumber = contactNumber;

    while (true){
        cout << "Enter Initial Deposit(minimum " << MIN_DEPOSIT << "): ";
        cin >> newAccount.balance;

        if(newAccount.balance < MIN_DEPOSIT){
            cout << "Initial deposit at least " << MIN_DEPOSIT << ". Try again." << endl;
        } else {
            break;
        }
    } system("pause");

    string pin;
    cout << "Set a 6-digit PIN: ";
    pin = getHiddenPin();
    newAccount.pinCode = encryptPin(pin);

    accountNumberStr[accountCount++] = newAccount;
    createATMCARDFile(newAccount);
    system("pause");
    cout << "Account registered seccessfully!" << endl;
}

// FUNCTION TO DELETE ACCOUNT
void deleteAccount(){
    int accountNumber;
    cout << "Enter the account number to delete: ";
    cin >> accountNumber;

    int accountIndex = findAccount(accountNumber);
    if(accountIndex == -1){
        cout << "Account not found." << endl;
        return;
    }

    for(int i = accountIndex; i < accountCount -1; i++){
        accounts[i] = accounts[i + 1];
    }

    accountCount--;
    cout << "Account deleted successfully." << endl;

    string filename = to_string(accountNumber) + "_card.txt";
    if(remove(filename.c_str()) != 0){
        cout << "Error deleting ATM card file." << endl;
    } else {
        cout << "ATM card file deleted successfully." << endl;
    }

    saveAccountsToFile();
}

// CHANGE PIN
void changePin(int accountIndex){
    string newPin;
    system("cls");
    cout << "Enter new 6-digit PIN: ";
    newPin = getHiddenPin();

    accounts(accountIndex.pinCode = encryptPin(newPin));
    updateATMCardFile(accounts[accountIndex]);
    system("pause");
    cout << "PIN changed successfully!" << endl; 
    system("pause");
}

// BALANCE INQUIRY
void balanceInquiry(int accountIndex){
    system("cls");
    cout << "Account Number: " << accounts[accountIndex].accountNumber << endl;
    cout << "Account Name: " << accounts[accountIndex].accountName << endl;
    cout << "Account Balance" << accounts[accountIndex].balance << endl;
    system("pause");
}

// WITHDRAW MONEY
void withdrawMoney(int accountIndex){
    int amount;
    system("cls");
    cout << "Enter amount to withdraw: ";
    cin >> amount;
    system("pause");
    if (amount > accounts[accountIndex].balance){
        cout << "Insuffiicient funds." << endl;
    } else {
        accounts[accountIndex].balance -= amount;
        updateATMCardFile(accounts[accountIndex]);
        cout << "Withdraw successful. New balance: " << accounts[accountIndex].balance << endl;
    }
}

// DEPOSIT MONEY
void depositMoney(int accountIndex){
    int amount;
    system("cls");
    cout << "Enter amount to deposit: ";
    cin >> amount;
    system("pause");
    accounts[accountIndex].balance += amount;
    updateATMCardFile(accounts[accountIndex]);
    cout << "Deposit successful. New balance: " << accounts[accountIndex].balance << endl;
    system("pause");
}

// FUNCTION FOR FUND TRANSFER
void fundTransfer(int accountIndex){
    int targetAccountNumber, transferAmount;

    cout << "Enter account number for transfer: ";
    cin >> targetAccountNumber;

    int targetIndex = findAccount(targetAccountNumber);
    if(targetIndex == -1){
        cout << "Target account not found!" << endl;
        return;
    } 

    cout << "Enter amount to transfer: ";
    cin >> transferAmount;

    if(transferAmount > accounts[accountIndex].balance){
        cout << "Insufficient funds in your account." << endl;
        return;
    }

    accounts[accountIndex].balance -= transferAmount;
    accounts[accountIndex].balance += transferAmount;

    updateATMCardFile(accounts[accountIndex]);
    updateATMCardFile(accounts[targetIndex]);

    cout << "Transfer successful! New Balance: " << accounts[accountIndex].balance << endl;
    cout << "Target account new balance: " << accounts[targetIndex].balance << endl;
} 

// FUNCTION TO SIMULATE ATM CARD INSERTION
void verifyCard(int accountNumber, string enteredPin){
    Account loadedAccount;
    if(readATMCardFile(accountNumber, enteredPin, loadedAccount)){
        int accountIndex = findAccount(accountNumber);
        if(accountIndex != -1){
            accounts[accountIndex] = loadedAccount;
            return accountIndex;
        }
    }
    return -1;
}

// MAIN MENU FOR ATM
void atmMenu(){
    int accountNumber;
    string pin;
    system("cls");
    cout << "Welcome to the ATM." << endl;
    cout << "Please insert your card (Enter account number): ";
    cin >> accountNumber;

    cout << "Enter your PIN: ";
    pin = getHiddenPin();

    system("pause");
    int accountIndex = verifyCard(accountNumber, pin);
    if (accountIndex == -1) return;

    int choice;
    do{
        system("pause");
        system("cls");
        cout << endl << "ATM MENU" << endl;
        cout << "1. Balance Inquiry" << endl;
        cout << "2. Withdraw Money" << endl;
        cout << "3. Deposit Money" << endl;
        cout << "4. Change PIN" << endl;
        cout << "5. Fund Trasfer" << endl;
        cout << "6. Exit" << endl;
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice){
            case 1: 
                balanceInquiry(accountIndex);
                break;
            case 2: 
                withdrawMoney(accountIndex);
                break;
            case 3:
                depositMoney(accountIndex);
                break;
            case 4:
                changePin(accountIndex);
                break;
            case 5:
                fundTransfer(accountIndex);
                break;
            case 6:
                cout << "Thank you for using th ATM." << endl;
                break;
            default:
                cout << "Invalid choice." << endl;
        }
        system("pause");
    }  while (choice != 6);
}

// MAIN FUNCTION
int main(){
    loadedAccountsFromFile();

    int option;
    do{
        system("cls");
        cout << endl << "ATM SIMULATION" << endl;
        cout << "1. Register Account" << endl;
        cout << "2. Access ATM" << endl;
        cout << "3. Delete Acount" << endl;
        cout << "4. Exit" << endl;
        cout << "Enter choice: ";
        cin >> option;

        switch (option){
            case 1:
                registerAccount();
                break;
            case 2: 
                atmMenu();
                break;
            case 3: 
                deleteAccount();
                break;
            case 4:
                saveAccountsToFile();
                cout << "Exiting. . . " << endl;
                break;
            default:
                cout << "Invalid choice. Try again." << endl;
        }
        system("pause");
    } while (option != 4);

    return 0;
}
