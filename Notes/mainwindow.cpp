#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QSplitter>
#include <QStringListModel>
#include <QMap>
#include <QMapIterator>
#include <QDialog>
#include <QMessageBox>
#include <QCloseEvent>

QMap<QString, QString> hash;
bool edi = false;
QString lastChosen = "";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    createActions();
    createMenus();
    this->setWindowTitle("New File");

}

MainWindow::~MainWindow()
{

    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event2){
    if (edi){
        QMessageBox msgBox;
        msgBox.setText("Сохрание файла");
        msgBox.setInformativeText("Хотите ли вы сохранить файл?");
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);
        int ret = msgBox.exec();
        switch (ret) {
            case QMessageBox::Save:
                save();
                event2->accept();
                break;
            case QMessageBox::Cancel:
                event2->ignore();
                break;
                //close();
            case QMessageBox::Discard:
                event2->accept();
                break;
        }
    }
}

void MainWindow::createActions(){
    newFile = new QAction("Создать файл", this);
    newFile->setShortcut(QKeySequence::New);
    newFile->setStatusTip("Создать файл");
    connect(newFile, &QAction::triggered, this, &MainWindow::newf);

    openFile = new QAction("Открыть файл", this);
    openFile->setShortcut(QKeySequence::Open);
    openFile->setStatusTip("Открыть файл");
    connect(openFile, &QAction::triggered, this, &MainWindow::open);

    saveFile = new QAction("Сохранить", this);
    saveFile->setShortcut(QKeySequence::Save);
    saveFile->setStatusTip("Сохранить");
    connect(saveFile, &QAction::triggered, this, &MainWindow::save);

    undo = new QAction("Отменить", this);
    undo->setShortcut(QKeySequence::Undo);
    undo->setStatusTip("Шаг назад");
    connect(undo, &QAction::triggered, this, &MainWindow::textUndo);

    redo = new QAction("Шаг вперёд", this);
    redo->setShortcut(QKeySequence::Redo);
    redo->setStatusTip("Шаг вперёд");
    connect(redo, &QAction::triggered, this, &MainWindow::textRedo);

    connect(ui->listWidget, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(listClicked(QListWidgetItem*)));
    connect(ui->textEdit, SIGNAL(textChanged()), this, SLOT(edited()));
    connect(ui->addB, SIGNAL(clicked()), this, SLOT(add()));
    connect(ui->delB, SIGNAL(clicked()), this, SLOT(del()));
}

void MainWindow::add(){
    QString tex = ui->lineEdit->text();
    if(tex == "") return;
    if(tex != ""){
        hash.insert(tex, "");
    }
    ui->listWidget->clear();
    QMapIterator <QString, QString> p(hash);
    while (p.hasNext()) {
        QString word = p.next().key();
        ui->listWidget->addItem(word);
    }
    edited();
}

void MainWindow::del(){
    QString tex = ui->listWidget->selectedItems()[0]->text();
    if(tex == "") return;
    hash.remove(tex);
    ui->listWidget->clear();
    QMapIterator <QString, QString> p(hash);
    if(tex == lastChosen) ui->textEdit->clear();
    while (p.hasNext()) {
        QString word = p.next().key();
        ui->listWidget->addItem(word);
    }
    edited();
}

void MainWindow::createMenus(){
    fileMenu = menuBar()->addMenu("Файл");
    fileMenu->addAction(newFile);
    fileMenu->addAction(openFile);
    fileMenu->addAction(saveFile);
    fileMenu->addSeparator();

    editMenu = menuBar()->addMenu("Правка");
    editMenu->addAction(undo);
    editMenu->addAction(redo);


}

void MainWindow::open(){
    QString fileName = QFileDialog::getOpenFileName(this, "Открыть файл", "", "Записная книжка (*.note);; Все файлы (*)");
    //QStringList type = fileName.split('.');
    if (fileName == "") return;
    QFile file = fileName;
    //QString temp = "";
    //QList<QString> list;
    ui->textEdit->clear();
    ui->listWidget->clear();

    if ((file.exists())&&(file.open(QIODevice::ReadOnly)))
    {
        this->setWindowTitle(fileName);
        ui->statusbar->hide();
        QDataStream in(&file);
        in.setVersion(QDataStream::Qt_4_5);
        hash.clear();
        in >> hash;
        QMapIterator <QString, QString> p(hash);
        while (p.hasNext()) {
            QString word = p.next().key();
            ui->listWidget->addItem(word);
        }
        edi = false;
        file.close();
    }
}

void MainWindow::listClicked(QListWidgetItem* item){
    QString text = item->text();
    lastChosen = text;
    ui->textEdit->setText(hash.find(text).value());
}

void MainWindow::edited(){
    if(!edi)
        this->setWindowTitle(this->windowTitle() + "*");
    edi = true;
    if(lastChosen != "")
        hash[lastChosen] = ui->textEdit->toPlainText();
    return;
}

void MainWindow::textUndo(){
    ui->textEdit->undo();
}

void MainWindow::textRedo(){
    ui->textEdit->redo();
}

void MainWindow::newf(){
    if (edi){
        QMessageBox msgBox;
        msgBox.setText("Сохрание файла");
        msgBox.setInformativeText("Хотите ли вы сохранить файл?");
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);
        int ret = msgBox.exec();
        switch (ret) {
            case QMessageBox::Save:
                save();
                break;
            case QMessageBox::Cancel:
                return;
            case QMessageBox::Discard:
                break;
        }
    }
    hash.clear();
    ui->textEdit->clear();
    ui->listWidget->clear();
    this->setWindowTitle("New File");
    return;
}

void MainWindow::save(){
    if (edi){
        QMessageBox msg;
        QString fileName = QFileDialog::getSaveFileName(this, "Сохранить файл", "", "Записная книжка (*.note);;Все файлы (*)");
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly)) {
            msg.setText("Ошибка чтения файла");
            msg.exec();
            return;
        }
        QDataStream out(&file);
        out.setVersion(QDataStream::Qt_4_5);
        out << hash;
        file.close();
        this->setWindowTitle(fileName);
    }
}
