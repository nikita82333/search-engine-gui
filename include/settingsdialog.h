#ifndef SEARCH_ENGINE_GUI_SETTINGSDIALOG_H
#define SEARCH_ENGINE_GUI_SETTINGSDIALOG_H

#include <QDialog>
#include <QCloseEvent>
#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>
#include <QStringList>
#include <QFile>
#include <QListWidget>

#include "ConverterJSON.h"
#include "CustomExceptions.hpp"


QT_BEGIN_NAMESPACE
namespace Ui { class SettingsDialog; }
QT_END_NAMESPACE

class SettingsDialog : public QDialog {
Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);

    ~SettingsDialog() override;

public slots:
    void addFile();
    void removeFile();
    void saveSettings();
    void selectFiles();
    void checkFileExists(QListWidgetItem* item);

signals:
    void closeDialog();

private:
    Ui::SettingsDialog *ui;

    const int _maxValueToEnter {1000000};
    ConverterJSON _converterJson;

};


#endif //SEARCH_ENGINE_GUI_SETTINGSDIALOG_H
