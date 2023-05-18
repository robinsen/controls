#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "dbmanager.h"

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
    void on_pushButton_clicked();
    void slot_db_readyMsg(DataEntityPtr pResp);

private:
    Ui::MainWindow *ui;

    CDBManager*  m_pDBManager = nullptr;
};

#endif // MAINWINDOW_H
