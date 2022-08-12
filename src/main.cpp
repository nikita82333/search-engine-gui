#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow* window;

    try {
        window = new MainWindow(nullptr);
    } catch (const std::exception& exception) {
        qDebug() << exception.what();
        QMessageBox::critical(nullptr, "Error", exception.what(), QMessageBox::Close);
        exit(-1);
    }

    QTimer::singleShot(0, window, SLOT(updateDocumentBase()));

    window->resize(640, 480);
    window->show();

    return QApplication::exec();
}
