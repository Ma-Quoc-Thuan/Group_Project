#ifndef STUDENTWINDOW_H
#define STUDENTWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QTableWidget>
#include <QPushButton>
#include "student.h"

class StudentWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit StudentWindow(const QString& mssv, QWidget *parent = nullptr);
    ~StudentWindow() = default;

private slots:
    void handleLogout();
    void handleChangePassword();

private:
    void setupUI();
    void applyStylesheet();
    void refreshStudentData();
    void loadStudentData();
    void loadTranscriptTable();
    void loadCourseRegistrationTable();
    void handleRegisterCourse(const Course& course);
    void handleUnregisterCourse(const QString& courseCode);

    QString currentMssv;
    Student currentStudent;

    QLabel* lblGpaWarning = nullptr; // MỚI: Dán nhãn cảnh báo GPA thấp

    QLabel* lblAvatar = nullptr;
    QLabel* lblName = nullptr;
    QLabel* lblInfo = nullptr;
    QTableWidget* tableTranscript = nullptr;
    QTableWidget* tableRegister = nullptr;
    QPushButton* btnLogout = nullptr;
    QPushButton* btnChangePassword = nullptr;
};

#endif // STUDENTWINDOW_H