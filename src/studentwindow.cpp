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
#include <QPushButton>
#include <QLabel>
#include <QTableWidget>
#include <QFrame>
#include <QDateTime>
#include <QTabWidget>

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
    resize(1050, 680);

    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QVBoxLayout* rootLayout = new QVBoxLayout(centralWidget);
    rootLayout->setContentsMargins(15, 15, 15, 15);
    rootLayout->setSpacing(12);

    // --- 1. HEADER BAR ---
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

    // --- 2. THANH CẢNH BÁO GPA THẤP ---
    lblGpaWarning = new QLabel(this);
    lblGpaWarning->setObjectName("LblGpaWarning");
    lblGpaWarning->setWordWrap(true);
    lblGpaWarning->setVisible(false);
    rootLayout->addWidget(lblGpaWarning);

    // --- 3. MAIN CONTENT (TÁCH 2 TAB RIÊNG BIỆT) ---
    QTabWidget* mainTabWidget = new QTabWidget(this);
    mainTabWidget->setObjectName("MainTabWidget");

    // TAB 1: BẢNG ĐIỂM CÁ NHÂN
    QWidget* tabTranscript = new QWidget(this);
    QVBoxLayout* layoutTranscriptTab = new QVBoxLayout(tabTranscript);
    layoutTranscriptTab->setContentsMargins(10, 10, 10, 10);

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
    tableTranscript->setSelectionBehavior(QAbstractItemView::SelectRows);
    layoutTranscriptTab->addWidget(tableTranscript);

    mainTabWidget->addTab(tabTranscript, QString::fromUtf8(u8"📊 Bảng Điểm Cá Nhân"));

    // TAB 2: ĐĂNG KÝ MÔN HỌC
    QWidget* tabRegister = new QWidget(this);
    QVBoxLayout* layoutRegisterTab = new QVBoxLayout(tabRegister);
    layoutRegisterTab->setContentsMargins(10, 10, 10, 10);

    tableRegister = new QTableWidget(this);
    tableRegister->setColumnCount(5);
    tableRegister->setHorizontalHeaderLabels({
        QString::fromUtf8(u8"Mã Môn"),
        QString::fromUtf8(u8"Tên Môn Học"),
        QString::fromUtf8(u8"Tín Chỉ"),
        QString::fromUtf8(u8"Sĩ Số"),
        QString::fromUtf8(u8"Thao Tác")
    });
    tableRegister->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableRegister->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableRegister->setSelectionBehavior(QAbstractItemView::SelectRows);
    layoutRegisterTab->addWidget(tableRegister);

    mainTabWidget->addTab(tabRegister, QString::fromUtf8(u8"📝 Đăng Ký Môn Học"));

    rootLayout->addWidget(mainTabWidget);
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
        #LblGpaWarning {
            background-color: #450a0a;
            color: #fca5a5;
            border: 1px solid #ef4444;
            border-radius: 6px;
            padding: 8px 12px;
            font-weight: bold;
        }
        QTabWidget::pane {
            border: 1px solid #334155;
            background-color: #1e293b;
            border-radius: 8px;
        }
        QTabBar::tab {
            background-color: #1e293b;
            color: #94a3b8;
            padding: 8px 16px;
            border: 1px solid #334155;
            border-bottom: none;
            border-top-left-radius: 6px;
            border-top-right-radius: 6px;
            font-weight: bold;
            margin-right: 4px;
        }
        QTabBar::tab:selected {
            background-color: #0284c7;
            color: #ffffff;
            border-color: #0284c7;
        }
        QTabBar::tab:hover:!selected {
            background-color: #334155;
            color: #f8fafc;
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
            padding: 6px 12px;
        }
        QPushButton:hover {
            background-color: #1d4ed8;
        }
        QPushButton:disabled {
            background-color: #475569;
            color: #94a3b8;
        }
        #BtnLogout { background-color: #ef4444; }
        #BtnLogout:hover { background-color: #dc2626; }
        #BtnChangePass { background-color: #f59e0b; }
        #BtnChangePass:hover { background-color: #d97706; }
    )";
    this->setStyleSheet(qss);
}

void StudentWindow::loadStudentData() {
    refreshStudentData();

    QString firstLetter = currentStudent.getFullName().isEmpty() ? "S" : QString(currentStudent.getFullName().at(0)).toUpper();
    if (lblAvatar) lblAvatar->setText(firstLetter);
    if (lblName) lblName->setText(currentStudent.getFullName());

    double gpa = currentStudent.calculateGPA();
    int currentCredits = currentStudent.getTotalRegisteredCredits();
    int maxCredits = currentStudent.getMaxAllowedCredits();

    if (lblGpaWarning) {
        if (currentStudent.isAcademicWarning()) {
            lblGpaWarning->setText(QString::fromUtf8(u8"⚠️ CẢNH BÁO HỌC TẬP: Điểm GPA của bạn hiện tại là %1 (< 2.0). "
                                                     u8"Bạn bị giới hạn đăng ký tối đa 14 tín chỉ trong học kỳ này!")
                                       .arg(QString::number(gpa, 'f', 2)));
            lblGpaWarning->setVisible(true);
        } else {
            lblGpaWarning->setVisible(false);
        }
    }

    if (lblInfo) {
        QString warningTag = currentStudent.isAcademicWarning() ? QString::fromUtf8(u8" ⚠️ [Cảnh báo]") : "";
        lblInfo->setText(QString::fromUtf8(u8"MSSV: %1 | Lớp: %2 | GPA: %3%4 | Tín chỉ: %5/%6")
                             .arg(currentStudent.getMssv())
                             .arg(currentStudent.getClassName())
                             .arg(QString::number(gpa, 'f', 2))
                             .arg(warningTag)
                             .arg(currentCredits)
                             .arg(maxCredits));
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

        QString courseCodeStr = !c.getCode().isEmpty() ? c.getCode() : c.courseCode;
        double calculatedScore = (c.midtermScore * 0.3) + (c.finalScore * 0.7);
        bool hasScore = (c.midtermScore > 0.0 || c.finalScore > 0.0 || c.score > 0.0);

        tableTranscript->setItem(row, 0, new QTableWidgetItem(courseCodeStr));
        tableTranscript->setItem(row, 1, new QTableWidgetItem(c.courseName));
        tableTranscript->setItem(row, 2, new QTableWidgetItem(QString::number(c.credits)));

        if (!hasScore) {
            tableTranscript->setItem(row, 3, new QTableWidgetItem(QString::fromUtf8(u8"Chưa có")));
        } else {
            double finalVal = (calculatedScore > 0.0) ? calculatedScore : c.score;
            tableTranscript->setItem(row, 3, new QTableWidgetItem(QString::number(finalVal, 'f', 1)));
        }
    }
}

void StudentWindow::loadCourseRegistrationTable() {
    if (!tableRegister) return;
    tableRegister->setRowCount(0);

    bool isOpen = DatabaseManager::getInstance()->isRegistrationOpen();
    auto availableCourses = DatabaseManager::getInstance()->getAvailableCourses();
    auto studentCourses = currentStudent.getCourses();

    for (const auto& ac : availableCourses) {
        QString acCode = !ac.getCode().isEmpty() ? ac.getCode() : ac.courseCode;

        if (ac.targetClass != QString::fromUtf8(u8"Tất cả") && ac.targetClass != currentStudent.getClassName()) {
            continue;
        }

        bool isRegistered = false;
        bool hasScore = false;

        for (const auto& sc : studentCourses) {
            QString scCode = !sc.getCode().isEmpty() ? sc.getCode() : sc.courseCode;
            if (scCode == acCode && !acCode.isEmpty()) {
                isRegistered = true;
                if (sc.midtermScore > 0.0 || sc.finalScore > 0.0 || sc.score > 0.0) {
                    hasScore = true;
                }
                break;
            }
        }

        int row = tableRegister->rowCount();
        tableRegister->insertRow(row);

        tableRegister->setItem(row, 0, new QTableWidgetItem(acCode));
        tableRegister->setItem(row, 1, new QTableWidgetItem(ac.courseName));
        tableRegister->setItem(row, 2, new QTableWidgetItem(QString::number(ac.credits)));
        tableRegister->setItem(row, 3, new QTableWidgetItem(QString("%1/%2").arg(ac.currentEnrolled).arg(ac.maxCapacity)));

        if (isRegistered) {
            QPushButton* btnUnreg = new QPushButton(QString::fromUtf8(u8"Hủy Đăng Ký"));
            if (!isOpen) {
                btnUnreg->setText(QString::fromUtf8(u8"Hết hạn"));
                btnUnreg->setEnabled(false);
            } else if (hasScore) {
                btnUnreg->setText(QString::fromUtf8(u8"Đã có điểm"));
                btnUnreg->setEnabled(false);
                btnUnreg->setToolTip(QString::fromUtf8(u8"Học phần đã có điểm số, không thể hủy đăng ký."));
            } else {
                btnUnreg->setStyleSheet("background-color: #ef4444; color: white; border-radius: 4px; padding: 4px 8px;");
                connect(btnUnreg, &QPushButton::clicked, this, [this, acCode]() {
                    handleUnregisterCourse(acCode);
                });
            }
            tableRegister->setCellWidget(row, 4, btnUnreg);
        } else {
            QPushButton* btnReg = new QPushButton(QString::fromUtf8(u8"Đăng Ký"));
            if (!isOpen) {
                btnReg->setText(QString::fromUtf8(u8"Chưa mở/Hết hạn"));
                btnReg->setEnabled(false);
            } else if (ac.currentEnrolled >= ac.maxCapacity) {
                btnReg->setText(QString::fromUtf8(u8"Đã đầy"));
                btnReg->setEnabled(false);
            } else {
                btnReg->setStyleSheet("background-color: #10b981; color: white; border-radius: 4px; padding: 4px 8px;");
                connect(btnReg, &QPushButton::clicked, this, [this, ac]() {
                    handleRegisterCourse(ac);
                });
            }
            tableRegister->setCellWidget(row, 4, btnReg);
        }
    }
}

void StudentWindow::handleRegisterCourse(const Course& course) {
    if (!DatabaseManager::getInstance()->isRegistrationOpen()) {
        QDateTime start = DatabaseManager::getInstance()->getRegStartTime();
        QDateTime end = DatabaseManager::getInstance()->getRegEndTime();
        QString msg = QString::fromUtf8(u8"Hiện tại không phải thời gian đăng ký học phần!\nThời gian mở: %1 đến %2")
                          .arg(start.toString("dd/MM/yyyy hh:mm"))
                          .arg(end.toString("dd/MM/yyyy hh:mm"));
        QMessageBox::warning(this, QString::fromUtf8(u8"Thông báo"), msg);
        return;
    }

    QString targetCode = !course.getCode().isEmpty() ? course.getCode() : course.courseCode;

    for (const Course& c : currentStudent.getCourses()) {
        QString cCode = !c.getCode().isEmpty() ? c.getCode() : c.courseCode;
        if (cCode == targetCode && !targetCode.isEmpty()) {
            QMessageBox::warning(this, QString::fromUtf8(u8"Lỗi đăng ký"),
                                 QString::fromUtf8(u8"Bạn đã đăng ký học phần [%1] này rồi!").arg(course.courseName));
            return;
        }
    }

    auto availList = DatabaseManager::getInstance()->getAvailableCourses();
    Course targetCourse = course;
    for (const auto& ac : availList) {
        QString acCode = !ac.getCode().isEmpty() ? ac.getCode() : ac.courseCode;
        if (acCode == targetCode) {
            targetCourse = ac;
            break;
        }
    }

    if (targetCourse.currentEnrolled >= targetCourse.maxCapacity) {
        QMessageBox::warning(this, QString::fromUtf8(u8"Lỗi đăng ký"),
                             QString::fromUtf8(u8"Môn học [%1] đã đầy sĩ số (%2/%3)!")
                                 .arg(targetCourse.courseName)
                                 .arg(targetCourse.currentEnrolled)
                                 .arg(targetCourse.maxCapacity));
        loadStudentData();
        return;
    }

    int currentCredits = currentStudent.getTotalRegisteredCredits();
    int maxCredits = currentStudent.getMaxAllowedCredits();

    if (currentCredits + targetCourse.credits > maxCredits) {
        QString errorMsg = QString::fromUtf8(u8"Không thể đăng ký! Tổng số tín chỉ sau khi đăng ký (%1) vượt quá giới hạn cho phép (%2 tín chỉ).\n\n")
                               .arg(currentCredits + targetCourse.credits)
                               .arg(maxCredits);

        if (currentStudent.isAcademicWarning()) {
            errorMsg += QString::fromUtf8(u8"⚠️ Bạn đang bị Cảnh báo học tập (GPA < 2.0) nên bị hạ giới hạn đăng ký xuống 14 tín chỉ.");
        }

        QMessageBox::warning(this, QString::fromUtf8(u8"Vượt giới hạn tín chỉ"), errorMsg);
        return;
    }

    currentStudent.addCourse(targetCourse);
    DatabaseManager::getInstance()->addOrUpdateStudent(currentStudent);

    QMessageBox::information(this, QString::fromUtf8(u8"Thành công"),
                             QString::fromUtf8(u8"Đã đăng ký thành công môn học: %1").arg(targetCourse.courseName));

    loadStudentData();
}

void StudentWindow::handleUnregisterCourse(const QString& courseCode) {
    if (!DatabaseManager::getInstance()->isRegistrationOpen()) {
        QDateTime start = DatabaseManager::getInstance()->getRegStartTime();
        QDateTime end = DatabaseManager::getInstance()->getRegEndTime();
        QString msg = QString::fromUtf8(u8"Đã hết thời hạn điều chỉnh / hủy đăng ký học phần!\nThời gian mở: %1 đến %2")
                          .arg(start.toString("dd/MM/yyyy hh:mm"))
                          .arg(end.toString("dd/MM/yyyy hh:mm"));
        QMessageBox::warning(this, QString::fromUtf8(u8"Thông báo"), msg);
        return;
    }

    auto courses = currentStudent.getCourses();
    QVector<Course> updatedCourses;
    bool found = false;

    for (const auto& c : courses) {
        QString cCode = !c.getCode().isEmpty() ? c.getCode() : c.courseCode;
        if (cCode == courseCode) {
            found = true;
            bool hasScore = (c.midtermScore > 0.0 || c.finalScore > 0.0 || c.score > 0.0);
            if (hasScore) {
                QMessageBox::warning(this, QString::fromUtf8(u8"Không thể hủy đăng ký"),
                                     QString::fromUtf8(u8"Học phần [%1] đã có điểm số, không thể hủy đăng ký!").arg(c.courseName));
                return;
            }
        } else {
            updatedCourses.append(c);
        }
    }

    if (!found) {
        QMessageBox::warning(this, QString::fromUtf8(u8"Lỗi"),
                             QString::fromUtf8(u8"Không tìm thấy môn học cần hủy!"));
        return;
    }

    currentStudent.setCourses(updatedCourses);
    DatabaseManager::getInstance()->addOrUpdateStudent(currentStudent);

    QMessageBox::information(this, QString::fromUtf8(u8"Thành công"),
                             QString::fromUtf8(u8"Đã hủy đăng ký môn học thành công!"));

    loadStudentData();
}

void StudentWindow::handleLogout() {
    LoginWindow* loginWin = new LoginWindow();
    loginWin->show();
    this->close();
}

void StudentWindow::handleChangePassword() {
    ChangePasswordDialog dialog(currentMssv, this);
    if (dialog.exec() == QDialog::Accepted) {
        refreshStudentData();
    }
}