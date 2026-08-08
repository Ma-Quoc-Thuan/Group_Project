#ifndef STUDENT_H
#define STUDENT_H

#include "person.h"   // Nhúng file lp cha vào
#include "course.h"
#include <QString>
#include <QVector>

// LP CON: K tha (Inheritance) t lp Person
class Student : public Person {
private:
    QString mssv;
    QString className;
    QString password;
    QVector<Course> enrolledCourses;

public:
    Student() = default;

    // Constructor: Gi hàm khi to ca lp cha (Person)  gán H tên, Ngày sinh, Quê quán
    Student(const QString& id, const QString& name, const QString& dob,
            const QString& home, const QString& cls, const QString& pass = "123456", double gpa = 0.0)
        : Person(name, dob, home), mssv(id), className(cls), password(pass) {
        Q_UNUSED(gpa); // B qua gpa truyn vào vì s t tính toán
    }

    // Getters cho các thuc tính riêng ca Sinh viên
    QString getMssv() const { return mssv; }
    QString getClassName() const { return className; }
    QString getPassword() const { return password; }
    QVector<Course> getCourses() const { return enrolledCourses; }

    // Setters
    void setMssv(const QString& id) { mssv = id; }
    void setClassName(const QString& cls) { className = cls; }
    void setPassword(const QString& pass) { password = pass; }
    void setCourses(const QVector<Course>& courses) { enrolledCourses = courses; }

    void addCourse(const Course& c) { enrolledCourses.append(c); }

    // HÀM X LÝ LOGIC: Vit trc tip trong file .h
    double calculateGPA() const {
        if (enrolledCourses.isEmpty()) return 0.0;

        double totalScore = 0.0;
        int totalCredits = 0;

        for (const Course& c : enrolledCourses) {
            // Gi s TB môn = 30% Gia k + 70% Cui k
            double courseAvg = (c.midtermScore * 0.3) + (c.finalScore * 0.7);
            totalScore += courseAvg * c.credits;
            totalCredits += c.credits;
        }

        return totalCredits > 0 ? (totalScore / totalCredits) : 0.0;
    }

    // A HÌNH (Polymorphism): Ghi è (Override) hàm ca lp cha
    virtual QString getRole() const override {
        return "Student";
    }
};

#endif // STUDENT_H