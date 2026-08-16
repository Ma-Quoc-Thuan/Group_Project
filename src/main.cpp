#include <QApplication>
#include <QFile>
#include <QTextStream>
#include "loginwindow.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    // Tải stylesheet nếu có
    QFile file("style.qss");
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream stream(&file);
        a.setStyleSheet(stream.readAll());
        file.close();
    }

    LoginWindow w;
    w.show();
    return a.exec();
}