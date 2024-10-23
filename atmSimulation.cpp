#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <cstdlib>
#include <random>
#include <conio.h>
#include <windows.h>
#include <dbt.h>
#include <chrono>
#include <thread>
#include <set>
#include <ctime>
#include <cctype>
#include <regex>
#include <limits>

using namespace std;

enum MenuOptions { REGISTER = 0, ACCESS_ATM, DELETE_ACCOUNT, EXIT };
bool usbDeviceDetected = false;
string usbDriveLetter;

const int MAX_ACCOUNTS = 100; // MAXIMUM NUMBER OF ACCOUNTS
const int MIN_DEPOSIT = 5000; // MINIMUM DEPOSITS
const int PIN_LENGTH = 6;
const int ACCOUNT_ID_LENGTH= 5; // DEFINE THE EXPECTED LENGTH FOR THE ACCOUNT NUMBER
const int CONTACT_NUMBER_LENGTH = 11; // MAX LENGTH OF CONTACT NUMBER
const int DAY_LENGTH = 2;  // LENGTH FOR DAY INPUT
const int MONTH_LENGTH = 2; // LENGTH FOR MONTH INPUT
const int YEAR_LENGTH = 4;  // LENGTH FOR YEAR INPUT
const string GREEN = "\033[32m";  // GREEN COLOR
const string RESET = "\033[0m";   // RESET COLOR
set<int> usedAccountIDs;
void atmMenu(int accountIndex);

void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

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

// FUNCTION TO DISPLAY A CENTERED MESSAGE
void displayCenteredMessage(const string& message, int width = 50) {
    int pad = (width - message.size()) / 2;
    for (int i = 0; i < pad; i++) cout << " ";
    cout << message << endl;
}

// FUNCTION TO DISPLAY A SPLASH SCREEN
void showSplashScreen() {
    system("cls");
    displayCenteredMessage("==========================================");
    displayCenteredMessage("           WELCOME TO POWER BANK          ");
    displayCenteredMessage("==========================================");
    displayCenteredMessage("         Insert USB       ");
}

// FUNCTION TO DISPLAY A PROGRESS BAR
void showProgressBar() {
    const int barWidth = 40;
    const int totalSteps = 15;
    const int consoleWidth = 50;

    system("cls");

    displayCenteredMessage("==========================================");
    displayCenteredMessage("            POWER BANK ATM LOADING        ");
    displayCenteredMessage("==========================================");
    displayCenteredMessage("Loading . . . ");

    cout << endl;

    int pad = (consoleWidth - barWidth - 2) / 2;

    cout << string(pad, ' ') << "[";

    for (int i = 0; i <= totalSteps; ++i) {

        int pos = (i * barWidth) / totalSteps;
        cout.flush();
        cout << "\r" << string(pad, ' ') << "[";

        cout << GREEN;
        for (int j = 0; j < barWidth; ++j) {
            if (j < pos) cout << "=";
            else cout << " ";
        }
        cout << RESET;

        cout << "]";
        this_thread::sleep_for(chrono::milliseconds(50));
    }

    cout << endl << endl;
    displayCenteredMessage("Loading complete!");
}

// FUNCTION TO GET THE DRIVE LETTER
string getUsbDriveLetter() {
    DWORD drives = GetLogicalDrives();
    char driveLetter = 'A';
    while (drives) {
        if (drives & 1) {
            string drive = string(1, driveLetter) + ":\\";
            UINT driveType = GetDriveTypeA(drive.c_str());
            if (driveType == DRIVE_REMOVABLE) {
                return drive;
            }
        }
        drives >>= 1;
        driveLetter++;
    }
    return "";
}

// FUNCTION TO HANDLE USB DEVICE CHANGES
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_DEVICECHANGE) {
        PDEV_BROADCAST_HDR pHdr = (PDEV_BROADCAST_HDR)lParam;
        if (wParam == DBT_DEVICEARRIVAL && pHdr->dbch_devicetype == DBT_DEVTYP_VOLUME) {
            usbDriveLetter = getUsbDriveLetter();
            if (!usbDriveLetter.empty()) {
                usbDeviceDetected = true;
            }
        }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// FUNCTION TO WAIT FOR USB INSERTION
bool waitForUsbInsert() {

    WNDCLASSA windowClass = { 0 };
    windowClass.lpfnWndProc = WindowProc;
    windowClass.lpszClassName = "UsbDetectionClass";

    RegisterClassA(&windowClass);

    HWND hwnd = CreateWindowA(windowClass.lpszClassName, "USB Detection", 0, 0, 0, 0, 0, nullptr, nullptr, nullptr, nullptr);

    if (!hwnd) {
        cerr << "Failed to create window for USB detection" << endl;
        return false;
    }

    DEV_BROADCAST_DEVICEINTERFACE dbdi = { 0 };
    dbdi.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
    dbdi.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    HDEVNOTIFY hDeviceNotify = RegisterDeviceNotification(hwnd, &dbdi, DEVICE_NOTIFY_WINDOW_HANDLE);

    if (!hDeviceNotify) {
        cerr << "Failed to register for device notifications" << endl;
        return false;
    }

    showSplashScreen();

    MSG msg = { 0 };

    while (!usbDeviceDetected) {
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        this_thread::sleep_for(chrono::milliseconds(100));
    }

    DestroyWindow(hwnd);
    return usbDeviceDetected;
}

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

bool isDigitsOnly(const string& str) {
    return regex_match(str, regex("\\d+"));
}

bool isValidDay(int day) {
    return day >= 1 && day <= 31;
}

bool isValidMonth(int month) {
    return month >= 1 && month <= 12;
}

bool isValidYear(int year) {
    return year >= 1900 && year <= 2023;
}

int getValidNumericInput(int minValue, int maxValue) {
    int input;
    while (true) {
        cin >> input;

        if (cin.fail() || input < minValue || input > maxValue) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a number between " << minValue << " and " << maxValue << ": ";
        } else {
            break;
        }
    }
    return input;
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

// FUNCTION TO CREAT A ATM CARD FILE
bool createATMCardFile(const Account &account) {
    string usbPath = "D:/";
    string filename = to_string(account.accountNumber) + "_card.txt";
    ofstream cardFile(usbPath + filename);

    system("cls");
    displayCenteredMessage("===========================================");
    displayCenteredMessage("CREATING ATM CARD FILE");
    displayCenteredMessage("===========================================");

    if (cardFile) {
        // Write account details to the file
        cardFile << account.accountNumber << endl;
        cardFile << account.accountName << endl;
        cardFile << account.birthday << endl;
        cardFile << account.contactNumber << endl;
        cardFile << account.pinCode << endl;
        cardFile << account.balance << endl;
        cardFile.close();

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);
        displayCenteredMessage("ATM card created successfully");
        return true;
    } else {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
        displayCenteredMessage("Error: Unable to create ATM card file.");
        displayCenteredMessage("There is no card detected.");
        return false;
    }

    displayCenteredMessage("===========================================");
    system("pause");
}

// FUNCTION TO READ ATM CARD FILE
bool readATMCardFile(int accountNumber, const string &enteredPin, Account &loadedAccount) {
    string usbPath = "D:/";
    string filename = to_string(accountNumber) + "_card.txt";
    ifstream cardFile(usbPath + filename);

    if (!cardFile) {
        system("cls");
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
        displayCenteredMessage("======================================================");
        displayCenteredMessage("ATM card not found. Please insert the correct card.");
        displayCenteredMessage("======================================================");
        system("pause");
        return false;
    }

    cardFile >> loadedAccount.accountNumber;
    cardFile.ignore();
    getline(cardFile, loadedAccount.accountName);
    getline(cardFile, loadedAccount.birthday);
    getline(cardFile, loadedAccount.contactNumber);
    cardFile >> loadedAccount.pinCode;
    cardFile >> loadedAccount.balance;

    cardFile.close();

    string encryptedPin = encryptPin(enteredPin);
    if (encryptedPin == loadedAccount.pinCode) {
        return true;  
    } else {
        displayCenteredMessage("Incorrect PIN.");
        return false;  
    }
}

// FUNCTION TO UPDATE THE ATM CARD FILE
void updateATMCardFile(const Account &account) {
    string usbPath = "D:/";
    string filename = to_string(account.accountNumber) + "_card.txt";
    ofstream cardFile(usbPath + filename);

    system("cls");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
    displayCenteredMessage("===========================================");
    displayCenteredMessage("UPDATING ATM CARD");
    displayCenteredMessage("===========================================");

    if (cardFile) {
        cardFile << account.accountNumber << endl;
        cardFile << account.accountName << endl;
        cardFile << account.birthday << endl;
        cardFile << account.contactNumber << endl;
        cardFile << account.pinCode << endl;
        cardFile << account.balance << endl;
        cardFile.close();

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);
        displayCenteredMessage("ATM card updated successfully");
    } else {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
        displayCenteredMessage("Error: Unable to update ATM card file.");
    }

    displayCenteredMessage("===========================================");
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
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);
        displayCenteredMessage("===========================================");
        displayCenteredMessage("ACCOUNTS SAVED SUCCESSFULLY");
        displayCenteredMessage("===========================================");
    } else {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
        displayCenteredMessage("===========================================");
        displayCenteredMessage("ERROR: FAILED TO SAVE DATA");
        displayCenteredMessage("===========================================");
        displayCenteredMessage("There was an issue saving the account.");
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

// UNIQUE ACCOUNT NUMBER GENERATOR
int generateUniqueAccountNumber() {
    srand(time(0));  // Seed the random number generator

    int accountNumber;
    bool isUnique;

    do {
        accountNumber = rand() % 90000 + 10000;  // Generate a 5-digit number (10000-99999)
        isUnique = true;  // Assume the number is unique initially

        // Check if the account number already exists
        for (int i = 0; i < accountCount; i++) {
            if (accounts[i].accountNumber == accountNumber) {
                isUnique = false;  // If a match is found, mark as not unique
                break;
            }
        }

    } while (!isUnique);  // Repeat if the number is not unique

    return accountNumber;
}

// REGISTRATION MODULE - ENROLL NEW ACCOUNTS
void registerAccount() {
    if (accountCount >= MAX_ACCOUNTS) {
        system("cls");
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
        displayCenteredMessage("==========================================", 50);
        displayCenteredMessage("Cannot create more accounts.", 50);
        displayCenteredMessage("Maximum limit reached.", 50);
        displayCenteredMessage("==========================================\n", 50);
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
        system("pause");
        return;
    }

    system("cls");
    Account newAccount;
    char confirm;

    // Change text color to Yellow for registration welcome message
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
    displayCenteredMessage("==========================================", 50);
    displayCenteredMessage("WELCOME TO POWER BANK", 50);
    displayCenteredMessage("==========================================\n", 50);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);

    displayCenteredMessage("ATM Account Registration", 50);
    displayCenteredMessage("Do you want to continue with registration?", 50);
    cout << "\t(1 to proceed / 0 to cancel): ";
    cin >> confirm;

    if (tolower(confirm) == '0') {
        system("cls");
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
        displayCenteredMessage("==========================================", 50);
        displayCenteredMessage("Registration canceled. Returning", 50);
        displayCenteredMessage("to main menu.", 50);
        displayCenteredMessage("==========================================\n", 50);
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
        system("pause");
        return;
    } else if (tolower(confirm) == '1') {

        newAccount.accountNumber = generateUniqueAccountNumber();
        cout << "\n\tGenerated Account Number: " << newAccount.accountNumber << endl;

        // Centered input prompts for account registration
        cout << "\tEnter Account Name: ";
        cin.ignore();
        getline(cin, newAccount.accountName);

        // BIRTHDAY INPUT IN SEPARATE PARTS
        int day, month, year;
        cout << "\tEnter Birthday\n";

        // INPUT DAY
        cout << "\tDay (1-31): ";
        day = getValidNumericInput(1, 31);

        // INPUT MONTH
        cout << "\tMonth (1-12): ";
        month = getValidNumericInput(1, 12);

        // INPUT YEAR
        cout << "\tYear (1900-2023): ";
        year = getValidNumericInput(1900, 2023);

        // FORMAT THE BIRTHDAY TO dd/mm/yyyy
        newAccount.birthday = (day < 10 ? "0" : "") + to_string(day) + "/" +
                              (month < 10 ? "0" : "") + to_string(month) + "/" + to_string(year);
        cout << "\tBirthday: " << newAccount.birthday << endl;

        // CONTACT NUMBER INPUT
        string contactNumber;
        cin.ignore();
        do {
            cout << "\tEnter Contact Number (11 digits): ";
            getline(cin, contactNumber);

            if (!isValidContactNumber(contactNumber)) {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
                displayCenteredMessage("Invalid contact number.", 50);
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
            }
        } while (!isValidContactNumber(contactNumber));

        newAccount.contactNumber = contactNumber;

        while (true) {
            cout << "\tEnter Initial Deposit (minimum " << MIN_DEPOSIT << "): ";
            cin >> newAccount.balance;

            if (newAccount.balance < MIN_DEPOSIT) {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
                displayCenteredMessage("Initial deposit must be at least " + to_string(MIN_DEPOSIT) + ". Try again.", 50);
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
            } else {
                break;
            }
        }

        string pin;
        cout << "\tSet a 6-digit PIN: ";
        pin = getHiddenPin();
        newAccount.pinCode = encryptPin(pin);

        accounts[accountCount++] = newAccount;

        if (createATMCardFile(newAccount)) {
            system("cls");
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);
            displayCenteredMessage("==========================================", 50);
            displayCenteredMessage("Account registered successfully!", 50);
            displayCenteredMessage("Enjoy powerful transactions with POWER BANK!", 50);
            displayCenteredMessage("==========================================\n", 50);

            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
        }

    } else {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
        displayCenteredMessage("==========================================", 50);
        displayCenteredMessage("Error. Returning to main menu.", 50);
        displayCenteredMessage("==========================================\n", 50);

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
        system("pause");
        return;
    }

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);

    displayCenteredMessage("IMPORTANT: Ensure you exit the program properly to save your account data.", 50);
    displayCenteredMessage("Failure to exit properly may result in data loss.", 50);
    system("pause");
}

// FUNCTION TO DELETE ACCOUNT
void deleteAccount() {
    int accountNumber;
    system("cls");

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
    displayCenteredMessage("==========================================", 50);
    displayCenteredMessage("DELETE ATM ACCOUNT", 50);
    displayCenteredMessage("==========================================\n", 50);

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);

    cout << "\tEnter the account number to delete: ";
    cin >> accountNumber;

    int accountIndex = findAccount(accountNumber);

    if (accountIndex == -1) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
        displayCenteredMessage("==========================================", 50);
        displayCenteredMessage("ACCOUNT NOT FOUND", 50);
        displayCenteredMessage("==========================================\n", 50);

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
        system("pause");
        return;
    }

    cout << "\t\nAccount Found:\n";
    cout << "\tAccount Number : " << accounts[accountIndex].accountNumber << endl;
    cout << "\tAccount Name   : " << accounts[accountIndex].accountName << endl;
    cout << "\tBalance        : PHP " << accounts[accountIndex].balance << "\n" << endl;


    char confirm;
    displayCenteredMessage("Are you sure you want to delete this account? (1 to confirm / 0 to cancel): ", 50);
    cin >> confirm;

    if (tolower(confirm) == '0') {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
        displayCenteredMessage("\nDeletion canceled. Returning to main menu.\n", 50);

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
        system("pause");
        return;
    } else if (tolower(confirm) == '1') {

        for (int i = accountIndex; i < accountCount - 1; i++) {
            accounts[i] = accounts[i + 1];
        }
        accountCount--;
        system("cls");
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);
        displayCenteredMessage("==========================================", 50);
        displayCenteredMessage("ACCOUNT DELETED SUCCESSFULLY", 50);
        displayCenteredMessage("==========================================\n", 50);
        system("cls");
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);

        string filename = "D:/" + to_string(accountNumber) + "_card.txt";
        if (remove(filename.c_str()) != 0) {

            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
            displayCenteredMessage("Error deleting ATM card account.", 50);

            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
        } else {
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);
            displayCenteredMessage("ATM card file deleted successfully.", 50);

            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
        }

        saveAccountsToFile();
    } else {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
        displayCenteredMessage("\nInvalid input. Returning to main menu.\n", 50);

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
    }

    system("pause");
}

// CHANGE PIN
void changePin(int accountIndex) {
    string newPin, confirmPin;
    system("cls");

    displayCenteredMessage("==========================================", 50);
    displayCenteredMessage("POWER BANK - CHANGE PIN", 50);
    displayCenteredMessage(" ==========================================\n", 50);

    do {
        cout << "\tEnter new 6-digit PIN: ";
        newPin = getHiddenPin();

        if (newPin.length() != 6 || !isdigit(newPin[0])) {
            displayCenteredMessage("==========================================", 50);
            displayCenteredMessage("ERROR: INVALID PIN LENGTH", 50);
            displayCenteredMessage("==========================================\n", 50);
            displayCenteredMessage("PIN must be exactly 6 digits. Please try again.", 50);
        }
    } while (newPin.length() != 6 || !isdigit(newPin[0]));

    cout << "\tConfirm new 6-digit PIN: ";
    confirmPin = getHiddenPin();

    if (newPin != confirmPin) {
        displayCenteredMessage("==========================================", 50);
        displayCenteredMessage("ERROR: PINS DO NOT MATCH", 50);
        displayCenteredMessage("==========================================\n", 50);
        displayCenteredMessage("PIN change failed. Please try again.", 50);
    } else {
        accounts[accountIndex].pinCode = encryptPin(newPin);
        updateATMCardFile(accounts[accountIndex]);

        displayCenteredMessage("==========================================", 50);
        displayCenteredMessage("PIN CHANGED SUCCESSFULLY", 50);
        displayCenteredMessage("==========================================\n", 50);
    }

    system("pause");
}

// BALANCE INQUIRY
void balanceInquiry(int accountIndex) {
    system("cls");

    displayCenteredMessage("==========================================", 50);
    displayCenteredMessage("POWER BANK SAVINGS INQUIRY", 50);
    displayCenteredMessage("==========================================", 50);

    displayCenteredMessage("\tAccount Details", 50);
    displayCenteredMessage("------------------------------------------\n");
    cout << "\tAccount Number : " << accounts[accountIndex].accountNumber << endl;
    cout << "\tAccount Name   : " << accounts[accountIndex].accountName << endl;
    cout << "\tCurrent Balance: PHP " << accounts[accountIndex].balance << endl;
    displayCenteredMessage("------------------------------------------\n");

    displayCenteredMessage("------------------------------------------\n");
    system("pause");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 6);
    displayCenteredMessage("Note: Please make sure to properly exit the system to ensure data is saved.", 50);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
    system("cls");
    displayCenteredMessage("==========================================", 50);
    displayCenteredMessage("Thank you for using", 50);
    displayCenteredMessage("POWER BANK ATM SERVICE", 50);
    displayCenteredMessage("==========================================\n", 50);

    system("pause");
}

// WITHDRAW MONEY
void withdrawMoney(int accountIndex) {
    int amount;
    system("cls");

    displayCenteredMessage("==========================================");
    displayCenteredMessage("POWER BANK - WITHDRAWAL");
    displayCenteredMessage("==========================================");

    cout << "\tAccount Number : " << accounts[accountIndex].accountNumber << endl;
    cout << "\tAccount Name   : " << accounts[accountIndex].accountName << endl;
    displayCenteredMessage("------------------------------------------");
    cout << "\tCurrent Balance: PHP " << accounts[accountIndex].balance << endl;
    displayCenteredMessage("------------------------------------------");

    cout << "\n\tEnter amount to withdraw: PHP ";
    cin >> amount;

    system("pause");

    if (amount > 0 && amount <= accounts[accountIndex].balance) {
        accounts[accountIndex].balance -= amount;
        updateATMCardFile(accounts[accountIndex]);

        system("cls");
        displayCenteredMessage("==========================================", 50);
        displayCenteredMessage("WITHDRAWAL SUCCESSFUL", 50);
        displayCenteredMessage("==========================================", 50);
        cout << "\tAmount Withdrawn: PHP " << amount << endl;
        cout << "\tNew Balance     : PHP " << accounts[accountIndex].balance << endl;
        displayCenteredMessage("==========================================", 50);
    } else {
        system("cls");
        displayCenteredMessage("==========================================", 50);
        displayCenteredMessage("ERROR: INSUFFICIENT FUNDS", 50);
        displayCenteredMessage("==========================================", 50);
        displayCenteredMessage("You do not have enough balance to withdraw that amount.", 50);
    }

    displayCenteredMessage("Please take your cash. Thank you for using POWER BANK.");

    system("pause");
}

// DEPOSIT MONEY
void depositMoney(int accountIndex) {
    int amount;
    system("cls");

    displayCenteredMessage("==========================================");
    displayCenteredMessage("POWER BANK - DEPOSIT");
    displayCenteredMessage("==========================================");

    cout << "\tAccount Number : " << accounts[accountIndex].accountNumber << endl;
    cout << "\tAccount Name   : " << accounts[accountIndex].accountName << endl;
    displayCenteredMessage("------------------------------------------");
    cout << "\tCurrent Balance: PHP " << accounts[accountIndex].balance << endl;
    displayCenteredMessage("------------------------------------------");

    cout << "\n\tEnter amount to deposit: PHP ";
    cin >> amount;

    if (amount <= 0) {
        system("cls");
        displayCenteredMessage("==========================================");
        displayCenteredMessage("ERROR: INVALID AMOUNT");
        displayCenteredMessage("==========================================");
        displayCenteredMessage("Deposit amount must be greater than 0.");
        system("pause");
    } else {
        accounts[accountIndex].balance += amount;
        updateATMCardFile(accounts[accountIndex]);
        system("cls");
        displayCenteredMessage("==========================================");
        displayCenteredMessage("DEPOSIT SUCCESSFUL");
        displayCenteredMessage("==========================================");
        cout << "\tAmount Deposited: PHP " << amount << endl;
        cout << "\tNew Balance     : PHP " << accounts[accountIndex].balance << endl;
        displayCenteredMessage("==========================================");
        system("pause");
    }

    system("pause");
}

// FUNCTION FOR FUND TRANSFER
void fundTransfer(int accountIndex) {
    int targetAccountNumber, transferAmount;

    system("cls");
    displayCenteredMessage("===========================================");
    displayCenteredMessage("POWER BANK - FUND TRANSFER");
    displayCenteredMessage("===========================================");

    cout << "\tEnter target account number for transfer: ";
    cin >> targetAccountNumber;

    int targetIndex = findAccount(targetAccountNumber);
    if (targetIndex == -1) {
        system("cls");
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
        displayCenteredMessage("===========================================");
        displayCenteredMessage("ERROR: ACCOUNT NOT FOUND");
        displayCenteredMessage("===========================================");
        displayCenteredMessage("The target account number does not exist. Please try again.");
        system("pause");
        return;
    }

    cout << "\tEnter amount to transfer: ";
    cin >> transferAmount;

    displayCenteredMessage("-------------------------------------------");

    if (transferAmount > accounts[accountIndex].balance) {
        system("cls");
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
        displayCenteredMessage("===========================================");
        displayCenteredMessage("ERROR: INSUFFICIENT FUNDS");
        displayCenteredMessage("===========================================");
        displayCenteredMessage("You do not have enough balance for this transaction.");
        system("pause");
        return;
    }
    system("cls");

    accounts[accountIndex].balance -= transferAmount;
    accounts[targetIndex].balance += transferAmount;

    updateATMCardFile(accounts[accountIndex]);
    updateATMCardFile(accounts[targetIndex]);
    system("cls");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 2);
    displayCenteredMessage("===========================================");
    displayCenteredMessage("TRANSFER SUCCESSFUL!");
    displayCenteredMessage("===========================================");

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
    cout << "\tAmount Transferred: " << transferAmount << endl;
    cout << "\tYour new balance: " << accounts[accountIndex].balance << endl;
    cout << "\tTarget account new balance: " << accounts[targetIndex].balance << "\n" << endl;

    system("pause");
}


// FUNCTION TO SIMULATE ATM CARD INSERTION
int verifyCard(int accountNumber, string enteredPin) {
    Account loadedAccount;
    if (readATMCardFile(accountNumber, enteredPin, loadedAccount)) {
        int accountIndex = findAccount(accountNumber);
        if (accountIndex != -1) {
            accounts[accountIndex] = loadedAccount;
            return accountIndex;
        }else {
            accounts[accountCount] = loadedAccount;
            accountIndex = accountCount;
            accountCount++;
        }
        return accountIndex;
    }
    return -1;
}

// LOGIN FOR ATM
void atmLogin() {
    int accountNumber;
    string pin;
    bool loginSuccess = false;
    int retryCount = 0;
    const int maxRetries = 3;

    while (!loginSuccess && retryCount < maxRetries) {
        system("cls");
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
        displayCenteredMessage("==========================================");
        displayCenteredMessage("Welcome to POWER BANK ATM MACHINE  ");
        displayCenteredMessage("==========================================");

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
        cout << "\t\nPlease insert your card (enter account number): ";
        cin >> accountNumber;

        if (accountNumber == 0) {
            displayCenteredMessage("Exiting ATM login process...");
            system("pause");
            return;
        }

        cout << "\tEnter your PIN: ";
        pin = getHiddenPin();

        int accountIndex = verifyCard(accountNumber, pin);

        if (accountIndex == -1) {
            system("cls");
            retryCount++;
            displayCenteredMessage("Invalid card or PIN. Please try again.");
            displayCenteredMessage("Attempt " + to_string(retryCount) + " of " + to_string(maxRetries));
            system("pause");
        } else {
            loginSuccess = true;
            atmMenu(accountIndex);
        }
    }

    if (!loginSuccess) {
        displayCenteredMessage("Maximum login attempts exceeded. Returning to the main menu.");
        system("pause");
    }
    system("pause");
}

// MAIN MENU FOR ATM
void atmMenu(int accountIndex) {
    int choice = 1;
    bool enterPressed = false;

    while (!enterPressed) {
        system("cls");

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
        displayCenteredMessage("==========================================");
        displayCenteredMessage("     POWER BANK ATM MACHINE MENU     ");
        displayCenteredMessage("==========================================");


        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
        displayCenteredMessage("IMPORTANT: Ensure you exit properly to save data.");
        displayCenteredMessage("Failure to exit properly may result in data loss.\n");

        displayCenteredMessage(choice == 1 ? " > Balance Inquiry" : "   Balance Inquiry");
        displayCenteredMessage(choice == 2 ? " > Withdraw Money" : "   Withdraw Money");
        displayCenteredMessage(choice == 3 ? " > Deposit Money" : "   Deposit Money");
        displayCenteredMessage(choice == 4 ? " > Change PIN" : "   Change PIN");
        displayCenteredMessage(choice == 5 ? " > Fund Transfer" : "   Fund Transfer");
        displayCenteredMessage(choice == 6 ? " > Exit" : "   Exit");

        int key = _getch();

        if (key == 224) {
            switch (_getch()) {
                case 72:
                    if (choice > 1) choice--;
                    break;
                case 80:
                    if (choice < 6) choice++;
                    break;
            }
        } else if (key == 13) {
            enterPressed = true;
        }
    }

    switch (choice) {
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
            system("cls");
            displayCenteredMessage("============================================");
            displayCenteredMessage("Thank you for using POWER BANK ATM MACHINE");
            displayCenteredMessage("============================================");
            return;
    }

    system("pause");

    atmMenu(accountIndex);
}

// MAIN MENU FOR ATM
void showMainMenu() {
    while (true) {
        system("cls");

        int choice = REGISTER;
        bool enterPressed = false;

        while (!enterPressed) {
            system("cls");
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
            displayCenteredMessage("==========================================");
            displayCenteredMessage("            POWER BANK MAIN MENU          ");
            displayCenteredMessage("==========================================");

            if (choice == REGISTER) {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
                displayCenteredMessage("  > Register Account");
            } else {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
                displayCenteredMessage("    Register Account");
            }

            if (choice == ACCESS_ATM) {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
                displayCenteredMessage("  > Access ATM");
            } else {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
                displayCenteredMessage("    Access ATM");
            }

            if (choice == DELETE_ACCOUNT) {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
                displayCenteredMessage("  > Delete Account");
            } else {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
                displayCenteredMessage("    Delete Account");
            }

            if (choice == EXIT) {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
                displayCenteredMessage("  > Exit");
            } else {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
                displayCenteredMessage("    Exit");
            }

            int key = _getch();

            if (key == 224) {
                switch (_getch()) {
                    case 72:
                        if (choice > REGISTER) choice--;
                        break;
                    case 80:
                        if (choice < EXIT) choice++;
                        break;
                }
            } else if (key == 13) {
                enterPressed = true;
            }
        }

        switch (choice) {
            case REGISTER:
                system("cls");
                displayCenteredMessage("You selected Register Account.");
                registerAccount();
                break;
            case ACCESS_ATM:
                system("cls");
                displayCenteredMessage("You selected Access ATM.");
                atmLogin();
                break;
            case DELETE_ACCOUNT:
                system("cls");
                displayCenteredMessage("You selected Delete Account.");
                deleteAccount();
                break;
            case EXIT:
                system("cls");
                displayCenteredMessage("Thank you for using POWER BANK!");
                displayCenteredMessage("Exiting...");
                saveAccountsToFile();
                exit(0);
        }
    }
}

// MAIN FUNCTION
int main() {
    loadAccountsFromFile();

    if (waitForUsbInsert()) {
        showProgressBar();
        showMainMenu();
    }

    return 0;
}
