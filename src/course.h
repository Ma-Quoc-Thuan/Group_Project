#ifndef COURSE_H
#define COURSE_H

#include <QString>

struct Course {
    // Mã môn học (Hỗ trợ cả courseId lẫn courseCode để tương thích toàn bộ project)
    QString courseId;
    QString courseCode;
    QString courseName;
    int credits = 3;

    // Lịch học & Phòng học
    QString dayOfWeek;
    QString timeSlot;
    QString room;
    QString targetClass;

    // Điểm số
    double midtermScore = 0.0;
    double finalScore = 0.0;
    double score = 0.0;

    // Lấy mã môn học chuẩn
    QString getCode() const {
        return !courseId.isEmpty() ? courseId : courseCode;
    }

    // Đồng bộ mã môn học
    void setCode(const QString& code) {
        courseId = code;
        courseCode = code;
    }

    // Tính điểm trung bình môn (40% giữa kỳ + 60% cuối kỳ)
    double getAverage() const {
        if (midtermScore > 0 || finalScore > 0) {
            return (midtermScore * 0.4) + (finalScore * 0.6);
        }
        return score;
    }
};

#endif // COURSE_H