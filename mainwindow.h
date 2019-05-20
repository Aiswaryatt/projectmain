#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "QMessageBox"
#include "QFileDialog"
#include "QPixmap"
#include <QMessageBox>
#include "updatepatches.h"
#include "opencv2/tracking.hpp"
#include <iostream>

using namespace cv;
using namespace std;
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
    void on_Brouse_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
