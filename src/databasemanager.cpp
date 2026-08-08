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
    // Xóa dữ liệu cũ nếu có
    availableCourses.clear();

    // Khởi tạo rõ ràng từng môn học
    Course c1;
    c1.setCode("INT101");
    c1.courseName = QString::fromUtf8("Lập trình C++");
    c1.credits = 3;
    c1.targetClass = QString::fromUtf8("Tất cả");
    availableCourses.append(c1);

    Course c2;
    c2.setCode("INT102");
    c2.courseName = QString::fromUtf8("Cấu trúc dữ liệu");
    c2.credits = 4;
    c2.targetClass = "CNTT01";
    availableCourses.append(c2);

    Course c3;
    c3.setCode("MATH10");
    c3.courseName = QString::fromUtf8("Toán cao cấp");
    c3.credits = 3;
    c3.targetClass = QString::fromUtf8("Tất cả");
    availableCourses.append(c3);

    Course c4;
    c4.setCode("ENG101");
    c4.courseName = QString::fromUtf8("Tiếng Anh chuyên ngành");
    c4.credits = 2;
    c4.targetClass = QString::fromUtf8("Tất cả");
    availableCourses.append(c4);

    // (Tùy chọn) Nếu bạn có khởi tạo danh sách sinh viên mẫu ở đây, hãy giữ nguyên phần đó
    // Ví dụ: students.append(Student("B22DCCN123", "Nguyễn Văn A", ...));
}

QVector<Student> DatabaseManager::getAllStudents() const {
    return students;
}

void DatabaseManager::deleteStudent(const QString& mssv) {
    // Dùng trimmed() để loại bỏ các khoảng trắng hoặc ký tự ẩn khi so sánh
    QString targetMssv = mssv.trimmed();

    for (int i = 0; i < students.size(); ++i) {
        if (students[i].getMssv().trimmed() == targetMssv) {
            students.removeAt(i);
            return; // Đã xóa xong thì thoát hàm
        }
    }
}

void DatabaseManager::addOrUpdateStudent(const Student& student) {
    QString targetMssv = student.getMssv().trimmed();

    for (int i = 0; i < students.size(); ++i) {
        // Nếu tìm thấy MSSV đã tồn tại -> Cập nhật (Ghi đè)
        if (students[i].getMssv().trimmed() == targetMssv) {
            students[i] = student;
            return;
        }
    }
    // Nếu duyệt hết danh sách không thấy -> Thêm sinh viên mới
    students.append(student);
}

Student DatabaseManager::getStudent(const QString& mssv) {
    QString targetMssv = mssv.trimmed();
    for (const auto& s : students) {
        if (s.getMssv().trimmed() == targetMssv) {
            return s;
        }
    }
    return Student(); // Trả về đối tượng rỗng nếu không tìm thấy
}
QVector<Course> DatabaseManager::getAvailableCourses() const {
    return availableCourses;
}

void DatabaseManager::addAvailableCourse(const Course& course) {
    for (const auto& c : availableCourses) {
        if (c.courseCode == course.courseCode) return; // Tránh trùng mã
    }
    availableCourses.append(course);
}