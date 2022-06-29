#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qlabel.h"
#include "fthread.h"
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_Next_clicked();

    void setInfoLabelText(QString text);

    void switchScenes(int scene);
    void deleteScenes(int scene);

    void setDlProgText(QString text);

private:
    Ui::MainWindow *ui;

    fThread fT;

    QLabel *lText;
    QLabel *lProg;
};
#endif // MAINWINDOW_H
