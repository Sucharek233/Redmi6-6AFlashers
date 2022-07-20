#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qlabel.h"
#include "fthread.h"
#include "resume.h"
#include <QMainWindow>
#include <QStandardPaths>
#include <QDir>

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

    void msgBoxThread(QString title, QString text, int exec);

    void on_pushButton_Close_clicked();

    void on_pushButton_Resume_clicked();

private:
    Ui::MainWindow *ui;

    fThread fT;
    resume startFrom;

    QLabel *lText;
    QLabel *lProg;
};
#endif // MAINWINDOW_H
