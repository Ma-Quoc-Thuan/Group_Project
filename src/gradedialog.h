#ifndef GRADEDIALOG_H
#define GRADEDIALOG_H

#include <QDialog>
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

class GradeDialog : public QDialog {
    Q_OBJECT

public:
    explicit GradeDialog(const QString& mssv, QWidget* parent = nullptr);

private slots:
    void loadCourseData();
    void saveScores();
    void addCourse();
    void deleteCourse(int index);

private:
    QString currentMssv;
    QTableWidget* tableWidget;
    QPushButton* btnSaveScores;

    QLineEdit* txtCourseId;
    QLineEdit* txtCourseName;
    QLineEdit* txtCredits;
    QLineEdit* txtDay;
    QLineEdit* txtTime;
    QLineEdit* txtRoom;
    // Đã xóa 2 biến txtMidterm và txtFinal để tối ưu UX

    QLabel* lblGpaSummary;
};

#endif // GRADEDIALOG_H