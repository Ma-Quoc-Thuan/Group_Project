#ifndef USER_H
#define USER_H

#include <QString>

// Lớp cha trừu tượng (Abstraction)
class User {
protected:
    QString username;
    QString password;

public:
    User(QString u, QString p) : username(u), password(p) {}
    virtual ~User() {}

    // Phương thức thuần ảo tạo tính Đa hình (Polymorphism)
    virtual QString getRole() const = 0;

    QString getUsername() const { return username; }
    bool verifyPassword(const QString& p) const { return password == p; }
};

// Lớp Admin kế thừa từ User (Inheritance)
class Admin : public User {
public:
    Admin(QString u, QString p) : User(u, p) {}
    QString getRole() const override { return "ADMIN"; }
};

// Lớp StudentUser kế thừa từ User
class StudentUser : public User {
private:
    QString studentMssv;
public:
    StudentUser(QString u, QString p, QString mssv) : User(u, p), studentMssv(mssv) {}
    QString getRole() const override { return "STUDENT"; }
    QString getStudentMssv() const { return studentMssv; }
};

#endif // USER_H