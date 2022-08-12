#ifndef SEARCH_ENGINE_GUI_MAINWINDOW_H
#define SEARCH_ENGINE_GUI_MAINWINDOW_H

#include <string>
#include <string_view>
#include <vector>

#include <QMainWindow>
#include <QDebug>
#include <QMessageBox>
#include <QCloseEvent>
#include <QMutex>
#include <QTimer>
#include <QString>

#include "ConverterJSON.h"
#include "InvertedIndex.h"
#include "SearchServer.h"
#include "CustomExceptions.hpp"

enum class MessageType {
    Info,
    Warning,
    Error
};

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;

public slots:
    void loadRequestsFromFile();
    void saveRequestsToFile();
    void addRequest();
    void removeRequest();
    void startSearch();
    void updateDocumentBase();
    void openAbout();
    void openSettings();

private:
    Ui::MainWindow *ui;

    const QString _programVersion {"1.0"};
    QString _programName;
    ConverterJSON _converterJson;
    InvertedIndex _invertedIndex;
    QMutex _invertedIndexAccess;
    QMutex _statusBarAccess;
    QTimer _updateTimer;

    void displaySearchResults(const std::vector<std::vector<RelativeIndex>>& relativeIndex) const;
    void saveSearchResults(const std::vector<std::vector<RelativeIndex>>& relativeIndex);
    void messageToStatusBar(const QString& message, MessageType type);
    void closeEvent(QCloseEvent* event) override;
};


#endif //SEARCH_ENGINE_GUI_MAINWINDOW_H
