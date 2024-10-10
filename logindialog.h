#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    LoginDialog(QWidget *parent = nullptr);
    QString getUsername() const { return m_username; }

private slots:
    void onLoginClicked();
    void onCreateAccountClicked();

private:
    void writeLog(const QString &message);
    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;
    QPushButton *loginButton;
    QPushButton *createAccountButton;
    QString m_username;

    bool checkCredentials(const QString &username, const QString &password);
    void createAccount(const QString &username, const QString &password);
};

#endif // LOGINDIALOG_H
