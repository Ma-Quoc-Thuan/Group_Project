#ifndef STUDENT_H
#define STUDENT_H

#include "person.h"
#include "course.h"
#include <QString>
#include <QVector>

class Student : public Person {
private:
    QString mssv;
    QString className;
    QString password;
    QVector<Course> enrolledCourses;
    double initialGpa = 0.0; // Bin lu GPA khi to t CSV

public:
    Student() = default;
    Student(const QString& id, const QString& name, const QString& dob,
            const QString& home, const QString& cls, const QString& pass = "123456", double gpa = 0.0)
        : Person(name, dob, home), mssv(id), className(cls), password(pass), initialGpa(gpa) {}

    // Getters & Setters
    QString getMssv() const { return mssv; }
    QString getClassName() const { return className; }
    QString getPassword() const { return password; }
    QVector<Course> getCourses() const { return enrolledCourses; }

    // H tr c 2 kiu vit hoa/thng  tránh li biên dch
    double getGPA() const { return initialGpa; }
    double getGpa() const { return initialGpa; }

    void setMssv(const QString& id) { mssv = id; }
    void setClassName(const QString& cls) { className = cls; }
    void setPassword(const QString& pass) { password = pass; }
    void setCourses(const QVector<Course>& courses) { enrolledCourses = courses; }
    void setGPA(double gpa) { initialGpa = gpa; }
    void setGpa(double gpa) { initialGpa = gpa; }
    void addCourse(const Course& c) { enrolledCourses.append(c); }

    // Tính GPA chính thc t danh sách môn hc ã ng ký
    double calculateGPA() const {
        if (enrolledCourses.isEmpty()) return 0.0;
        double totalScore = 0.0;
        int gradedCredits = 0;
        for (const Course& c : enrolledCourses) {
            if (c.midtermScore > 0.0 || c.finalScore > 0.0 || c.score > 0.0) {
                double courseAvg = (c.midtermScore * 0.3) + (c.finalScore * 0.7);
                if (courseAvg == 0.0 && c.score > 0.0) {
                    courseAvg = c.score;
                }
                totalScore += courseAvg * c.credits;
                gradedCredits += c.credits;
            }
        }
        if (gradedCredits == 0) return 0.0;
        return totalScore / gradedCredits;
    }

    // LOGIC CNH BÁO HC TP
    int getTotalRegisteredCredits() const {
        int sum = 0;
        for (const Course& c : enrolledCourses) {
            sum += c.credits;
        }
        return sum;
    }

    bool isAcademicWarning() const {
        if (enrolledCourses.isEmpty()) return false;
        int gradedCredits = 0;
        for (const Course& c : enrolledCourses) {
            if (c.midtermScore > 0.0 || c.finalScore > 0.0 || c.score > 0.0) {
                gradedCredits += c.credits;
            }
        }
        if (gradedCredits == 0) return false;
        return calculateGPA() < 5.0;
    }

    int getMaxAllowedCredits() const {
        if (isAcademicWarning()) {
            return 14; // Cnh báo GPA (< 5.0): Gii hn 14 tín ch
        }
        return 24;     // Bình thng: Gii hn 24 tín ch
    }

    virtual QString getRole() const override {
        return "Student";
    }
};

#endif // STUDENT_H