#ifndef ADMINWINDOW_H
#define ADMINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QListWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QComboBox>
#include <QTabWidget>
#include <QLabel>

// Thêm thư viện vẽ biểu đồ của Qt Charts
#include <QtCharts/QChartView>

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
using namespace QtCharts;
#endif

class AdminWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit AdminWindow(QWidget *parent = nullptr);

private slots:
    void onSearchTextChanged(const QString& text);
    void addCourse();
    void onClassSelected(QListWidgetItem* item);
    void importFromFile();
    void addStudent();
    void onCellDoubleClicked(int row, int column);
    void handleLogout();
    void addClassSchedule();
    void updateDashboardStats();
    void handleEditStudent(const QString& mssv);

    // Slot mới: Xử lý khi click vào tiêu đề cột để sắp xếp
    void onHeaderClicked(int logicalIndex);

private:
    void setupUI();
    void applyStylesheet();

    // Thanh Header & Sidebar
    QLineEdit* txtSearch = nullptr;
    QPushButton* btnAddCourse = nullptr;
    QPushButton* btnImportFile = nullptr;
    QPushButton* btnLogout = nullptr;

    // Phân vùng Tab
    QTabWidget* mainTabWidget = nullptr;

    // Tab 1: Sinh viên (Danh sách tổng)
    QLineEdit* txtMssv = nullptr;
    QLineEdit* txtName = nullptr;
    QLineEdit* txtDob = nullptr;
    QLineEdit* txtHometown = nullptr;
    QLineEdit* txtClass = nullptr;
    QTableWidget* tableWidget = nullptr;

    // Tab 2: Quản lý Lớp học
    QListWidget* listClasses = nullptr;
    QTableWidget* tableClassStudents = nullptr;

    // Tab 3: Lịch học
    QComboBox* cbScheduleClass = nullptr;
    QComboBox* cbScheduleCourse = nullptr;
    QComboBox* cbDayOfWeek = nullptr;
    QLineEdit* txtTimeSlot = nullptr;
    QLineEdit* txtRoom = nullptr;
    QTableWidget* tableSchedules = nullptr;

    // Tab 4: Thống kê Cards & Biểu đồ
    QLabel* lblTotalStudents = nullptr;
    QLabel* lblAverageGpa = nullptr;
    QLabel* lblExcellentRatio = nullptr;
    QChartView* chartView = nullptr;

    // Biến lưu trữ trạng thái sắp xếp hiện tại
    int m_sortColumn = 0;                             // Cột sắp xếp (0: MSSV, 1: Tên, 5: GPA)
    Qt::SortOrder m_sortOrder = Qt::AscendingOrder;   // Hướng sắp xếp (Tăng/Giảm)

    // Hàm phụ trợ
    QTableWidgetItem* createReadOnlyItem(const QString& text);
    void loadClassesList();
    void loadDataByFilter(const QString& filterType = "", const QString& filterValue = "");
};

#endif // ADMINWINDOW_H