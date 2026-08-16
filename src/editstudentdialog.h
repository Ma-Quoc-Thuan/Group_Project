#ifndef EDITSTUDENTDIALOG_H
#define EDITSTUDENTDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include "student.h"

class EditStudentDialog : public QDialog {
    Q_OBJECT
public:
    explicit EditStudentDialog(const QString& mssv, QWidget *parent = nullptr);

private slots:
    void saveStudent();

private:
    QString currentMssv;

    QLineEdit *txtMssv; // Thêm ô chỉnh sửa MSSV
    QLineEdit *txtName;
    QLineEdit *txtDob;
    QLineEdit *txtHometown;
    QLineEdit *txtClass;
};

#endif // EDITSTUDENTDIALOG_H