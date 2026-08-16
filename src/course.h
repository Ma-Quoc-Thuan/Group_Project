#ifndef COURSE_H
#define COURSE_H

#include <QString>

struct Course {
    QString courseId;
    QString courseCode;
    QString courseName;
    int credits = 3;

    // SĨ SỐ HỌC PHẦN (Mới thêm)
    int maxCapacity = 40;     // Sĩ số tối đa
    int currentEnrolled = 0;  // Sĩ số hiện tại
    QString dayOfWeek;
    QString timeSlot;
    QString room;
    QString targetClass;

    double midtermScore = 0.0;
    double finalScore = 0.0;
    double score = 0.0;

    QString getCode() const {
        return !courseId.isEmpty() ? courseId : courseCode;
    }

    void setCode(const QString& code) {
        courseId = code;
        courseCode = code;
    }

    double getAverage() const {
        if (midtermScore > 0 || finalScore > 0) {
            return (midtermScore * 0.4) + (finalScore * 0.6);
        }
        return score;
    }

    // HÀM BỔ SUNG: Kiểm tra lớp đã đầy sĩ số chưa
    bool isFull() const {
        return currentEnrolled >= maxCapacity;
    }
};

#endif // COURSE_H