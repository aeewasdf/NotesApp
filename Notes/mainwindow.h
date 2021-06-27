#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    void createActions();
    void createMenus();
    QMenu *fileMenu;
    QMenu *editMenu;
    QAction *newFile;
    QAction *openFile;
    QAction *saveFile;
    QAction *undo;
    QAction *redo;

private slots:
    void newf();
    void open();
    void save();
    void textUndo();
    void textRedo();
    void edited();
    void listClicked(QListWidgetItem* item);
    void closeEvent(QCloseEvent *event);
    void add();
    void del();
};
#endif // MAINWINDOW_H
