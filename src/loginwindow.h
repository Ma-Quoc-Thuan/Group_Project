#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>

class LoginWindow : public QWidget {
    Q_OBJECT

private:
    QLineEdit* txtUsername;
    QLineEdit* txtPassword;
    QPushButton* btnLogin;

public:
    explicit LoginWindow(QWidget *parent = nullptr);

private slots:
    void handleLogin();
};

#endif // LOGINWINDOW_H