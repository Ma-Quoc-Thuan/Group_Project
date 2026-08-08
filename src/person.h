#ifndef PERSON_H
#define PERSON_H

#include <QString>

// LỚP CHA: Chứa các thông tin cơ bản của một con người
class Person {
protected: // Dùng protected để lớp con (Student, Teacher) có thể truy cập được
    QString fullName;
    QString birthDate;
    QString hometown;

public:
    Person() = default;

    // Constructor
    Person(const QString& name, const QString& dob, const QString& home)
        : fullName(name), birthDate(dob), hometown(home) {}

    // Khai báo Destructor ảo (Rất quan trọng trong OOP để tránh rò rỉ bộ nhớ)
    virtual ~Person() = default;

    // Các hàm Getters
    QString getFullName() const { return fullName; }
    QString getBirthDate() const { return birthDate; }
    QString getHometown() const { return hometown; }

    // Các hàm Setters
    void setFullName(const QString& name) { fullName = name; }
    void setBirthDate(const QString& dob) { birthDate = dob; }
    void setHometown(const QString& home) { hometown = home; }

    // ĐA HÌNH (Polymorphism): Hàm ảo để lớp con ghi đè
    virtual QString getRole() const {
        return "Person";
    }
};

#endif // PERSON_H