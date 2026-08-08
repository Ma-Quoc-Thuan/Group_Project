#ifndef ADMINWINDOW_H
#define ADMINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QListWidget> // Đã thay QTreeWidget bằng QListWidget
#include <QTableWidget>
#include <QPushButton>
#include <QComboBox>
#include <QTabWidget>
#include <QLabel>

class AdminWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit AdminWindow(QWidget *parent = nullptr);

private slots:
    void onSearchTextChanged(const QString& text);
    void addCourse();
    void onClassSelected(QListWidgetItem* item); // Hàm sự kiện mới khi chọn Lớp
    void importFromFile();
    void addStudent();
    void onCellDoubleClicked(int row, int column);
    void handleLogout();
    void addClassSchedule();
    void updateDashboardStats();
    void handleEditStudent(const QString& mssv);

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

    // Tab 2 (Mới): Quản lý Lớp học
    QListWidget* listClasses = nullptr;
    QTableWidget* tableClassStudents = nullptr;

    // Tab 3: Lịch học
    QComboBox* cbScheduleClass = nullptr;
    QComboBox* cbScheduleCourse = nullptr;
    QComboBox* cbDayOfWeek = nullptr;
    QLineEdit* txtTimeSlot = nullptr;
    QLineEdit* txtRoom = nullptr;
    QTableWidget* tableSchedules = nullptr;

    // Tab 4: Thống kê Cards
    QLabel* lblTotalStudents = nullptr;
    QLabel* lblAverageGpa = nullptr;
    QLabel* lblExcellentRatio = nullptr;

    // Hàm phụ trợ
    QTableWidgetItem* createReadOnlyItem(const QString& text);
    void loadClassesList(); // Hàm mới tải danh sách lớp học
    void loadDataByFilter(const QString& filterType = "", const QString& filterValue = "");
};

#endif // ADMINWINDOW_H