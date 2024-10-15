#include "logindialog.h"
#include <QVBoxLayout>
#include <QLabel>

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Login");

    usernameEdit = new QLineEdit(this);
    passwordEdit = new QLineEdit(this);
    passwordEdit->setEchoMode(QLineEdit::Password);

    loginButton = new QPushButton("Login", this);
    createAccountButton = new QPushButton("Create Account", this);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("Username:", this));
    layout->addWidget(usernameEdit);
    layout->addWidget(new QLabel("Password:", this));
    layout->addWidget(passwordEdit);
    layout->addWidget(loginButton);
    layout->addWidget(createAccountButton);

    connect(loginButton, &QPushButton::clicked, this, &LoginDialog::onLoginClicked);
    connect(createAccountButton, &QPushButton::clicked, this, &LoginDialog::onCreateAccountClicked);
}

void LoginDialog::onLoginClicked()
{
    QString username = usernameEdit->text();
    QString password = passwordEdit->text();

    if (checkCredentials(username, password)) {
        m_username = username;
        accept();
    } else {
        QMessageBox::warning(this, "Login Failed", "Incorrect username or password.");
    }
}

void LoginDialog::onCreateAccountClicked()
{
    QString username = usernameEdit->text();
    QString password = passwordEdit->text();

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Invalid Input", "Please enter both username and password.");
        return;
    }

    createAccount(username, password);
    // After creating the account, you might want to automatically log the user in
    // or clear the input fields for them to log in manually
    usernameEdit->clear();
    passwordEdit->clear();
}

bool LoginDialog::checkCredentials(const QString &username, const QString &password)
{
    QFile file("LOGINS.txt");
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text))
        return false;

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList parts = line.split(':');
        if (parts.size() == 2 && parts[0] == username && parts[1] == password) {
            file.close();
            return true;
        }
    }

    file.close();
    return false;
}

void LoginDialog::createAccount(const QString &username, const QString &password)
{
    qDebug() << "Attempting to create account for username:" << username;

    QFile file("LOGINS.txt");
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        qDebug() << "Failed to open LOGINS.txt:" << file.errorString();
        QMessageBox::critical(this, "Error", "Unable to open LOGINS.txt: " + file.errorString());
        return;
    }

    // Check if the username already exists
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.startsWith(username + ":")) {
            qDebug() << "Username already exists:" << username;
            QMessageBox::warning(this, "Account Exists", "This username is already taken. Please choose another.");
            file.close();
            return;
        }
    }

    // Append the new user information to the file
    QTextStream out(&file);
    out << username << ":" << password << "\n";

    if (out.status() != QTextStream::Ok) {
        qDebug() << "Failed to write to LOGINS.txt:" << out.status();
        QMessageBox::critical(this, "Error", "Failed to write to LOGINS.txt");
        file.close();
        return;
    }

    file.close();

    if (file.error() != QFile::NoError) {
        qDebug() << "Error occurred while closing LOGINS.txt:" << file.errorString();
        QMessageBox::critical(this, "Error", "Error occurred while closing LOGINS.txt: " + file.errorString());
        return;
    }

    qDebug() << "Account created successfully for username:" << username;
    QMessageBox::information(this, "Account Created", "Your account has been created successfully. You can now log in.");
}

