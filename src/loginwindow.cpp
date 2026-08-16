#include "loginwindow.h"
#include "adminwindow.h"
#include "studentwindow.h"
#include "databasemanager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>

LoginWindow::LoginWindow(QWidget *parent) : QWidget(parent) {
    setWindowTitle(QString::fromUtf8("SIMS - Đăng Nhập"));
    resize(400, 320);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(40, 40, 40, 40);
    layout->setSpacing(16);

    QLabel* lblTitle = new QLabel("SIMS LOGIN");
    lblTitle->setObjectName("lblLogoText");
    lblTitle->setAlignment(Qt::AlignCenter);

    txtUsername = new QLineEdit();
    txtUsername->setPlaceholderText(QString::fromUtf8("Tên đăng nhập (Admin: admin | SV: 20230001)"));

    txtPassword = new QLineEdit();
    txtPassword->setEchoMode(QLineEdit::Password);
    txtPassword->setPlaceholderText(QString::fromUtf8("Mật khẩu (Mặc định: 123456)"));

    btnLogin = new QPushButton(QString::fromUtf8("Đăng Nhập"));
    btnLogin->setObjectName("btnSuccess");

    layout->addWidget(lblTitle);
    layout->addWidget(txtUsername);
    layout->addWidget(txtPassword);
    layout->addWidget(btnLogin);

    connect(btnLogin, &QPushButton::clicked, this, &LoginWindow::handleLogin);
}

void LoginWindow::handleLogin() {
    QString user = txtUsername->text().trimmed();
    QString pass = txtPassword->text().trimmed();

    if (user == "admin" && pass == "admin") {
        AdminWindow* adminWin = new AdminWindow();
        adminWin->show();
        this->close();
    } else {
        Student s = DatabaseManager::getInstance()->getStudent(user);

        // Lấy mật khẩu chuẩn của sinh viên từ database (Nếu chưa đổi thì mặc định là "123456")
        QString validPass = s.getPassword().isEmpty() ? "123456" : s.getPassword();

        // Chỉ kiểm tra khớp chính xác mật khẩu hiện tại (Đã loại bỏ || pass == "123456")
        if (!s.getMssv().isEmpty() && pass == validPass) {
            StudentWindow* studentWin = new StudentWindow(user);
            studentWin->show();
            this->close();
        } else {
            QMessageBox::warning(this, QString::fromUtf8("Lỗi"), QString::fromUtf8("Tài khoản hoặc mật khẩu không chính xác!"));
        }
    }
}