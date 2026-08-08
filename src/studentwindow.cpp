#if defined(_MSC_VER) && (_MSC_VER >= 1600)
#pragma execution_character_set("utf-8")
#endif

#include "studentwindow.h"
#include "loginwindow.h"
#include "changepassworddialog.h"
#include "databasemanager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QGroupBox>

StudentWindow::StudentWindow(const QString& mssv, QWidget *parent)
    : QMainWindow(parent), currentMssv(mssv) {
    refreshStudentData();
    setupUI();
    applyStylesheet();
    loadStudentData();
}

void StudentWindow::refreshStudentData() {
    currentStudent = DatabaseManager::getInstance()->getStudent(currentMssv);
}

void StudentWindow::setupUI() {
    setWindowTitle(QString::fromUtf8(u8"SIMS PRO - Cổng Thông Tin Sinh Viên"));
    resize(1000, 650);

    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QVBoxLayout* rootLayout = new QVBoxLayout(centralWidget);
    rootLayout->setContentsMargins(15, 15, 15, 15);
    rootLayout->setSpacing(15);

    // --- HEADER BAR ---
    QFrame* headerCard = new QFrame(this);
    headerCard->setObjectName("HeaderCard");
    QHBoxLayout* headerLayout = new QHBoxLayout(headerCard);

    lblAvatar = new QLabel(this);
    lblAvatar->setObjectName("LblAvatar");
    lblAvatar->setFixedSize(50, 50);
    lblAvatar->setAlignment(Qt::AlignCenter);

    QVBoxLayout* infoLayout = new QVBoxLayout();
    lblName = new QLabel(this);
    lblName->setObjectName("LblName");
    lblInfo = new QLabel(this);
    lblInfo->setObjectName("LblInfo");

    infoLayout->addWidget(lblName);
    infoLayout->addWidget(lblInfo);

    btnChangePassword = new QPushButton(QString::fromUtf8(u8"🔑 Đổi mật khẩu"), this);
    btnChangePassword->setObjectName("BtnChangePass");

    btnLogout = new QPushButton(QString::fromUtf8(u8"Đăng xuất"), this);
    btnLogout->setObjectName("BtnLogout");

    connect(btnChangePassword, &QPushButton::clicked, this, &StudentWindow::handleChangePassword);
    connect(btnLogout, &QPushButton::clicked, this, &StudentWindow::handleLogout);

    headerLayout->addWidget(lblAvatar);
    headerLayout->addLayout(infoLayout);
    headerLayout->addStretch();
    headerLayout->addWidget(btnChangePassword);
    headerLayout->addWidget(btnLogout);

    rootLayout->addWidget(headerCard);

    // --- MAIN CONTENT ---
    QHBoxLayout* bodyLayout = new QHBoxLayout();

    // Section 1: Bảng điểm cá nhân
    QGroupBox* boxTranscript = new QGroupBox(QString::fromUtf8(u8"📊 Bảng Điểm Cá Nhân"), this);
    QVBoxLayout* transLayout = new QVBoxLayout(boxTranscript);
    tableTranscript = new QTableWidget(this);
    tableTranscript->setColumnCount(4);
    tableTranscript->setHorizontalHeaderLabels({
        QString::fromUtf8(u8"Mã Môn"),
        QString::fromUtf8(u8"Tên Môn Học"),
        QString::fromUtf8(u8"Tín Chỉ"),
        QString::fromUtf8(u8"Điểm Số")
    });
    tableTranscript->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableTranscript->setEditTriggers(QAbstractItemView::NoEditTriggers);
    transLayout->addWidget(tableTranscript);
    bodyLayout->addWidget(boxTranscript, 1);

    // Section 2: Đăng ký môn học
    QGroupBox* boxRegister = new QGroupBox(QString::fromUtf8(u8"📝 Đăng Ký Môn Học Mới"), this);
    QVBoxLayout* regLayout = new QVBoxLayout(boxRegister);
    tableRegister = new QTableWidget(this);
    tableRegister->setColumnCount(4);
    tableRegister->setHorizontalHeaderLabels({
        QString::fromUtf8(u8"Mã Môn"),
        QString::fromUtf8(u8"Tên Môn Học"),
        QString::fromUtf8(u8"Tín Chỉ"),
        QString::fromUtf8(u8"Thao Tác")
    });
    tableRegister->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableRegister->setEditTriggers(QAbstractItemView::NoEditTriggers);
    regLayout->addWidget(tableRegister);
    bodyLayout->addWidget(boxRegister, 1);

    rootLayout->addLayout(bodyLayout);
}

void StudentWindow::applyStylesheet() {
    QString qss = R"(
        QMainWindow, QWidget {
            background-color: #0f172a;
            color: #f8fafc;
            font-family: 'Segoe UI', Arial, sans-serif;
            font-size: 13px;
        }
        #HeaderCard {
            background-color: #1e293b;
            border: 1px solid #334155;
            border-radius: 8px;
            padding: 10px;
        }
        #LblAvatar {
            background-color: #0284c7;
            color: white;
            font-size: 20px;
            font-weight: bold;
            border-radius: 25px;
        }
        #LblName {
            font-size: 18px;
            font-weight: bold;
            color: #38bdf8;
        }
        #LblInfo {
            color: #94a3b8;
        }
        QGroupBox {
            font-weight: bold;
            color: #38bdf8;
            border: 1px solid #334155;
            border-radius: 8px;
            margin-top: 10px;
            padding-top: 15px;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 5px;
        }
        QTableWidget {
            background-color: #1e293b;
            border: 1px solid #334155;
            gridline-color: #334155;
            color: #f8fafc;
            border-radius: 6px;
        }
        QHeaderView::section {
            background-color: #334155;
            color: #38bdf8;
            padding: 6px;
            font-weight: bold;
            border: none;
        }
        QPushButton {
            background-color: #2563eb;
            color: white;
            font-weight: bold;
            border: none;
            border-radius: 6px;
            padding: 8px 12px;
        }
        QPushButton:hover {
            background-color: #1d4ed8;
        }
        #BtnLogout {
            background-color: #ef4444;
        }
        #BtnLogout:hover {
            background-color: #dc2626;
        }
        #BtnChangePass {
            background-color: #f59e0b;
        }
        #BtnChangePass:hover {
            background-color: #d97706;
        }
    )";
    this->setStyleSheet(qss);
}

void StudentWindow::loadStudentData() {
    refreshStudentData();

    QString firstLetter = currentStudent.getFullName().isEmpty() ? "S" : QString(currentStudent.getFullName().at(0)).toUpper();
    if (lblAvatar) lblAvatar->setText(firstLetter);
    if (lblName) lblName->setText(currentStudent.getFullName());
    if (lblInfo) {
        lblInfo->setText(QString::fromUtf8(u8"MSSV: %1 | Lớp: %2 | GPA: %3")
                             .arg(currentStudent.getMssv())
                             .arg(currentStudent.getClassName())
                             .arg(QString::number(currentStudent.calculateGPA(), 'f', 2)));
    }

    loadTranscriptTable();
    loadCourseRegistrationTable();
}

void StudentWindow::loadTranscriptTable() {
    if (!tableTranscript) return;
    tableTranscript->setRowCount(0);

    auto courses = currentStudent.getCourses();
    for (const auto& c : courses) {
        int row = tableTranscript->rowCount();
        tableTranscript->insertRow(row);

        tableTranscript->setItem(row, 0, new QTableWidgetItem(c.courseCode));
        tableTranscript->setItem(row, 1, new QTableWidgetItem(c.courseName));
        tableTranscript->setItem(row, 2, new QTableWidgetItem(QString::number(c.credits)));
        tableTranscript->setItem(row, 3, new QTableWidgetItem(QString::number(c.score, 'f', 1)));
    }
}

void StudentWindow::loadCourseRegistrationTable() {
    if (!tableRegister) return;
    tableRegister->setRowCount(0);

    auto availableCourses = DatabaseManager::getInstance()->getAvailableCourses();
    auto studentCourses = currentStudent.getCourses();

    for (const auto& ac : availableCourses) {
        if (ac.targetClass != QString::fromUtf8(u8"Tất cả") && ac.targetClass != currentStudent.getClassName()) {
            continue;
        }

        bool isRegistered = false;
        for (const auto& sc : studentCourses) {
            if (sc.courseCode == ac.courseCode) {
                isRegistered = true;
                break;
            }
        }

        int row = tableRegister->rowCount();
        tableRegister->insertRow(row);

        tableRegister->setItem(row, 0, new QTableWidgetItem(ac.courseCode));
        tableRegister->setItem(row, 1, new QTableWidgetItem(ac.courseName));
        tableRegister->setItem(row, 2, new QTableWidgetItem(QString::number(ac.credits)));

        if (isRegistered) {
            QPushButton* btnUnreg = new QPushButton(QString::fromUtf8(u8"Hủy Đăng Ký"));
            btnUnreg->setStyleSheet("background-color: #ef4444; color: white; border-radius: 4px; padding: 4px 8px;");
            connect(btnUnreg, &QPushButton::clicked, this, [this, ac]() {
                handleUnregisterCourse(ac.courseCode);
            });
            tableRegister->setCellWidget(row, 3, btnUnreg);
        } else {
            QPushButton* btnReg = new QPushButton(QString::fromUtf8(u8"Đăng Ký"));
            btnReg->setStyleSheet("background-color: #10b981; color: white; border-radius: 4px; padding: 4px 8px;");
            connect(btnReg, &QPushButton::clicked, this, [this, ac]() {
                handleRegisterCourse(ac);
            });
            tableRegister->setCellWidget(row, 3, btnReg);
        }
    }
}

void StudentWindow::handleRegisterCourse(const Course& course) {
    currentStudent.addCourse(course);
    DatabaseManager::getInstance()->addOrUpdateStudent(currentStudent);
    QMessageBox::information(this, QString::fromUtf8(u8"Thành công"),
                             QString::fromUtf8(u8"Đã đăng ký môn học: %1").arg(course.courseName));
    loadStudentData();
}

void StudentWindow::handleUnregisterCourse(const QString& courseCode) {
    auto courses = currentStudent.getCourses();
    QVector<Course> updatedCourses;
    for (const auto& c : courses) {
        if (c.courseCode != courseCode) {
            updatedCourses.append(c);
        }
    }
    currentStudent.setCourses(updatedCourses);
    DatabaseManager::getInstance()->addOrUpdateStudent(currentStudent);
    QMessageBox::information(this, QString::fromUtf8(u8"Thành công"),
                             QString::fromUtf8(u8"Đã hủy đăng ký môn học!"));
    loadStudentData();
}

void StudentWindow::handleLogout() {
    LoginWindow* loginWin = new LoginWindow();
    loginWin->show();
    this->close();
}

void StudentWindow::handleChangePassword() {
    ChangePasswordDialog dialog(currentMssv, this);
    dialog.exec();
}