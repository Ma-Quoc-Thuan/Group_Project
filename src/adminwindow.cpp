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
#include <QBrush>
#include <QColor>
#include <QDateTimeEdit>
#include <QDateTime>

// Thêm các thư viện vẽ biểu đồ của Qt Charts
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QtCharts/QChart>

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
using namespace QtCharts;
#endif

namespace {
// Chuẩn hóa điểm về thang điểm 4
double normalizeGPA(double gpa) {
    if (gpa > 4.0 && gpa <= 10.0) {
        if (gpa >= 8.5) return 4.0; // Xuất sắc / Giỏi
        if (gpa >= 7.0) return 3.0; // Khá
        if (gpa >= 5.5) return 2.0; // Trung bình
        if (gpa >= 4.0) return 1.0; // Trung bình yếu
        return 0.0;                 // Yếu / Kém
    }
    if (gpa >= 0.0 && gpa <= 4.0) return gpa;
    return 0.0;
}

// Lấy GPA ưu tiên: Lấy từ điểm môn học, nếu chưa có môn học thì lấy GPA nhập từ CSV
double getStudentGPA(const Student& s) {
    double gpa = s.calculateGPA();
    if (gpa == 0.0) {
        gpa = s.getGPA();
    }
    return normalizeGPA(gpa);
}

// Hàm hỗ trợ sắp xếp danh sách sinh viên theo Cột và Thứ tự (Tăng / Giảm dần)
void sortStudentList(QVector<Student>& list, int sortColumn, Qt::SortOrder order) {
    bool isAsc = (order == Qt::AscendingOrder);
    std::sort(list.begin(), list.end(), [sortColumn, isAsc](const Student& a, const Student& b) {
        if (sortColumn == 1) { // Sắp xếp theo TÊN (A-Z)
            QStringList partsA = a.getFullName().trimmed().split(' ', Qt::SkipEmptyParts);
            QStringList partsB = b.getFullName().trimmed().split(' ', Qt::SkipEmptyParts);
            QString lastNameA = partsA.isEmpty() ? "" : partsA.last();
            QString lastNameB = partsB.isEmpty() ? "" : partsB.last();

            int cmp = QString::compare(lastNameA, lastNameB, Qt::CaseInsensitive);
            if (cmp != 0) return isAsc ? (cmp < 0) : (cmp > 0);

            int cmpFull = QString::compare(a.getFullName(), b.getFullName(), Qt::CaseInsensitive);
            if (cmpFull != 0) return isAsc ? (cmpFull < 0) : (cmpFull > 0);
            return isAsc ? (a.getMssv() < b.getMssv()) : (a.getMssv() > b.getMssv());

        } else if (sortColumn == 5) { // Sắp xếp theo GPA
            double gpaA = getStudentGPA(a);
            double gpaB = getStudentGPA(b);
            if (gpaA != gpaB) return isAsc ? (gpaA < gpaB) : (gpaA > gpaB);

            bool okA = false, okB = false;
            qlonglong idA = a.getMssv().toLongLong(&okA);
            qlonglong idB = b.getMssv().toLongLong(&okB);
            if (okA && okB) return isAsc ? (idA < idB) : (idA > idB);
            return isAsc ? (a.getMssv() < b.getMssv()) : (a.getMssv() > b.getMssv());

        } else { // Cột 0: Sắp xếp theo MSSV (Mặc định)
            bool okA = false, okB = false;
            qlonglong idA = a.getMssv().toLongLong(&okA);
            qlonglong idB = b.getMssv().toLongLong(&okB);
            if (okA && okB) return isAsc ? (idA < idB) : (idA > idB);
            return isAsc ? (a.getMssv() < b.getMssv()) : (a.getMssv() > b.getMssv());
        }
    });
}

// Bộ phân tích dòng CSV thông minh xử lý ngoặc kép và dấu phân tách
QStringList parseCsvLine(const QString& line) {
    QStringList fields;
    bool inQuotes = false;
    QString current;
    QChar delim = line.contains(';') ? ';' : ',';

    for (int i = 0; i < line.length(); ++i) {
        QChar ch = line.at(i);
        if (ch == '"') {
            inQuotes = !inQuotes;
        } else if (ch == delim && !inQuotes) {
            fields.append(current.trimmed());
            current.clear();
        } else {
            current.append(ch);
        }
    }
    fields.append(current.trimmed());

    for (int i = 0; i < fields.size(); ++i) {
        if (fields[i].startsWith('"') && fields[i].endsWith('"') && fields[i].length() >= 2) {
            fields[i] = fields[i].mid(1, fields[i].length() - 2).trimmed();
        }
    }
    return fields;
}
}

AdminWindow::AdminWindow(QWidget *parent) : QMainWindow(parent) {
    setupUI();
    applyStylesheet();

    connect(txtSearch, &QLineEdit::textChanged, this, &AdminWindow::onSearchTextChanged);
    connect(btnAddCourse, &QPushButton::clicked, this, &AdminWindow::addCourse);
    connect(btnImportFile, &QPushButton::clicked, this, &AdminWindow::importFromFile);
    connect(tableWidget, &QTableWidget::cellDoubleClicked, this, &AdminWindow::onCellDoubleClicked);
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

    // --- 1. HEADER BAR ---
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

    // --- 2. THÂN CHÍNH ---
    QWidget* bodyWidget = new QWidget(this);
    QHBoxLayout* bodyLayout = new QHBoxLayout(bodyWidget);
    bodyLayout->setContentsMargins(10, 10, 10, 10);
    bodyLayout->setSpacing(10);

    // --- SIDEBAR TRÁI ---
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

    sidebar->addSpacing(10);
    btnAddCourse = new QPushButton(QString::fromUtf8(u8"➕ Tạo Học Phần (Mở Đăng Ký)"), this);
    btnAddCourse->setObjectName("BtnAddCourse");
    sidebar->addWidget(btnAddCourse);

    btnImportFile = new QPushButton(QString::fromUtf8(u8"📂 Nhập File (CSV)"), this);
    btnImportFile->setObjectName("BtnImportFile");
    sidebar->addWidget(btnImportFile);

    sidebar->addSpacing(15);
    QLabel* lblTimeTitle = new QLabel(QString::fromUtf8(u8"⏰ THỜI GIAN ĐĂNG KÝ"), this);
    lblTimeTitle->setObjectName("SubHeader");
    sidebar->addWidget(lblTimeTitle);

    QDateTimeEdit* dtStart = new QDateTimeEdit(QDateTime::currentDateTime(), this);
    dtStart->setDisplayFormat("dd/MM/yyyy hh:mm");
    dtStart->setCalendarPopup(true);
    sidebar->addWidget(new QLabel(QString::fromUtf8(u8"Bắt đầu:"), this));
    sidebar->addWidget(dtStart);

    QDateTimeEdit* dtEnd = new QDateTimeEdit(QDateTime::currentDateTime().addDays(7), this);
    dtEnd->setDisplayFormat("dd/MM/yyyy hh:mm");
    dtEnd->setCalendarPopup(true);
    sidebar->addWidget(new QLabel(QString::fromUtf8(u8"Kết thúc:"), this));
    sidebar->addWidget(dtEnd);

    QDateTime sTime = DatabaseManager::getInstance()->getRegStartTime();
    QDateTime eTime = DatabaseManager::getInstance()->getRegEndTime();
    if (sTime.isValid()) dtStart->setDateTime(sTime);
    if (eTime.isValid()) dtEnd->setDateTime(eTime);

    QPushButton* btnSaveTime = new QPushButton(QString::fromUtf8(u8"💾 Lưu Hạn Đăng Ký"), this);
    btnSaveTime->setObjectName("BtnAddCourse");
    sidebar->addWidget(btnSaveTime);

    connect(btnSaveTime, &QPushButton::clicked, this, [dtStart, dtEnd, this]() {
        QDateTime start = dtStart->dateTime();
        QDateTime end = dtEnd->dateTime();

        if (start >= end) {
            QMessageBox::warning(this, QString::fromUtf8(u8"Lỗi"), QString::fromUtf8(u8"Thời gian bắt đầu phải nhỏ hơn thời gian kết thúc!"));
            return;
        }

        DatabaseManager::getInstance()->setRegistrationPeriod(start, end);
        QMessageBox::information(this, QString::fromUtf8(u8"Thành công"), QString::fromUtf8(u8"Cập nhật thời gian đăng ký học phần thành công!"));
    });

    sidebar->addStretch();
    bodyLayout->addWidget(sidebarFrame);

    // --- MAIN TAB WIDGET ---
    mainTabWidget = new QTabWidget(this);
    mainTabWidget->setObjectName("MainTabWidget");

    // TAB 1: TẤT CẢ SINH VIÊN
    QWidget* tabStudent = new QWidget();
    QHBoxLayout* tab1Layout = new QHBoxLayout(tabStudent);

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

    QGroupBox* boxTable = new QGroupBox(QString::fromUtf8(u8"Danh Sách Toàn Bộ Sinh Viên"), tabStudent);
    QVBoxLayout* boxTableLayout = new QVBoxLayout(boxTable);

    tableWidget = new QTableWidget();
    tableWidget->setColumnCount(7);
    tableWidget->setHorizontalHeaderLabels({
        "MSSV ↕", QString::fromUtf8(u8"Họ và Tên ↕"), QString::fromUtf8(u8"Ngày Sinh"),
        QString::fromUtf8(u8"Quê Quán"), QString::fromUtf8(u8"Lớp"), "GPA ↕", QString::fromUtf8(u8"Thao Tác")
    });
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableWidget->horizontalHeader()->setSectionsClickable(true);
    connect(tableWidget->horizontalHeader(), &QHeaderView::sectionClicked, this, &AdminWindow::onHeaderClicked);

    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

    boxTableLayout->addWidget(tableWidget);
    tab1Layout->addWidget(boxTable);

    mainTabWidget->addTab(tabStudent, QString::fromUtf8(u8"📋 Danh Sách Tổng"));

    // TAB 2: QUẢN LÝ LỚP HỌC
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
        "MSSV ↕", QString::fromUtf8(u8"Họ và Tên ↕"), QString::fromUtf8(u8"Ngày Sinh"),
        QString::fromUtf8(u8"Quê Quán"), QString::fromUtf8(u8"Lớp"), "GPA ↕", QString::fromUtf8(u8"Thao Tác")
    });
    tableClassStudents->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableClassStudents->horizontalHeader()->setSectionsClickable(true);
    connect(tableClassStudents->horizontalHeader(), &QHeaderView::sectionClicked, this, &AdminWindow::onHeaderClicked);

    tableClassStudents->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableClassStudents->setSelectionBehavior(QAbstractItemView::SelectRows);
    classStudentsLayout->addWidget(tableClassStudents);
    tabClassLayout->addWidget(boxClassStudents);

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

    // TAB 3: THỜI KHÓA BIỂU
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

    // TAB 4: THỐNG KÊ & BÁO CÁO
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

    chartView = new QChartView(this);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setStyleSheet("background-color: transparent;");
    tab3Layout->addWidget(chartView);

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
        QLineEdit, QComboBox, QDateTimeEdit {
            background-color: #334155;
            border: 1px solid #475569;
            border-radius: 6px;
            padding: 6px 10px;
            color: #f8fafc;
        }
        QLineEdit:focus, QComboBox:focus, QDateTimeEdit:focus {
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
        QHeaderView::section:hover {
            background-color: #475569;
            cursor: pointer;
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

void AdminWindow::onHeaderClicked(int logicalIndex) {
    // Chỉ cho phép sắp xếp theo Cột 0 (MSSV), Cột 1 (Tên), hoặc Cột 5 (GPA)
    if (logicalIndex != 0 && logicalIndex != 1 && logicalIndex != 5) return;

    // Đảo chiều sắp xếp nếu click lại đúng cột vừa sắp xếp
    if (m_sortColumn == logicalIndex) {
        m_sortOrder = (m_sortOrder == Qt::AscendingOrder) ? Qt::DescendingOrder : Qt::AscendingOrder;
    } else {
        m_sortColumn = logicalIndex;
        m_sortOrder = Qt::AscendingOrder;
    }

    loadDataByFilter();
    if (listClasses && listClasses->currentItem()) {
        onClassSelected(listClasses->currentItem());
    }
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

    QSpinBox* spinMaxCapacity = new QSpinBox(&dialog);
    spinMaxCapacity->setRange(1, 200);
    spinMaxCapacity->setValue(40);
    form.addRow(QString::fromUtf8(u8"Sĩ Số Tối Đa:"), spinMaxCapacity);

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
        int maxCap = spinMaxCapacity->value();
        QString targetClass = cbClass->currentText();

        if (code.isEmpty() || name.isEmpty()) {
            QMessageBox::warning(this, QString::fromUtf8(u8"Cảnh báo"), QString::fromUtf8(u8"Vui lòng nhập đầy đủ mã và tên môn học!"));
            return;
        }

        Course newCourse;
        newCourse.courseCode = code;
        newCourse.courseName = name;
        newCourse.credits = credits;
        newCourse.maxCapacity = maxCap;
        newCourse.currentEnrolled = 0;
        newCourse.score = 0.0;
        newCourse.targetClass = targetClass;

        DatabaseManager::getInstance()->addAvailableCourse(newCourse);

        QMessageBox::information(this, QString::fromUtf8(u8"Thành công"),
                                 QString::fromUtf8(u8"Đã mở đăng ký môn '%1' (%2) cho lớp [%3] với sĩ số tối đa %4!")
                                     .arg(name).arg(code).arg(targetClass).arg(maxCap));

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

    for (const QString& cls : classes) {
        listClasses->addItem(cls);
        if (cbScheduleClass) cbScheduleClass->addItem(cls);
    }

    if (cbScheduleCourse) {
        for (const QString& crs : allCourses) {
            cbScheduleCourse->addItem(crs);
        }
    }
}

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

    // SẮP XẾP THEO CỘT VÀ HƯỚNG SẮP XẾP HIỆN TẠI
    sortStudentList(classStudents, m_sortColumn, m_sortOrder);

    for (const auto& s : classStudents) {
        int row = tableClassStudents->rowCount();
        tableClassStudents->insertRow(row);

        tableClassStudents->setItem(row, 0, createReadOnlyItem(s.getMssv()));
        tableClassStudents->setItem(row, 1, createReadOnlyItem(s.getFullName()));
        tableClassStudents->setItem(row, 2, createReadOnlyItem(s.getBirthDate()));
        tableClassStudents->setItem(row, 3, createReadOnlyItem(s.getHometown()));
        tableClassStudents->setItem(row, 4, createReadOnlyItem(s.getClassName()));

        double gpaVal = getStudentGPA(s);
        QTableWidgetItem* gpaItem = createReadOnlyItem(QString::number(gpaVal, 'f', 2));
        if (gpaVal < 2.0 && gpaVal > 0.0) {
            gpaItem->setForeground(QBrush(QColor(239, 68, 68)));
            gpaItem->setToolTip(QString::fromUtf8(u8"⚠️ Cảnh báo học tập (GPA < 2.0) - Giới hạn 14 tín chỉ"));
        }
        tableClassStudents->setItem(row, 5, gpaItem);

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

    // SẮP XẾP THEO CỘT VÀ HƯỚNG SẮP XẾP HIỆN TẠI
    sortStudentList(filteredStudents, m_sortColumn, m_sortOrder);

    for (const auto& s : filteredStudents) {
        int row = tableWidget->rowCount();
        tableWidget->insertRow(row);

        tableWidget->setItem(row, 0, createReadOnlyItem(s.getMssv()));
        tableWidget->setItem(row, 1, createReadOnlyItem(s.getFullName()));
        tableWidget->setItem(row, 2, createReadOnlyItem(s.getBirthDate()));
        tableWidget->setItem(row, 3, createReadOnlyItem(s.getHometown()));
        tableWidget->setItem(row, 4, createReadOnlyItem(s.getClassName()));

        double gpaVal = getStudentGPA(s);
        QTableWidgetItem* gpaItem = createReadOnlyItem(QString::number(gpaVal, 'f', 2));
        if (gpaVal < 2.0 && gpaVal > 0.0) {
            gpaItem->setForeground(QBrush(QColor(239, 68, 68)));
            gpaItem->setToolTip(QString::fromUtf8(u8"⚠️ Cảnh báo học tập (GPA < 2.0) - Giới hạn 14 tín chỉ"));
        }
        tableWidget->setItem(row, 5, gpaItem);

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

        if (isFirstLine) {
            isFirstLine = false;
            if (line.contains("MSSV", Qt::CaseInsensitive) || line.contains("HoTen", Qt::CaseInsensitive) ||
                line.contains("Lop", Qt::CaseInsensitive) || line.contains(QString::fromUtf8(u8"Điểm"), Qt::CaseInsensitive)) {
                continue;
            }
        }

        QStringList parts = parseCsvLine(line);

        if (parts.size() >= 5) {
            QString mssv = parts[0];
            QString name = parts[1];
            QString dob = parts[2];
            QString hometown = parts[3];
            QString cls = parts[4];

            double rawGpa = 0.0;
            if (parts.size() >= 6 && !parts[5].isEmpty()) {
                bool ok = false;
                QString gpaStr = parts[5];
                gpaStr.replace(',', '.');
                double val = gpaStr.toDouble(&ok);
                if (ok) rawGpa = val;
            }

            double gpa = normalizeGPA(rawGpa);

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
        if (chartView && chartView->chart()) {
            chartView->chart()->removeAllSeries();
        }
        return;
    }

    double sumGpa = 0.0;
    int countExcellent = 0;
    int countGood = 0;
    int countFair = 0;
    int countAverage = 0;
    int countWeak = 0;

    for (const auto& s : students) {
        double gpa = getStudentGPA(s);
        sumGpa += gpa;

        if (gpa >= 3.60) countExcellent++;
        else if (gpa >= 3.20) countGood++;
        else if (gpa >= 2.50) countFair++;
        else if (gpa >= 2.00) countAverage++;
        else countWeak++;
    }

    double avgGpa = sumGpa / total;
    double ratio = (double)(countExcellent + countGood) / total * 100.0;

    if (lblTotalStudents) lblTotalStudents->setText(QString::number(total));
    if (lblAverageGpa) lblAverageGpa->setText(QString::number(avgGpa, 'f', 2));
    if (lblExcellentRatio) lblExcellentRatio->setText(QString::number(ratio, 'f', 1) + "%");

    QPieSeries *series = new QPieSeries();
    if (countExcellent > 0) series->append(QString::fromUtf8(u8"Xuất sắc (%1)").arg(countExcellent), countExcellent);
    if (countGood > 0)      series->append(QString::fromUtf8(u8"Giỏi (%1)").arg(countGood), countGood);
    if (countFair > 0)      series->append(QString::fromUtf8(u8"Khá (%1)").arg(countFair), countFair);
    if (countAverage > 0)   series->append(QString::fromUtf8(u8"Trung bình (%1)").arg(countAverage), countAverage);
    if (countWeak > 0)      series->append(QString::fromUtf8(u8"Yếu / Cảnh báo (%1)").arg(countWeak), countWeak);

    for (auto slice : series->slices()) {
        slice->setLabelVisible(true);
    }

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle(QString::fromUtf8(u8"BIỂU ĐỒ PHÂN BỔ HỌC LỰC SINH VIÊN (THANG ĐIỂM 4)"));
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->setTheme(QChart::ChartThemeDark);

    if (chartView) {
        chartView->setChart(chart);
    }
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