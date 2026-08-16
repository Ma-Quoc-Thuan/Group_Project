#ifndef CHANGEPASSWORDDIALOG_H
#define CHANGEPASSWORDDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include "databasemanager.h"

class ChangePasswordDialog : public QDialog {
    Q_OBJECT
private:
    QString currentMssv;
    QLineEdit* txtOldPass;
    QLineEdit* txtNewPass;
    QLineEdit* txtConfirmPass;

public:
    ChangePasswordDialog(const QString& mssv, QWidget* parent = nullptr)
        : QDialog(parent), currentMssv(mssv) {
        setWindowTitle(QString::fromUtf8("🔑 Thay Đổi Mật Khẩu"));
        setFixedSize(380, 320);

        QVBoxLayout* layout = new QVBoxLayout(this);
        layout->setContentsMargins(20, 20, 20, 20);
        layout->setSpacing(10);

        QLabel* lblTitle = new QLabel(QString::fromUtf8("<h3 style='color:#EDEFF5; margin:0;'>Cập Nhật Mật Khẩu</h3>"), this);
        layout->addWidget(lblTitle);

        layout->addWidget(new QLabel(QString::fromUtf8("Mật khẩu hiện tại:"), this));
        txtOldPass = new QLineEdit(this);
        txtOldPass->setEchoMode(QLineEdit::Password);
        layout->addWidget(txtOldPass);

        layout->addWidget(new QLabel(QString::fromUtf8("Mật khẩu mới:"), this));
        txtNewPass = new QLineEdit(this);
        txtNewPass->setEchoMode(QLineEdit::Password);
        layout->addWidget(txtNewPass);

        layout->addWidget(new QLabel(QString::fromUtf8("Xác nhận mật khẩu mới:"), this));
        txtConfirmPass = new QLineEdit(this);
        txtConfirmPass->setEchoMode(QLineEdit::Password);
        layout->addWidget(txtConfirmPass);

        QHBoxLayout* btnLayout = new QHBoxLayout();
        QPushButton* btnCancel = new QPushButton(QString::fromUtf8("Hủy"), this);
        btnCancel->setObjectName("btnGhost");

        QPushButton* btnSave = new QPushButton(QString::fromUtf8("💾 Lưu Thay Đổi"), this);
        btnSave->setObjectName("btnSuccess");

        btnLayout->addWidget(btnCancel);
        btnLayout->addWidget(btnSave);
        layout->addLayout(btnLayout);

        connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);
        connect(btnSave, &QPushButton::clicked, this, &ChangePasswordDialog::handleChangePassword);
    }

private slots:
    void handleChangePassword() {
        QString oldP = txtOldPass->text().trimmed();
        QString newP = txtNewPass->text().trimmed();
        QString confirmP = txtConfirmPass->text().trimmed();

        Student s = DatabaseManager::getInstance()->getStudent(currentMssv);

        // Mật khẩu hiện tại mặc định là "123456" nếu trong DB đang rỗng
        QString currentPass = s.getPassword().isEmpty() ? "123456" : s.getPassword();

        if (oldP != currentPass) {
            QMessageBox::warning(this, QString::fromUtf8("Lỗi"), QString::fromUtf8("Mật khẩu hiện tại không chính xác!"));
            return;
        }

        if (newP.isEmpty()) {
            QMessageBox::warning(this, QString::fromUtf8("Lỗi"), QString::fromUtf8("Mật khẩu mới không được để trống!"));
            return;
        }

        if (newP != confirmP) {
            QMessageBox::warning(this, QString::fromUtf8("Lỗi"), QString::fromUtf8("Xác nhận mật khẩu mới không khớp!"));
            return;
        }

        s.setPassword(newP);
        DatabaseManager::getInstance()->addOrUpdateStudent(s);

        QMessageBox::information(this, QString::fromUtf8("Thành công"), QString::fromUtf8("Đổi mật khẩu thành công!"));
        accept();
    }
};

#endif // CHANGEPASSWORDDIALOG_H