#include "logindialog.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QDir>
#include <QDebug>

// Constructor for the LoginDialog class
LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Login"); // Set the title of the login dialog

    // Create input fields for username and password
    usernameEdit = new QLineEdit(this); // Username input field
    passwordEdit = new QLineEdit(this); // Password input field
    passwordEdit->setEchoMode(QLineEdit::Password); // Set password field to hide input

    // Create buttons for login and account creation
    loginButton = new QPushButton("Login", this);
    createAccountButton = new QPushButton("Create Account", this);

    // Create a vertical layout and add widgets
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("Username:", this)); // Label for username
    layout->addWidget(usernameEdit); // Add username input field
    layout->addWidget(new QLabel("Password:", this)); // Label for password
    layout->addWidget(passwordEdit); // Add password input field
    layout->addWidget(loginButton); // Add login button
    layout->addWidget(createAccountButton); // Add create account button

    // Connect button signals to their respective slot functions
    connect(loginButton, &QPushButton::clicked, this, &LoginDialog::onLoginClicked);
    connect(createAccountButton, &QPushButton::clicked, this, &LoginDialog::onCreateAccountClicked);
}

// Slot function triggered when the Login button is clicked
void LoginDialog::onLoginClicked()
{
    QString username = usernameEdit->text(); // Get username input
    QString password = passwordEdit->text(); // Get password input

    // Check if the provided credentials are valid
    if (checkCredentials(username, password)) {
        m_username = username; // Store the username if login is successful
        accept(); // Close the dialog with an accepted result
    } else {
        // Show warning message if login fails
        QMessageBox::warning(this, "Login Failed", "Incorrect username or password.");
    }
}

// Slot function triggered when the Create Account button is clicked
void LoginDialog::onCreateAccountClicked()
{
    QString username = usernameEdit->text(); // Get username input
    QString password = passwordEdit->text(); // Get password input

    // Check for empty username or password fields
    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Invalid Input", "Please enter both username and password.");
        return; // Exit the function if input is invalid
    }

    createAccount(username, password); // Attempt to create a new account
    // Clear input fields after account creation for user to log in manually
    usernameEdit->clear();
    passwordEdit->clear();
}

// Function to check if the provided credentials are valid
bool LoginDialog::checkCredentials(const QString &username, const QString &password)
{
    QString filePath = QDir::currentPath() + "/LOGINS.txt"; // Path to the login credentials file
    QFile file(filePath); // Create a QFile object for the login file
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) { // Attempt to open the file for reading
        qDebug() << "Failed to open LOGINS.txt for reading:" << file.errorString(); // Log error
        return false; // Return false if file cannot be opened
    }

    QTextStream in(&file); // Create a text stream for reading the file
    while (!in.atEnd()) { // Loop through each line in the file
        QString line = in.readLine(); // Read a line from the file
        QStringList parts = line.split(':'); // Split line into parts using ':' as a delimiter
        // Check if the username and password match
        if (parts.size() == 2 && parts[0] == username && parts[1] == password) {
            file.close(); // Close the file if credentials are correct
            return true; // Return true if login is successful
        }
    }

    file.close(); // Close the file after checking all credentials
    return false; // Return false if no matching credentials were found
}

// Function to create a new account
void LoginDialog::createAccount(const QString &username, const QString &password)
{
    QString logMsg = QString("Attempt to create account for username: %1 at %2\n")
    .arg(username)
        .arg(QDateTime::currentDateTime().toString()); // Log the account creation attempt
    qDebug() << logMsg; // Log the attempt

    QString filePath = QDir::currentPath() + "/LOGINS.txt"; // Path to the login file
    logMsg += QString("File path: %1\n").arg(filePath);
    qDebug() << "File path:" << filePath;

    QFile file(filePath); // Create a QFile object for the login file
    // Attempt to open the file for reading and writing
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        logMsg += QString("Failed to open LOGINS.txt: %1\n").arg(file.errorString());
        qDebug() << logMsg; // Log the failure
        // Show error message if file cannot be opened
        QMessageBox::critical(this, "Error", "Unable to open LOGINS.txt: " + file.errorString());
        writeLog(logMsg); // Write log message
        return; // Exit the function
    }

    // Check if the username already exists in the file
    QTextStream in(&file); // Create a text stream for reading
    while (!in.atEnd()) { // Loop through each line
        QString line = in.readLine(); // Read a line from the file
        // If the username already exists, show a warning
        if (line.startsWith(username + ":")) {
            logMsg += "Username already exists\n";
            qDebug() << logMsg; // Log the message
            QMessageBox::warning(this, "Account Exists", "This username is already taken. Please choose another.");
            file.close(); // Close the file
            writeLog(logMsg); // Write log message
            return; // Exit the function
        }
    }

    // Append the new username and password to the file
    QTextStream out(&file); // Create a text stream for writing
    out << username << ":" << password << "\n"; // Write the new account details
    out.flush(); // Ensure the data is written to the file

    // Check for errors during writing
    if (out.status() != QTextStream::Ok) {
        logMsg += QString("Failed to write to LOGINS.txt: %1\n").arg(out.status());
        qDebug() << logMsg; // Log the error message
        // Show error message if writing fails
        QMessageBox::critical(this, "Error", "Failed to write to LOGINS.txt");
        file.close(); // Close the file
        writeLog(logMsg); // Write log message
        return; // Exit the function
    }

    file.close(); // Close the file after writing

    // Check for errors when closing the file
    if (file.error() != QFile::NoError) {
        logMsg += QString("Error occurred while closing LOGINS.txt: %1\n").arg(file.errorString());
        qDebug() << logMsg; // Log the error message
        QMessageBox::critical(this, "Error", "Error occurred while closing LOGINS.txt: " + file.errorString());
        writeLog(logMsg); // Write log message
        return; // Exit the function
    }

    logMsg += "Account created successfully\n"; // Log successful account creation
    qDebug() << logMsg; // Log the success message
    QMessageBox::information(this, "Account Created", "Your account has been created successfully. You can now log in."); // Inform the user
    writeLog(logMsg); // Write log message

    // Verify the contents of the file after creation
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) { // Attempt to reopen the file for reading
        QTextStream verifyStream(&file); // Create a text stream for verification
        QString fileContents = verifyStream.readAll(); // Read all contents of the file
        logMsg += "File contents after creation:\n" + fileContents; // Log the contents
        file.close(); // Close the file
    } else {
        logMsg += "Failed to open file for verification\n"; // Log verification failure
    }

    writeLog(logMsg); // Write log message
}

// Function to write log messages to a separate log file
void LoginDialog::writeLog(const QString &message)
{
    QString logFilePath = QDir::currentPath() + "/login_log.txt"; // Path to the log file
    QFile logFile(logFilePath); // Create a QFile object for the log file
    // Attempt to open the log file for appending
    if (logFile.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream logStream(&logFile); // Create a text stream for writing logs
        logStream << message << "\n\n"; // Write the message and a new line
        logFile.close(); // Close the log file
    }
}
