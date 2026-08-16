#if defined(_MSC_VER) && (_MSC_VER >= 1600)
#pragma execution_character_set("utf-8")
#endif

#include "databasemanager.h"

DatabaseManager* DatabaseManager::instance = nullptr;

DatabaseManager* DatabaseManager::getInstance() {
    if (!instance) {
        instance = new DatabaseManager();
    }
    return instance;
}

DatabaseManager::DatabaseManager() {
    initSampleData();
}

void DatabaseManager::initSampleData() {
    availableCourses.clear();

    Course c1;
    c1.setCode("INT101");
    c1.courseName = QString::fromUtf8("Lập trình C++");
    c1.credits = 3;
    c1.maxCapacity = 40;
    c1.targetClass = QString::fromUtf8("Tất cả");
    availableCourses.append(c1);

    Course c2;
    c2.setCode("INT102");
    c2.courseName = QString::fromUtf8("Cấu trúc dữ liệu");
    c2.credits = 4;
    c2.maxCapacity = 40;
    c2.targetClass = "CNTT01";
    availableCourses.append(c2);

    Course c3;
    c3.setCode("MATH10");
    c3.courseName = QString::fromUtf8("Toán cao cấp");
    c3.credits = 3;
    c3.maxCapacity = 40;
    c3.targetClass = QString::fromUtf8("Tất cả");
    availableCourses.append(c3);

    Course c4;
    c4.setCode("ENG101");
    c4.courseName = QString::fromUtf8("Tiếng Anh chuyên ngành");
    c4.credits = 2;
    c4.maxCapacity = 40;
    c4.targetClass = QString::fromUtf8("Tất cả");
    availableCourses.append(c4);
}

QVector<Student> DatabaseManager::getAllStudents() const {
    return students;
}

void DatabaseManager::deleteStudent(const QString& mssv) {
    QString targetMssv = mssv.trimmed();
    for (int i = 0; i < students.size(); ++i) {
        if (students[i].getMssv().trimmed() == targetMssv) {
            students.removeAt(i);
            return;
        }
    }
}

void DatabaseManager::addOrUpdateStudent(const Student& student) {
    QString targetMssv = student.getMssv().trimmed();
    for (int i = 0; i < students.size(); ++i) {
        if (students[i].getMssv().trimmed() == targetMssv) {
            students[i] = student;
            return;
        }
    }
    students.append(student);
}

Student DatabaseManager::getStudent(const QString& mssv) {
    QString targetMssv = mssv.trimmed();
    for (const auto& s : students) {
        if (s.getMssv().trimmed() == targetMssv) {
            return s;
        }
    }
    return Student();
}

QVector<Course> DatabaseManager::getAvailableCourses() const {
    QVector<Course> resultList = availableCourses;

    for (auto& course : resultList) {
        int count = 0;
        for (const auto& student : students) {
            for (const auto& sc : student.getCourses()) {
                if (sc.courseCode == course.courseCode || sc.getCode() == course.getCode()) {
                    count++;
                    break;
                }
            }
        }
        course.currentEnrolled = count;
    }

    return resultList;
}

void DatabaseManager::addAvailableCourse(const Course& course) {
    for (int i = 0; i < availableCourses.size(); ++i) {
        if (availableCourses[i].courseCode == course.courseCode || availableCourses[i].getCode() == course.getCode()) {
            availableCourses[i] = course;
            return;
        }
    }
    availableCourses.append(course);
}