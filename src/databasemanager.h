#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QString>
#include <QVector>
#include "student.h"

class DatabaseManager {
private:
    static DatabaseManager* instance;
    QVector<Student> students;
    QVector<Course> availableCourses;
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
};

#endif // DATABASEMANAGER_H