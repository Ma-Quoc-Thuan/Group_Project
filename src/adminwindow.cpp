#if defined(_MSC_VER) && (_MSC_VER >= 1600)
#pragma execution_character_set("utf-8")
#endif

#include "adminwindow.h"
#include "editstudentdialog.h"
#include "loginwindow.h"
#include "gradedialog.h"
#include "databasemanager.h"

#include <algorithm>
#include <QSet>
#include <QMap>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QFile>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QHeaderView>
#include <QDialog>
#include <QFormLayout>
#include <QSpinBox>
#include <QComboBox>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QFrame>
#include <QListWidget>

AdminWindow::AdminWindow(QWidget *parent) : QMainWindow(parent) {
    setupUI();
    applyStylesheet();

    // Kết nối tín hiệu (Connections)
    connect(txtSearch, &QLineEdit::textChanged, this, &AdminWindow::onSearchTextChanged);
    connect(btnAddCourse, &QPushButton::clicked, this, &AdminWindow::addCourse);
    connect(btnImportFile, &QPushButton::clicked, this, &AdminWindow::importFromFile);

    // Tín hiệu bảng của Tab 1 (Tất cả sinh viên)
    connect(tableWidget, &QTableWidget::cellDoubleClicked, this, &AdminWindow::onCellDoubleClicked);

    // Tín hiệu danh sách Lớp học (Tab 2)
    connect(listClasses, &QListWidget::itemClicked, this, &AdminWindow::onClassSelected);
    connect(btnLogout, &QPushButton::clicked, this, &AdminWindow::handleLogout);

    loadClassesList();
    loadDataByFilter();
    updateDashboardStats();
}

void AdminWindow::setupUI() {
    setWindowTitle(QString::fromUtf8(u8"SIMS PRO - Cổng Quản Lý Giáo Viên"));
    resize(1200, 700);

    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QVBoxLayout* rootLayout = new QVBoxLayout(centralWidget);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    // --- 1. HEADER BAR THANH TRÊN ---
    QFrame* headerBar = new QFrame(this);
    headerBar->setObjectName("HeaderBar");
    QHBoxLayout* headerLayout = new QHBoxLayout(headerBar);
    headerLayout->setContentsMargins(20, 10, 20, 10);

    QLabel* lblLogo = new QLabel(QString::fromUtf8(u8"🎓 SIMS PRO"), this);
    lblLogo->setObjectName("LogoLabel");

    QLabel* lblTitle = new QLabel(QString::fromUtf8(u8"HỆ THỐNG QUẢN LÝ SINH VIÊN & ĐÀO TẠO"), this);
    lblTitle->setObjectName("HeaderTitle");

    QLabel* lblUser = new QLabel(QString::fromUtf8(u8"👤 Giáo Viên: Admin"), this);
    lblUser->setObjectName("UserLabel");

    btnLogout = new QPushButton(QString::fromUtf8(u8"Đăng xuất"), this);
    btnLogout->setObjectName("BtnLogout");

    headerLayout->addWidget(lblLogo);
    headerLayout->addWidget(lblTitle);
    headerLayout->addStretch();
    headerLayout->addWidget(lblUser);
    headerLayout->addWidget(btnLogout);

    rootLayout->addWidget(headerBar);

    // --- 2. THÂN CHÍNH (Sidebar + TabWidget) ---
    QWidget* bodyWidget = new QWidget(this);
    QHBoxLayout* bodyLayout = new QHBoxLayout(bodyWidget);
    bodyLayout->setContentsMargins(10, 10, 10, 10);
    bodyLayout->setSpacing(10);

    // --- SIDEBAR TRÁI (Đã thu gọn, bỏ Tree phân loại) ---
    QFrame* sidebarFrame = new QFrame(this);
    sidebarFrame->setObjectName("SidebarFrame");
    sidebarFrame->setFixedWidth(240);
    QVBoxLayout* sidebar = new QVBoxLayout(sidebarFrame);

    QLabel* lblSidebarTitle = new QLabel(QString::fromUtf8(u8"CÔNG CỤ NHANH"), this);
    lblSidebarTitle->setObjectName("SubHeader");
    sidebar->addWidget(lblSidebarTitle);

    txtSearch = new QLineEdit(this);
    txtSearch->setPlaceholderText(QString::fromUtf8(u8"🔍 Tìm MSSV hoặc Tên..."));
    txtSearch->setObjectName("TxtSearch");
    sidebar->addWidget(txtSearch);

    btnAddCourse = new QPushButton(QString::fromUtf8(u8"➕ Tạo Học Phần (Mở Đăng Ký)"), this);
    btnAddCourse->setObjectName("BtnAddCourse");
    sidebar->addWidget(btnAddCourse);

    btnImportFile = new QPushButton(QString::fromUtf8(u8"📂 Nhập File (CSV)"), this);
    btnImportFile->setObjectName("BtnImportFile");
    sidebar->addWidget(btnImportFile);

    sidebar->addStretch();

    bodyLayout->addWidget(sidebarFrame);

    // --- MAIN TAB WIDGET ---
    mainTabWidget = new QTabWidget(this);
    mainTabWidget->setObjectName("MainTabWidget");

    // ==========================================
    // TAB 1: TẤT CẢ SINH VIÊN
    // ==========================================
    QWidget* tabStudent = new QWidget();
    QHBoxLayout* tab1Layout = new QHBoxLayout(tabStudent);

    // Form thêm sinh viên
    QGroupBox* boxAddStudent = new QGroupBox(QString::fromUtf8(u8"Thêm Sinh Viên Mới"), tabStudent);
    boxAddStudent->setFixedWidth(280);
    QVBoxLayout* formLayout = new QVBoxLayout(boxAddStudent);

    txtMssv = new QLineEdit(); txtMssv->setPlaceholderText(QString::fromUtf8(u8"Mã số sinh viên (MSSV)"));
    txtName = new QLineEdit(); txtName->setPlaceholderText(QString::fromUtf8(u8"Họ và tên sinh viên"));
    txtDob = new QLineEdit(); txtDob->setPlaceholderText(QString::fromUtf8(u8"Ngày sinh (DD/MM/YYYY)"));
    txtHometown = new QLineEdit(); txtHometown->setPlaceholderText(QString::fromUtf8(u8"Quê quán"));
    txtClass = new QLineEdit(); txtClass->setPlaceholderText(QString::fromUtf8(u8"Lớp (VD: CNTT01)"));

    formLayout->addWidget(new QLabel(QString::fromUtf8(u8"MSSV:")));
    formLayout->addWidget(txtMssv);
    formLayout->addWidget(new QLabel(QString::fromUtf8(u8"Họ và Tên:")));
    formLayout->addWidget(txtName);
    formLayout->addWidget(new QLabel(QString::fromUtf8(u8"Ngày Sinh:")));
    formLayout->addWidget(txtDob);
    formLayout->addWidget(new QLabel(QString::fromUtf8(u8"Quê Quán:")));
    formLayout->addWidget(txtHometown);
    formLayout->addWidget(new QLabel(QString::fromUtf8(u8"Lớp Học:")));
    formLayout->addWidget(txtClass);

    QPushButton* btnAddStudent = new QPushButton(QString::fromUtf8(u8"➕ Thêm Sinh Viên"), boxAddStudent);
    btnAddStudent->setObjectName("BtnAddStudent");
    connect(btnAddStudent, &QPushButton::clicked, this, &AdminWindow::addStudent);

    formLayout->addWidget(btnAddStudent);
    formLayout->addStretch();
    tab1Layout->addWidget(boxAddStudent);

    // Bảng danh sách sinh viên
    QGroupBox* boxTable = new QGroupBox(QString::fromUtf8(u8"Danh Sách Toàn Bộ Sinh Viên"), tabStudent);
    QVBoxLayout* boxTableLayout = new QVBoxLayout(boxTable);

    tableWidget = new QTableWidget();
    tableWidget->setColumnCount(7);
    tableWidget->setHorizontalHeaderLabels({
        "MSSV", QString::fromUtf8(u8"Họ và Tên"), QString::fromUtf8(u8"Ngày Sinh"),
        QString::fromUtf8(u8"Quê Quán"), QString::fromUtf8(u8"Lớp"), "GPA", QString::fromUtf8(u8"Thao Tác")
    });
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

    boxTableLayout->addWidget(tableWidget);
    tab1Layout->addWidget(boxTable);

    mainTabWidget->addTab(tabStudent, QString::fromUtf8(u8"📋 Danh Sách Tổng"));

    // ==========================================
    // TAB 2: QUẢN LÝ LỚP HỌC (TAB MỚI)
    // ==========================================
    QWidget* tabClass = new QWidget();
    QHBoxLayout* tabClassLayout = new QHBoxLayout(tabClass);

    QGroupBox* boxClassList = new QGroupBox(QString::fromUtf8(u8"Danh Sách Các Lớp"), tabClass);
    boxClassList->setFixedWidth(200);
    QVBoxLayout* classListLayout = new QVBoxLayout(boxClassList);

    listClasses = new QListWidget();
    listClasses->setObjectName("ListClasses");
    classListLayout->addWidget(listClasses);
    tabClassLayout->addWidget(boxClassList);

    QGroupBox* boxClassStudents = new QGroupBox(QString::fromUtf8(u8"Sinh Viên Thuộc Lớp (Chọn lớp bên trái)"), tabClass);
    QVBoxLayout* classStudentsLayout = new QVBoxLayout(boxClassStudents);

    tableClassStudents = new QTableWidget();
    tableClassStudents->setColumnCount(7);
    tableClassStudents->setHorizontalHeaderLabels({
        "MSSV", QString::fromUtf8(u8"Họ và Tên"), QString::fromUtf8(u8"Ngày Sinh"),
        QString::fromUtf8(u8"Quê Quán"), QString::fromUtf8(u8"Lớp"), "GPA", QString::fromUtf8(u8"Thao Tác")
    });
    tableClassStudents->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableClassStudents->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableClassStudents->setSelectionBehavior(QAbstractItemView::SelectRows);
    classStudentsLayout->addWidget(tableClassStudents);
    tabClassLayout->addWidget(boxClassStudents);

    // Tín hiệu click đúp sinh viên trong Tab Lớp
    connect(tableClassStudents, &QTableWidget::cellDoubleClicked, this, [this](int row, int column){
        Q_UNUSED(column);
        QTableWidgetItem* mssvItem = tableClassStudents->item(row, 0);
        if (mssvItem) {
            QString mssv = mssvItem->text();
            GradeDialog dialog(mssv, this);
            dialog.exec();

            loadClassesList();
            loadDataByFilter();
            updateDashboardStats();
            if(listClasses->currentItem()) onClassSelected(listClasses->currentItem());
        }
    });

    mainTabWidget->addTab(tabClass, QString::fromUtf8(u8"🏫 Quản Lý Lớp"));

    // ==========================================
    // TAB 3: THỜI KHÓA BIỂU
    // ==========================================
    QWidget* tabSchedule = new QWidget();
    QHBoxLayout* tab2Layout = new QHBoxLayout(tabSchedule);

    QGroupBox* boxScheduleForm = new QGroupBox(QString::fromUtf8(u8"Xếp Lịch Học"), tabSchedule);
    boxScheduleForm->setFixedWidth(300);
    QFormLayout* schedForm = new QFormLayout(boxScheduleForm);

    cbScheduleClass = new QComboBox();
    cbScheduleCourse = new QComboBox();
    cbDayOfWeek = new QComboBox();
    cbDayOfWeek->addItems({
        QString::fromUtf8(u8"Thứ Hai"), QString::fromUtf8(u8"Thứ Ba"), QString::fromUtf8(u8"Thứ Tư"),
        QString::fromUtf8(u8"Thứ Năm"), QString::fromUtf8(u8"Thứ Sáu"), QString::fromUtf8(u8"Thứ Bảy")
    });

    txtTimeSlot = new QLineEdit(); txtTimeSlot->setPlaceholderText(QString::fromUtf8(u8"VD: Tiết 1-3 (07:00-09:30)"));
    txtRoom = new QLineEdit(); txtRoom->setPlaceholderText(QString::fromUtf8(u8"VD: A2-301"));

    schedForm->addRow(QString::fromUtf8(u8"Chọn Lớp:"), cbScheduleClass);
    schedForm->addRow(QString::fromUtf8(u8"Chọn Môn:"), cbScheduleCourse);
    schedForm->addRow(QString::fromUtf8(u8"Thứ trong tuần:"), cbDayOfWeek);
    schedForm->addRow(QString::fromUtf8(u8"Tiết / Giờ:"), txtTimeSlot);
    schedForm->addRow(QString::fromUtf8(u8"Phòng Học:"), txtRoom);

    QPushButton* btnAddSchedule = new QPushButton(QString::fromUtf8(u8"📅 Thêm Lịch Học"));
    btnAddSchedule->setObjectName("BtnAddSchedule");
    connect(btnAddSchedule, &QPushButton::clicked, this, &AdminWindow::addClassSchedule);
    schedForm->addRow(btnAddSchedule);

    tab2Layout->addWidget(boxScheduleForm);

    QGroupBox* boxSchedTable = new QGroupBox(QString::fromUtf8(u8"Thời Khóa Biểu Các Lớp"), tabSchedule);
    QVBoxLayout* boxSchedTableLayout = new QVBoxLayout(boxSchedTable);

    tableSchedules = new QTableWidget();
    tableSchedules->setColumnCount(5);
    tableSchedules->setHorizontalHeaderLabels({
        QString::fromUtf8(u8"Lớp"), QString::fromUtf8(u8"Môn Học"), QString::fromUtf8(u8"Thứ"),
        QString::fromUtf8(u8"Thời Gian"), QString::fromUtf8(u8"Phòng Học")
    });
    tableSchedules->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableSchedules->setEditTriggers(QAbstractItemView::NoEditTriggers);

    boxSchedTableLayout->addWidget(tableSchedules);
    tab2Layout->addWidget(boxSchedTable);

    mainTabWidget->addTab(tabSchedule, QString::fromUtf8(u8"📅 Thời Khóa Biểu"));

    // ==========================================
    // TAB 4: THỐNG KÊ & BÁO CÁO
    // ==========================================
    QWidget* tabStats = new QWidget();
    QVBoxLayout* tab3Layout = new QVBoxLayout(tabStats);

    QHBoxLayout* cardsLayout = new QHBoxLayout();

    QFrame* card1 = new QFrame(); card1->setObjectName("StatCard");
    QVBoxLayout* c1 = new QVBoxLayout(card1);
    c1->addWidget(new QLabel(QString::fromUtf8(u8"TỔNG SỐ SINH VIÊN"), card1));
    lblTotalStudents = new QLabel("0", card1);
    lblTotalStudents->setStyleSheet("font-size: 28px; font-weight: bold; color: #3b82f6;");
    c1->addWidget(lblTotalStudents);
    cardsLayout->addWidget(card1);

    QFrame* card2 = new QFrame(); card2->setObjectName("StatCard");
    QVBoxLayout* c2 = new QVBoxLayout(card2);
    c2->addWidget(new QLabel(QString::fromUtf8(u8"GPA TRUNG BÌNH TOÀN TRƯỜNG"), card2));
    lblAverageGpa = new QLabel("0.00", card2);
    lblAverageGpa->setStyleSheet("font-size: 28px; font-weight: bold; color: #10b981;");
    c2->addWidget(lblAverageGpa);
    cardsLayout->addWidget(card2);

    QFrame* card3 = new QFrame(); card3->setObjectName("StatCard");
    QVBoxLayout* c3 = new QVBoxLayout(card3);
    c3->addWidget(new QLabel(QString::fromUtf8(u8"TỶ LỆ GIỎI / XUẤT SẮC"), card3));
    lblExcellentRatio = new QLabel("0.0%", card3);
    lblExcellentRatio->setStyleSheet("font-size: 28px; font-weight: bold; color: #f59e0b;");
    c3->addWidget(lblExcellentRatio);
    cardsLayout->addWidget(card3);

    tab3Layout->addLayout(cardsLayout);
    tab3Layout->addStretch();

    mainTabWidget->addTab(tabStats, QString::fromUtf8(u8"📊 Thống Kê & Báo Cáo"));

    bodyLayout->addWidget(mainTabWidget);
    rootLayout->addWidget(bodyWidget);
}

void AdminWindow::applyStylesheet() {
    QString qss = R"(
        QMainWindow, QWidget {
            background-color: #0f172a;
            color: #f8fafc;
            font-family: 'Segoe UI', Arial, sans-serif;
            font-size: 13px;
        }

        #HeaderBar {
            background-color: #1e293b;
            border-bottom: 2px solid #334155;
        }
        #LogoLabel {
            font-size: 20px;
            font-weight: bold;
            color: #38bdf8;
        }
        #HeaderTitle {
            font-size: 14px;
            font-weight: 600;
            color: #94a3b8;
            margin-left: 15px;
        }
        #UserLabel {
            color: #cbd5e1;
            font-weight: 500;
        }

        #SidebarFrame {
            background-color: #1e293b;
            border-radius: 8px;
            padding: 10px;
        }
        #SubHeader {
            font-weight: bold;
            color: #38bdf8;
            font-size: 12px;
            margin-bottom: 5px;
        }

        QLineEdit, QComboBox {
            background-color: #334155;
            border: 1px solid #475569;
            border-radius: 6px;
            padding: 6px 10px;
            color: #f8fafc;
        }
        QLineEdit:focus, QComboBox:focus {
            border: 1px solid #38bdf8;
        }

        QListWidget {
            background-color: #0f172a;
            border: 1px solid #334155;
            border-radius: 6px;
            color: #f8fafc;
            padding: 5px;
        }
        QListWidget::item {
            padding: 8px;
            border-radius: 4px;
            margin-bottom: 2px;
        }
        QListWidget::item:selected {
            background-color: #0284c7;
            color: white;
            font-weight: bold;
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

        #BtnLogout { background-color: #ef4444; }
        #BtnLogout:hover { background-color: #dc2626; }
        #BtnAddCourse { background-color: #10b981; }
        #BtnAddCourse:hover { background-color: #059669; }
        #BtnImportFile { background-color: #f59e0b; }
        #BtnImportFile:hover { background-color: #d97706; }

        #StatCard {
            background-color: #1e293b;
            border: 1px solid #334155;
            border-radius: 10px;
            padding: 15px;
        }

        QTabWidget::pane {
            border: 1px solid #334155;
            border-radius: 8px;
            background-color: #0f172a;
        }
        QTabBar::tab {
            background-color: #1e293b;
            color: #94a3b8;
            padding: 8px 16px;
            border-top-left-radius: 6px;
            border-top-right-radius: 6px;
            margin-right: 2px;
            font-weight: bold;
        }
        QTabBar::tab:selected {
            background-color: #0284c7;
            color: white;
        }
    )";
    this->setStyleSheet(qss);
}

QTableWidgetItem* AdminWindow::createReadOnlyItem(const QString& text) {
    QTableWidgetItem* item = new QTableWidgetItem(text);
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    return item;
}

void AdminWindow::onSearchTextChanged(const QString& text) {
    Q_UNUSED(text);
    loadDataByFilter();
}

void AdminWindow::addCourse() {
    QDialog dialog(this);
    dialog.setWindowTitle(QString::fromUtf8(u8"Thêm Môn Học Mở Đăng Ký"));
    dialog.setMinimumWidth(350);

    QFormLayout form(&dialog);

    QLineEdit* txtCourseCode = new QLineEdit(&dialog);
    txtCourseCode->setPlaceholderText(QString::fromUtf8(u8"VD: INT103"));
    form.addRow(QString::fromUtf8(u8"Mã Môn Học:"), txtCourseCode);

    QLineEdit* txtCourseName = new QLineEdit(&dialog);
    txtCourseName->setPlaceholderText(QString::fromUtf8(u8"VD: Lập trình Java"));
    form.addRow(QString::fromUtf8(u8"Tên Môn Học:"), txtCourseName);

    QSpinBox* spinCredits = new QSpinBox(&dialog);
    spinCredits->setRange(1, 10);
    spinCredits->setValue(3);
    form.addRow(QString::fromUtf8(u8"Số Tín Chỉ:"), spinCredits);

    QComboBox* cbClass = new QComboBox(&dialog);
    cbClass->addItem(QString::fromUtf8(u8"Tất cả"));
    auto students = DatabaseManager::getInstance()->getAllStudents();
    QSet<QString> classSet;
    for (const auto& s : students) {
        if (!s.getClassName().isEmpty()) classSet.insert(s.getClassName());
    }
    for (const QString& cls : classSet) {
        cbClass->addItem(cls);
    }
    form.addRow(QString::fromUtf8(u8"Áp dụng cho Lớp:"), cbClass);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);

    connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        QString code = txtCourseCode->text().trimmed().toUpper();
        QString name = txtCourseName->text().trimmed();
        int credits = spinCredits->value();
        QString targetClass = cbClass->currentText();

        if (code.isEmpty() || name.isEmpty()) {
            QMessageBox::warning(this, QString::fromUtf8(u8"Cảnh báo"), QString::fromUtf8(u8"Vui lòng nhập đầy đủ mã và tên môn học!"));
            return;
        }

        Course newCourse;
        newCourse.courseCode = code;
        newCourse.courseName = name;
        newCourse.credits = credits;
        newCourse.score = 0.0;
        newCourse.targetClass = targetClass;

        DatabaseManager::getInstance()->addAvailableCourse(newCourse);

        QMessageBox::information(this, QString::fromUtf8(u8"Thành công"),
                                 QString::fromUtf8(u8"Đã mở đăng ký môn '%1' (%2) cho lớp [%3]!")
                                     .arg(name).arg(code).arg(targetClass));

        loadClassesList();
    }
}

void AdminWindow::loadClassesList() {
    if(listClasses) listClasses->clear();
    if (cbScheduleClass) cbScheduleClass->clear();
    if (cbScheduleCourse) cbScheduleCourse->clear();

    auto students = DatabaseManager::getInstance()->getAllStudents();
    QSet<QString> classes;
    QSet<QString> allCourses;

    for (const auto& s : students) {
        if (!s.getClassName().isEmpty()) classes.insert(s.getClassName());
        for (const auto& c : s.getCourses()) {
            if (!c.courseName.isEmpty()) allCourses.insert(c.courseName);
        }
    }

    auto availCourses = DatabaseManager::getInstance()->getAvailableCourses();
    for (const auto& ac : availCourses) {
        if (!ac.courseName.isEmpty()) allCourses.insert(ac.courseName);
    }

    // Load vào ListWidget
    for (const QString& cls : classes) {
        listClasses->addItem(cls);
        if (cbScheduleClass) cbScheduleClass->addItem(cls);
    }

    // Load vào ComboBox Thời khóa biểu
    if (cbScheduleCourse) {
        for (const QString& crs : allCourses) {
            cbScheduleCourse->addItem(crs);
        }
    }
}

// Bấm vào lớp học bên trái thì load sinh viên vào bảng bên phải
void AdminWindow::onClassSelected(QListWidgetItem* item) {
    if (!item || !tableClassStudents) return;
    QString className = item->text();

    tableClassStudents->blockSignals(true);
    tableClassStudents->setRowCount(0);

    auto students = DatabaseManager::getInstance()->getAllStudents();
    QVector<Student> classStudents;
    for (const auto& s : students) {
        if (s.getClassName() == className) {
            classStudents.append(s);
        }
    }

    std::sort(classStudents.begin(), classStudents.end(), [](const Student& a, const Student& b) {
        return a.getMssv() < b.getMssv();
    });

    for (const auto& s : classStudents) {
        int row = tableClassStudents->rowCount();
        tableClassStudents->insertRow(row);

        tableClassStudents->setItem(row, 0, createReadOnlyItem(s.getMssv()));
        tableClassStudents->setItem(row, 1, createReadOnlyItem(s.getFullName()));
        tableClassStudents->setItem(row, 2, createReadOnlyItem(s.getBirthDate()));
        tableClassStudents->setItem(row, 3, createReadOnlyItem(s.getHometown()));
        tableClassStudents->setItem(row, 4, createReadOnlyItem(s.getClassName()));
        tableClassStudents->setItem(row, 5, createReadOnlyItem(QString::number(s.calculateGPA(), 'f', 2)));

        QPushButton* btnDel = new QPushButton(QString::fromUtf8(u8"Xóa"));
        btnDel->setStyleSheet("background-color: #ef4444; color: white; border-radius: 4px; padding: 4px 8px;");
        connect(btnDel, &QPushButton::clicked, this, [this, s](){
            DatabaseManager::getInstance()->deleteStudent(s.getMssv());
            loadClassesList();
            loadDataByFilter();
            updateDashboardStats();
            if(listClasses->currentItem()) onClassSelected(listClasses->currentItem());
        });

        QPushButton* btnEdit = new QPushButton(QString::fromUtf8(u8"Sửa"));
        btnEdit->setStyleSheet("background-color: #f59e0b; color: white; border-radius: 4px; padding: 4px 8px; margin-right: 5px;");
        connect(btnEdit, &QPushButton::clicked, this, [this, s](){
            handleEditStudent(s.getMssv());
            if(listClasses->currentItem()) onClassSelected(listClasses->currentItem());
        });

        QWidget* actionWidget = new QWidget();
        QHBoxLayout* actionLayout = new QHBoxLayout(actionWidget);
        actionLayout->setContentsMargins(0, 0, 0, 0);
        actionLayout->addWidget(btnEdit);
        actionLayout->addWidget(btnDel);
        tableClassStudents->setCellWidget(row, 6, actionWidget);
    }
    tableClassStudents->blockSignals(false);
}

// Hàm này giờ chỉ phục vụ Tab 1 (Tất cả sinh viên + Tìm kiếm)
void AdminWindow::loadDataByFilter(const QString& filterType, const QString& filterValue) {
    Q_UNUSED(filterType);
    Q_UNUSED(filterValue);

    tableWidget->blockSignals(true);
    tableWidget->setRowCount(0);

    auto students = DatabaseManager::getInstance()->getAllStudents();
    QVector<Student> filteredStudents;
    QString keyword = txtSearch ? txtSearch->text().trimmed().toLower() : "";

    for (const auto& s : students) {
        if (!keyword.isEmpty()) {
            bool matchMssv = s.getMssv().toLower().contains(keyword);
            bool matchName = s.getFullName().toLower().contains(keyword);
            if (!matchMssv && !matchName) continue;
        }
        filteredStudents.append(s);
    }

    std::sort(filteredStudents.begin(), filteredStudents.end(), [](const Student& a, const Student& b) {
        return a.getMssv() < b.getMssv();
    });

    for (const auto& s : filteredStudents) {
        int row = tableWidget->rowCount();
        tableWidget->insertRow(row);

        tableWidget->setItem(row, 0, createReadOnlyItem(s.getMssv()));
        tableWidget->setItem(row, 1, createReadOnlyItem(s.getFullName()));
        tableWidget->setItem(row, 2, createReadOnlyItem(s.getBirthDate()));
        tableWidget->setItem(row, 3, createReadOnlyItem(s.getHometown()));
        tableWidget->setItem(row, 4, createReadOnlyItem(s.getClassName()));
        tableWidget->setItem(row, 5, createReadOnlyItem(QString::number(s.calculateGPA(), 'f', 2)));

        QPushButton* btnDel = new QPushButton(QString::fromUtf8(u8"Xóa"));
        btnDel->setStyleSheet("background-color: #ef4444; color: white; border-radius: 4px; padding: 4px 8px;");
        connect(btnDel, &QPushButton::clicked, this, [this, s](){
            DatabaseManager::getInstance()->deleteStudent(s.getMssv());
            loadClassesList();
            loadDataByFilter();
            updateDashboardStats();
            if(listClasses->currentItem()) onClassSelected(listClasses->currentItem());
        });

        QPushButton* btnEdit = new QPushButton(QString::fromUtf8(u8"Sửa"));
        btnEdit->setStyleSheet("background-color: #f59e0b; color: white; border-radius: 4px; padding: 4px 8px; margin-right: 5px;");
        connect(btnEdit, &QPushButton::clicked, this, [this, s](){
            handleEditStudent(s.getMssv());
            if(listClasses->currentItem()) onClassSelected(listClasses->currentItem());
        });

        QWidget* actionWidget = new QWidget();
        QHBoxLayout* actionLayout = new QHBoxLayout(actionWidget);
        actionLayout->setContentsMargins(0, 0, 0, 0);
        actionLayout->addWidget(btnEdit);
        actionLayout->addWidget(btnDel);
        tableWidget->setCellWidget(row, 6, actionWidget);
    }

    tableWidget->blockSignals(false);
    updateDashboardStats();
}

void AdminWindow::importFromFile() {
    QString filePath = QFileDialog::getOpenFileName(this, QString::fromUtf8(u8"Chọn file danh sách sinh viên"), "", "File CSV (*.csv);;Tất cả tệp (*.*)");
    if (filePath.isEmpty()) return;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, QString::fromUtf8(u8"Lỗi"), QString::fromUtf8(u8"Không thể mở file dữ liệu!"));
        return;
    }

    QTextStream in(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    in.setEncoding(QStringConverter::Utf8);
#else
    in.setCodec("UTF-8");
#endif

    int count = 0;
    bool isFirstLine = true;

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;

        if (isFirstLine && (line.contains("MSSV") || line.contains("HoTen"))) {
            isFirstLine = false;
            continue;
        }

        QStringList parts = line.split(",");
        if (parts.size() >= 5) {
            QString mssv = parts[0].trimmed();
            QString name = parts[1].trimmed();
            QString dob = parts[2].trimmed();
            QString hometown = parts[3].trimmed();
            QString cls = parts[4].trimmed();

            double gpa = (parts.size() >= 6) ? parts[5].trimmed().toDouble() : 0.0;

            if (!mssv.isEmpty() && !name.isEmpty()) {
                Student s(mssv, name, dob, hometown, cls, "123456", gpa);
                DatabaseManager::getInstance()->addOrUpdateStudent(s);
                count++;
            }
        }
    }

    file.close();
    QMessageBox::information(this, QString::fromUtf8(u8"Thành công"), QString::fromUtf8(u8"Đã tải lên %1 sinh viên!").arg(count));

    loadClassesList();
    loadDataByFilter();
    updateDashboardStats();
}

void AdminWindow::addStudent() {
    if(txtMssv->text().isEmpty() || txtName->text().isEmpty()) {
        QMessageBox::warning(this, QString::fromUtf8(u8"Cảnh báo"), QString::fromUtf8(u8"Vui lòng nhập đầy đủ MSSV và Họ Tên!"));
        return;
    }

    Student s(txtMssv->text().trimmed(), txtName->text().trimmed(), txtDob->text().trimmed(), txtHometown->text().trimmed(), txtClass->text().trimmed());
    DatabaseManager::getInstance()->addOrUpdateStudent(s);

    loadClassesList();
    loadDataByFilter();
    updateDashboardStats();

    // Nếu tab lớp học đang chọn 1 lớp trùng với sinh viên mới, tải lại bảng đó
    if(listClasses->currentItem() && listClasses->currentItem()->text() == s.getClassName()) {
        onClassSelected(listClasses->currentItem());
    }

    txtMssv->clear(); txtName->clear(); txtDob->clear(); txtHometown->clear(); txtClass->clear();
}

void AdminWindow::addClassSchedule() {
    if (!cbScheduleClass || !cbScheduleCourse || !cbDayOfWeek || !txtTimeSlot || !txtRoom) return;

    QString className = cbScheduleClass->currentText();
    QString courseCode = cbScheduleCourse->currentText();
    QString day = cbDayOfWeek->currentText();
    QString timeSlot = txtTimeSlot->text().trimmed();
    QString room = txtRoom->text().trimmed();

    if (timeSlot.isEmpty() || room.isEmpty()) {
        QMessageBox::warning(this, QString::fromUtf8(u8"Thông báo"), QString::fromUtf8(u8"Vui lòng nhập đầy đủ tiết học và phòng học!"));
        return;
    }

    if (tableSchedules) {
        int row = tableSchedules->rowCount();
        tableSchedules->insertRow(row);
        tableSchedules->setItem(row, 0, createReadOnlyItem(className));
        tableSchedules->setItem(row, 1, createReadOnlyItem(courseCode));
        tableSchedules->setItem(row, 2, createReadOnlyItem(day));
        tableSchedules->setItem(row, 3, createReadOnlyItem(timeSlot));
        tableSchedules->setItem(row, 4, createReadOnlyItem(room));
    }

    txtTimeSlot->clear();
    txtRoom->clear();
    QMessageBox::information(this, QString::fromUtf8(u8"Thành công"), QString::fromUtf8(u8"Đã thêm lịch học thành công!"));
}

void AdminWindow::updateDashboardStats() {
    auto students = DatabaseManager::getInstance()->getAllStudents();
    int total = students.size();
    if (total == 0) {
        if (lblTotalStudents) lblTotalStudents->setText("0");
        if (lblAverageGpa) lblAverageGpa->setText("0.00");
        if (lblExcellentRatio) lblExcellentRatio->setText("0.0%");
        return;
    }

    double sumGpa = 0.0;
    int excellentCount = 0;
    for (const auto& s : students) {
        double gpa = s.calculateGPA();
        sumGpa += gpa;
        if (gpa >= 3.2) excellentCount++;
    }

    double avgGpa = sumGpa / total;
    double ratio = (double)excellentCount / total * 100.0;

    if (lblTotalStudents) lblTotalStudents->setText(QString::number(total));
    if (lblAverageGpa) lblAverageGpa->setText(QString::number(avgGpa, 'f', 2));
    if (lblExcellentRatio) lblExcellentRatio->setText(QString::number(ratio, 'f', 1) + "%");
}

void AdminWindow::onCellDoubleClicked(int row, int column) {
    Q_UNUSED(column);
    QTableWidgetItem* mssvItem = tableWidget->item(row, 0);
    if (mssvItem) {
        QString mssv = mssvItem->text();
        GradeDialog dialog(mssv, this);
        dialog.exec();

        loadClassesList();
        loadDataByFilter();
        updateDashboardStats();
    }
}

void AdminWindow::handleEditStudent(const QString& mssv) {
    EditStudentDialog dialog(mssv, this);
    if (dialog.exec() == QDialog::Accepted) {
        loadClassesList();
        loadDataByFilter();
        updateDashboardStats();
    }
}

void AdminWindow::handleLogout() {
    LoginWindow* loginWin = new LoginWindow();
    loginWin->show();
    this->close();
}