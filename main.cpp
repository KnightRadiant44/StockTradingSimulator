#include <QApplication>
#include <QDir>
#include <QDebug>
#include "mainwindow.h"
#include "logindialog.h"
#include <QCoreApplication>
#include <QProcess>
#include <QTextStream>
#include <QFile>

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);

    qDebug() << "Application started";
    qDebug() << "Current working directory:" << QDir::currentPath();
    qDebug() << "LOGINS.txt full path:" << QDir::currentPath() + "/LOGINS.txt";

    // Check if LOGINS.txt exists
    QFile loginsFile("LOGINS.txt");
    if (loginsFile.exists()) {
        qDebug() << "LOGINS.txt exists";
    } else {
        qDebug() << "LOGINS.txt does not exist, it will be created when a new account is added";
    }

    LoginDialog loginDialog;
    if (loginDialog.exec() == QDialog::Accepted) {
        qDebug() << "Login successful, opening main window";
        MainWindow mainWindow(loginDialog.getUsername());
        mainWindow.show();
        return a.exec();
    } else {
        qDebug() << "Login canceled or failed";
    }

    return 0;
}
