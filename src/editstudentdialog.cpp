#if defined(_MSC_VER) && (_MSC_VER >= 1600)
#pragma execution_character_set("utf-8")
#endif

#include "editstudentdialog.h"
#include "databasemanager.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>

EditStudentDialog::EditStudentDialog(const QString& mssv, QWidget *parent)
    : QDialog(parent), currentMssv(mssv) {

    setWindowTitle(QString::fromUtf8(u8"Chỉnh sửa thông tin Sinh viên"));
    resize(320, 350);
    QVBoxLayout* layout = new QVBoxLayout(this);

    // Lấy dữ liệu sinh viên hiện tại lên
    Student s = DatabaseManager::getInstance()->getStudent(mssv);

    // --- TẠO CÁC Ô NHẬP LIỆU ---
    layout->addWidget(new QLabel(QString::fromUtf8(u8"Mã số SV (MSSV):")));
    txtMssv = new QLineEdit(s.getMssv(), this);
    layout->addWidget(txtMssv);

    layout->addWidget(new QLabel(QString::fromUtf8(u8"Họ và tên:")));
    txtName = new QLineEdit(s.getFullName(), this);
    layout->addWidget(txtName);

    layout->addWidget(new QLabel(QString::fromUtf8(u8"Ngày sinh (DD/MM/YYYY):")));
    txtDob = new QLineEdit(s.getBirthDate(), this);
    layout->addWidget(txtDob);

    layout->addWidget(new QLabel(QString::fromUtf8(u8"Quê quán:")));
    txtHometown = new QLineEdit(s.getHometown(), this);
    layout->addWidget(txtHometown);

    layout->addWidget(new QLabel(QString::fromUtf8(u8"Lớp:")));
    txtClass = new QLineEdit(s.getClassName(), this);
    layout->addWidget(txtClass);

    QPushButton* btnSave = new QPushButton(QString::fromUtf8(u8"💾 Lưu thông tin"), this);
    btnSave->setStyleSheet("background-color: #2563eb; color: white; padding: 8px; border-radius: 4px; font-weight: bold;");
    connect(btnSave, &QPushButton::clicked, this, &EditStudentDialog::saveStudent);

    layout->addStretch();
    layout->addWidget(btnSave);
}

void EditStudentDialog::saveStudent() {
    QString newMssv = txtMssv->text().trimmed();

    // Kiểm tra không cho để trống MSSV
    if (newMssv.isEmpty()) {
        QMessageBox::warning(this, QString::fromUtf8(u8"Lỗi"), QString::fromUtf8(u8"MSSV không được để trống!"));
        return;
    }

    // Gọi sinh viên cũ ra
    Student s = DatabaseManager::getInstance()->getStudent(currentMssv);

    // Cập nhật các thông tin mới
    s.setFullName(txtName->text().trimmed());
    s.setBirthDate(txtDob->text().trimmed());
    s.setHometown(txtHometown->text().trimmed());
    s.setClassName(txtClass->text().trimmed());

    // NẾU NGƯỜI DÙNG CÓ THAY ĐỔI MSSV
    if (newMssv != currentMssv) {
        // Cập nhật MSSV mới cho đối tượng Student
        s.setMssv(newMssv);
        // Xóa dòng sinh viên cũ trong Database (để tránh bị nhân đôi)
        DatabaseManager::getInstance()->deleteStudent(currentMssv);
    }

    // Đẩy lại vào DB (lưu mới hoặc ghi đè)
    DatabaseManager::getInstance()->addOrUpdateStudent(s);

    QMessageBox::information(this, QString::fromUtf8(u8"Thành công"),
                             QString::fromUtf8(u8"Đã cập nhật thông tin sinh viên!"));
    accept(); // Đóng hộp thoại
}