#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSharedPointer>

#include "IntegralImage.h"
#include "Trainer.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void on_positiveBrowseButton_clicked();

    void on_negativeBrowseButton_clicked();

    void on_trainButton_clicked();

    void on_testFileBrowseButton_clicked();

    void on_testButton_clicked();

private:
    Ui::MainWindow *ui;

    QSharedPointer<ClassifierChain> _chain;
};

#endif // MAINWINDOW_H
