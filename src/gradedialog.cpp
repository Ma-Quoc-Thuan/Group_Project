#include "gradedialog.h"
#include "databasemanager.h"
#include "student.h"
#include "course.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>

GradeDialog::GradeDialog(const QString& mssv, QWidget* parent)
    : QDialog(parent), currentMssv(mssv) {

    setWindowTitle(QString::fromUtf8("Quản Lý Điểm & Đăng Ký Học Phần · MSSV: ") + currentMssv);
    resize(1050, 560);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(18, 18, 18, 18);
    mainLayout->setSpacing(12);

    tableWidget = new QTableWidget(this);
    tableWidget->setColumnCount(10);
    tableWidget->setHorizontalHeaderLabels({
        QString::fromUtf8("Mã Môn"), QString::fromUtf8("Tên Môn"), QString::fromUtf8("Tín Chỉ"),
        QString::fromUtf8("Thứ"), QString::fromUtf8("Tiết"), QString::fromUtf8("Phòng"),
        QString::fromUtf8("Điểm GK"), QString::fromUtf8("Điểm CK"), QString::fromUtf8("ĐTB Môn"), QString::fromUtf8("Thao Tác")
    });
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableWidget->verticalHeader()->setVisible(false);
    mainLayout->addWidget(tableWidget);

    btnSaveScores = new QPushButton(QString::fromUtf8("💾 Lưu Điểm & Lịch Học Trên Bảng"), this);
    btnSaveScores->setObjectName("btnPrimary");
    mainLayout->addWidget(btnSaveScores);
    connect(btnSaveScores, &QPushButton::clicked, this, &GradeDialog::saveScores);

    QVBoxLayout* formLayout = new QVBoxLayout();

    QHBoxLayout* row1 = new QHBoxLayout();
    txtCourseId = new QLineEdit(this); txtCourseId->setPlaceholderText(QString::fromUtf8("Mã môn"));
    txtCourseName = new QLineEdit(this); txtCourseName->setPlaceholderText(QString::fromUtf8("Tên môn"));
    txtCredits = new QLineEdit(this); txtCredits->setPlaceholderText(QString::fromUtf8("Tín chỉ"));
    row1->addWidget(txtCourseId); row1->addWidget(txtCourseName); row1->addWidget(txtCredits);

    QHBoxLayout* row2 = new QHBoxLayout();
    txtDay = new QLineEdit(this); txtDay->setPlaceholderText(QString::fromUtf8("Thứ (VD: T2)"));
    txtTime = new QLineEdit(this); txtTime->setPlaceholderText(QString::fromUtf8("Tiết (VD: 1-3)"));
    txtRoom = new QLineEdit(this); txtRoom->setPlaceholderText(QString::fromUtf8("Phòng (VD: A101)"));

    // Đã xóa việc gọi txtMidterm và txtFinal ở đây

    row2->addWidget(txtDay); row2->addWidget(txtTime); row2->addWidget(txtRoom);

    QPushButton* btnAdd = new QPushButton(QString::fromUtf8("➕ Đăng Ký Môn Cho SV Này"), this);
    btnAdd->setObjectName("btnSuccess");
    row2->addWidget(btnAdd);

    formLayout->addLayout(row1);
    formLayout->addLayout(row2);
    mainLayout->addLayout(formLayout);

    lblGpaSummary = new QLabel(this);
    lblGpaSummary->setStyleSheet("font-size: 14px; font-weight: bold; color: #1FA97A; margin-top: 5px;");
    mainLayout->addWidget(lblGpaSummary);

    connect(btnAdd, &QPushButton::clicked, this, &GradeDialog::addCourse);

    loadCourseData();
}

void GradeDialog::loadCourseData() {
    Student s = DatabaseManager::getInstance()->getStudent(currentMssv);
    QVector<Course> courses = s.getCourses();

    tableWidget->setRowCount(0);
    for (int i = 0; i < courses.size(); ++i) {
        const auto& c = courses[i];
        tableWidget->insertRow(i);

        QTableWidgetItem* idItem = new QTableWidgetItem(c.courseId);
        idItem->setFlags(idItem->flags() & ~Qt::ItemIsEditable); // Không cho sửa mã môn
        tableWidget->setItem(i, 0, idItem);

        QTableWidgetItem* nameItem = new QTableWidgetItem(c.courseName);
        nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsEditable); // Không cho sửa tên môn
        tableWidget->setItem(i, 1, nameItem);

        QTableWidgetItem* creditItem = new QTableWidgetItem(QString::number(c.credits));
        creditItem->setFlags(creditItem->flags() & ~Qt::ItemIsEditable); // Không cho sửa tín chỉ
        tableWidget->setItem(i, 2, creditItem);

        // Các cột Thứ, Tiết, Phòng, Điểm được phép nháy đúp để sửa
        tableWidget->setItem(i, 3, new QTableWidgetItem(c.dayOfWeek));
        tableWidget->setItem(i, 4, new QTableWidgetItem(c.timeSlot));
        tableWidget->setItem(i, 5, new QTableWidgetItem(c.room));

        tableWidget->setItem(i, 6, new QTableWidgetItem(QString::number(c.midtermScore, 'f', 1)));
        tableWidget->setItem(i, 7, new QTableWidgetItem(QString::number(c.finalScore, 'f', 1)));

        QTableWidgetItem* avgItem = new QTableWidgetItem(QString::number(c.getAverage(), 'f', 2));
        avgItem->setFlags(avgItem->flags() & ~Qt::ItemIsEditable); // Điểm TB tự tính, không cho sửa tay
        tableWidget->setItem(i, 8, avgItem);

        QPushButton* btnDel = new QPushButton(QString::fromUtf8("Hủy Đăng Ký"));
        btnDel->setObjectName("btnDanger");
        connect(btnDel, &QPushButton::clicked, this, [this, i](){
            deleteCourse(i);
        });
        tableWidget->setCellWidget(i, 9, btnDel);
    }

    lblGpaSummary->setText(QString::fromUtf8("🏆 GPA Trung Bình Tích Lũy Hiện Tại: %1 / 10.0")
                               .arg(QString::number(s.calculateGPA(), 'f', 2)));
}

void GradeDialog::saveScores() {
    Student s = DatabaseManager::getInstance()->getStudent(currentMssv);
    QVector<Course> courses = s.getCourses();

    for (int i = 0; i < courses.size(); ++i) {
        courses[i].dayOfWeek = tableWidget->item(i, 3)->text().trimmed();
        courses[i].timeSlot = tableWidget->item(i, 4)->text().trimmed();
        courses[i].room = tableWidget->item(i, 5)->text().trimmed();

        double mid = tableWidget->item(i, 6)->text().toDouble();
        double final = tableWidget->item(i, 7)->text().toDouble();

        // Validate điểm (bắt buộc từ 0 đến 10)
        if (mid < 0) mid = 0; if (mid > 10) mid = 10;
        if (final < 0) final = 0; if (final > 10) final = 10;

        courses[i].midtermScore = mid;
        courses[i].finalScore = final;
    }

    s.setCourses(courses);
    DatabaseManager::getInstance()->addOrUpdateStudent(s);

    QMessageBox::information(this, QString::fromUtf8("Thành công"), QString::fromUtf8("Đã lưu thông tin lịch học và điểm!"));
    loadCourseData();
}

void GradeDialog::addCourse() {
    if (txtCourseId->text().isEmpty() || txtCourseName->text().isEmpty()) {
        QMessageBox::warning(this, QString::fromUtf8("Thông báo"), QString::fromUtf8("Vui lòng nhập Mã môn và Tên môn để đăng ký!"));
        return;
    }

    Course c;
    c.courseId = txtCourseId->text().trimmed();
    c.courseName = txtCourseName->text().trimmed();
    c.credits = txtCredits->text().toInt();

    c.dayOfWeek = txtDay->text().trimmed();
    c.timeSlot = txtTime->text().trimmed();
    c.room = txtRoom->text().trimmed();

    // Set mặc định khi mới đăng ký điểm là 0
    c.midtermScore = 0.0;
    c.finalScore = 0.0;

    Student s = DatabaseManager::getInstance()->getStudent(currentMssv);
    s.addCourse(c);
    DatabaseManager::getInstance()->addOrUpdateStudent(s);

    txtCourseId->clear(); txtCourseName->clear(); txtCredits->clear();
    txtDay->clear(); txtTime->clear(); txtRoom->clear();
    // Không cần clear txtMidterm và txtFinal nữa

    loadCourseData();
}

void GradeDialog::deleteCourse(int index) {
    Student s = DatabaseManager::getInstance()->getStudent(currentMssv);
    QVector<Course> courses = s.getCourses();

    if (index >= 0 && index < courses.size()) {
        courses.removeAt(index);
        s.setCourses(courses);
        DatabaseManager::getInstance()->addOrUpdateStudent(s);
        loadCourseData();
    }
}