// You may need to build the project (run Qt uic code generator) to get "ui_SettingsDialog.h" resolved

#include "settingsdialog.h"
#include "ui_SettingsDialog.h"


SettingsDialog::SettingsDialog(QWidget *parent) :
        QDialog(parent), ui(new Ui::SettingsDialog) {
    ui->setupUi(this);

    QObject::connect(this, SIGNAL(closeDialog()), this, SLOT(accept()));

    try {
        _converterJson.LoadConfig();
        ui->lineEditName->setText(_converterJson.GetProgramName().c_str());
        ui->lineEditVersion->setText(_converterJson.GetConfigFileVersion().c_str());
        ui->lineEditMaxResponses->setText(QString::number(_converterJson.GetResponsesLimit()));
        ui->lineEditUpdateInterval->setText(QString::number(_converterJson.GetBaseUpdateInterval()));
        std::vector<std::string> fileNames = _converterJson.GetFileNames();
        ui->listWidget->clear();
        for (const auto& fileName : fileNames) {
            auto* item = new QListWidgetItem(fileName.c_str(), ui->listWidget);
            item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
            item->setBackground(QColor(230,230,230));
        }
    } catch (const std::exception& exception) {
        qDebug() << exception.what();
        QMessageBox::critical(this, tr("Error"), exception.what(), QMessageBox::Close);
    }

    ui->lineEditMaxResponses->setValidator(new QIntValidator(1,_maxValueToEnter,this));
    ui->lineEditUpdateInterval->setValidator(new QIntValidator(1,_maxValueToEnter,this));
}

SettingsDialog::~SettingsDialog() {
    delete ui;
}

void SettingsDialog::addFile() {
    auto* item = new QListWidgetItem("", ui->listWidget);
    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
    item->setBackground(QColor(230,230,230));
}

void SettingsDialog::removeFile() {
    if (!ui->listWidget->selectedItems().empty()) {
        for (const auto& item : ui->listWidget->selectedItems()) {
            ui->listWidget->takeItem(ui->listWidget->row(item));
        }
    }
}

void SettingsDialog::saveSettings() {
    auto answer = QMessageBox::question(this, " ",
                                        tr("Apply new settings?"), QMessageBox::Yes | QMessageBox::No);
    if (answer == QMessageBox::Yes) {
        bool ok, isReadyToSave = true;
        int responsesLimit = ui->lineEditMaxResponses->text().toInt(&ok);
        if (!ok || responsesLimit <= 0 || responsesLimit > _maxValueToEnter) {
            isReadyToSave = false;
            QMessageBox::critical(this, tr("Error"),
                                  tr("Max responses has incorrect value!"), QMessageBox::Ok);
        }
        int baseUpdateInterval = ui->lineEditUpdateInterval->text().toInt(&ok);
        if (!ok || baseUpdateInterval <= 0 || baseUpdateInterval > _maxValueToEnter) {
            isReadyToSave = false;
            QMessageBox::critical(this, tr("Error"),
                                  tr("Base update interval has incorrect value!"), QMessageBox::Ok);
        }
        std::vector<std::string> fileNames;
        for (int iRow = 0; iRow < ui->listWidget->count(); ++iRow) {
            fileNames.emplace_back(ui->listWidget->item(iRow)->text().toStdString());
        }
        if (fileNames.empty()) {
            QMessageBox::critical(this, tr("Error"),
                                  tr("File list cannot be empty!"), QMessageBox::Ok);
        }
        if (isReadyToSave && !fileNames.empty()) {
            try {
                _converterJson.SaveConfig(responsesLimit, baseUpdateInterval, fileNames);
                emit closeDialog();
            } catch (const FileBusyException &exception) {
                qDebug() << exception.what();
                QMessageBox::critical(this, tr("Error"), exception.what(), QMessageBox::Close);
            }
        }
    }
}

void SettingsDialog::selectFiles() {
    QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Select files"),
                                                          ".","Text file (*.txt)");
    for (const auto& fileName : fileNames) {
        auto* item = new QListWidgetItem(fileName, ui->listWidget);
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
        item->setBackground(QColor(230,230,230));
    }
}

void SettingsDialog::checkFileExists(QListWidgetItem* item) {
    if (!QFile::exists(item->text())) {
        item->setForeground(QBrush(QColor("red")));
    } else {
        item->setForeground(QBrush(QColor("black")));
    }
}
