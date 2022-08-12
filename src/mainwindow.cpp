// You may need to build the project (run Qt uic code generator) to get "ui_MainWindow.h" resolved

#include "mainwindow.h"
#include "ui_MainWindow.h"
#include "settingsdialog.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    _programName = _converterJson.GetProgramName().c_str();
    setWindowTitle(_programName + " " + _programVersion);
    if (_programVersion != _converterJson.GetConfigFileVersion().c_str()) {
        qDebug() << "Warning: \"Config.json\" has incorrect file version!";
        messageToStatusBar(tr("Warning: \"Config.json\" has incorrect file version!"), MessageType::Warning);
    }

    _updateTimer.start(_converterJson.GetBaseUpdateInterval() * 1000);
    QObject::connect(&_updateTimer, SIGNAL(timeout()),
                     this, SLOT(updateDocumentBase()));
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::loadRequestsFromFile() {
    try {
        std::vector<std::string> requests = _converterJson.GetRequests();
        ui->listWidget->clear();
        for (const auto& request : requests) {
            auto* item = new QListWidgetItem(request.c_str(), ui->listWidget);
            item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
            item->setBackground(QColor(230,230,230));
        }
        messageToStatusBar(tr("Requests loaded from file \"requests.json\"."), MessageType::Info);
    } catch (const FileMissingException& exception) {
        qDebug() << exception.what();
        messageToStatusBar(exception.what(), MessageType::Error);
    }
}

void MainWindow::saveRequestsToFile() {
    std::vector<std::string> requests;
    for (int iRow = 0; iRow < ui->listWidget->count(); ++iRow) {
        requests.emplace_back(ui->listWidget->item(iRow)->text().toStdString());
    }
    try {
        _converterJson.PutRequests(requests);
        messageToStatusBar(tr("Requests saved to the file \"requests.json\"."), MessageType::Info);
    } catch (const FileBusyException& exception) {
        qDebug() << exception.what();
        messageToStatusBar(exception.what(), MessageType::Error);
    }
}

void MainWindow::addRequest() {
    auto* item = new QListWidgetItem("", ui->listWidget);
    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
    item->setBackground(QColor(230,230,230));
}

void MainWindow::removeRequest() {
    if (!ui->listWidget->selectedItems().empty()) {
        for (const auto& item : ui->listWidget->selectedItems()) {
            ui->listWidget->takeItem(ui->listWidget->row(item));
        }
    }
}

void MainWindow::startSearch() {
    _invertedIndexAccess.lock();
    SearchServer searchServer(_invertedIndex);
    _invertedIndexAccess.unlock();
    std::vector<std::string> requests;
    for (int iRow = 0; iRow < ui->listWidget->count(); ++iRow) {
        requests.emplace_back(ui->listWidget->item(iRow)->text().toStdString());
    }

    std::vector<std::vector<RelativeIndex>> relativeIndex;
    _invertedIndexAccess.lock();
    relativeIndex = searchServer.Search(requests);
    _invertedIndexAccess.unlock();
    displaySearchResults(relativeIndex);
    saveSearchResults(relativeIndex);
}

void MainWindow::updateDocumentBase() {
    auto [textDocs, warningMessage] = _converterJson.GetTextDocuments();
    if (!warningMessage.empty()) {
        qDebug() << warningMessage.c_str();
        messageToStatusBar(warningMessage.c_str(), MessageType::Warning);
    }
    _invertedIndexAccess.lock();
    _invertedIndex.UpdateDocumentBase(textDocs);
    _invertedIndexAccess.unlock();
}

void MainWindow::openAbout()
{
    QMessageBox::about(this, tr("About ") + _programName,
                       "<h2>" + _programName + " " + _programVersion + "</h2>" +
                          tr("<p>Copyright &copy; 2022 Kuznetsov N.V."
                          "<p>SearchEngine is a training project. "
                          "The application searches through text files "
                          "by building a search index. Developed "
                          "using the <b>Qt &copy;</b> and <b>nlohmann_json</b> libraries."));
}

void MainWindow::openSettings() {
    auto* settingsDialog = new SettingsDialog;
    if (settingsDialog->exec() == QDialog::Accepted) {
        try {
            _converterJson.LoadConfig();
            messageToStatusBar(tr("New settings applied successfully!"), MessageType::Info);
        } catch (const std::exception& exception ) {
            qDebug() << exception.what();
            messageToStatusBar(exception.what(), MessageType::Error);
        }
    }
    delete settingsDialog;
}

void MainWindow::displaySearchResults(const std::vector<std::vector<RelativeIndex>>& relativeIndex) const {
    ui->treeWidget->clear();
    ui->treeWidget->setColumnCount(4);
    ui->treeWidget->setHeaderLabels({"request", "result", "docId", "rank"});
    std::vector<std::string> fileNames = _converterJson.GetFileNames();
    int iRequest = 1;
    for (const auto& answer : relativeIndex) {
        auto* request = new QTreeWidgetItem(ui->treeWidget);
        request->setText(0, "request" + QString::number(iRequest));
        auto* result = new QTreeWidgetItem(request);
        if (answer.empty()) {
            result->setText(1, "false");
        } else {
            result->setText(1, "true");
        }
        for (const auto& [docId, rank]: answer) {
            auto* doc = new QTreeWidgetItem(result);
            doc->setText(2, QString::number(docId));
            doc->setToolTip(2, fileNames[docId].c_str());
            doc->setText(3, QString::number(rank,'g',3));
        }
        ++iRequest;
    }
    ui->treeWidget->resizeColumnToContents(0);
    ui->treeWidget->resizeColumnToContents(1);
    ui->treeWidget->resizeColumnToContents(2);
    ui->treeWidget->resizeColumnToContents(3);
}

void MainWindow::saveSearchResults(const std::vector<std::vector<RelativeIndex>> &relativeIndex) {
    std::vector<std::vector<std::pair<int, float>>> answers;
    for (const auto& answer : relativeIndex) {
        std::vector<std::pair<int, float>> docs;
        for (const auto& [docId, rank]: answer) {
            docs.emplace_back(std::make_pair(docId,  rank));
        }
        answers.emplace_back(docs);
    }
    try {
        _converterJson.PutAnswers(answers);
        messageToStatusBar(tr("Search results saved to the file \"answers.json\"."), MessageType::Info);
    } catch (const FileBusyException& exception) {
        qDebug() << exception.what();
        messageToStatusBar(exception.what(), MessageType::Error);
    }
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    auto answer = QMessageBox::question(this, _programName,
                                        tr("Are you sure?"),QMessageBox::Yes| QMessageBox::No);
    switch (answer) {
        case QMessageBox::Yes:
            _updateTimer.stop();
            event->accept();
            break;
        case QMessageBox::No:
            event->ignore();
            break;
    }
}

void MainWindow::messageToStatusBar(const QString& message, MessageType type) {
    QMutexLocker locker(&_statusBarAccess);
    switch (type) {
        case MessageType::Info:
            ui->statusBar->setStyleSheet("color: black");
            ui->statusBar->showMessage(message,5000);
            break;
        case MessageType::Warning:
            ui->statusBar->setStyleSheet("color: blue");
            ui->statusBar->showMessage(message,70000);
            break;
        case MessageType::Error:
            ui->statusBar->setStyleSheet("color: red");
            ui->statusBar->showMessage(message,10000);
            break;
    }
}

