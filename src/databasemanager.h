#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QString>
#include <QVector>
#include <QDateTime>
#include "student.h"

class DatabaseManager {
private:
    static DatabaseManager* instance;
    QVector<Student> students;
    QVector<Course> availableCourses;

    QDateTime regStartTime;
    QDateTime regEndTime;

    DatabaseManager();

public:
    static DatabaseManager* getInstance();
    void initSampleData();
    Student getStudent(const QString& mssv);
    QVector<Student> getAllStudents() const;
    void addOrUpdateStudent(const Student& student);
    void deleteStudent(const QString& mssv);

    QVector<Course> getAvailableCourses() const;
    void addAvailableCourse(const Course& course);

    // Cấu hình thời hạn đăng ký học phần
    void setRegistrationPeriod(const QDateTime& start, const QDateTime& end) {
        regStartTime = start;
        regEndTime = end;
    }
    QDateTime getRegStartTime() const { return regStartTime; }
    QDateTime getRegEndTime() const { return regEndTime; }

    // Kiểm tra hiện tại có trong thời gian mở đăng ký không
    bool isRegistrationOpen() const {
        if (!regStartTime.isValid() || !regEndTime.isValid()) return true;
        QDateTime now = QDateTime::currentDateTime();
        return (now >= regStartTime && now <= regEndTime);
    }
};

#endif // DATABASEMANAGER_H