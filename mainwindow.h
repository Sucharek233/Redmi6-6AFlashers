#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qlabel.h"
#include "fthread.h"
#include "resume.h"
#include <iostream>
#include <QMainWindow>
#include <QStandardPaths>
#include <QDir>
#include <QProgressBar>
#include <QRadioButton>
#include <QLineEdit>

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

    void setDlProgText(QString text);

    void msgBoxThread(QString title, QString text, int exec);

    void on_pushButton_Close_clicked();

    void on_pushButton_Resume_clicked();

    void switchScenes(int scene);
    void deleteScenes(int scene);

    void progressBar(int percentage);

    void rAuto_clicked() {
        iROM->setEnabled(false);
        rAuto->setText("Pick ROM automatically\n"
                       "11.0.4.0 for Redmi 6\n"
                       "11.0.8.0 for Redmi 6A");
        rCustom->setText("Paste in your own MIUI version link");
    }
    void rCustom_clicked() {
        iROM->setEnabled(true);
        rCustom->setText("Paste in your own MIUI version link\n"
                         "Warning! Links are NOT checked!\n"
                         "Please make sure you're entering a\n"
                         "correct link for your phone.");
        rAuto->setText("Pick ROM automatically");
    }
    void iROM_text() {link = iROM->text();}

private:
    Ui::MainWindow *ui;

    fThread fT;
    resume startFrom;

    QLabel *lText;
    QLabel *lProg;

    QLabel *spacer;

    QRadioButton *rAuto;
    QRadioButton *rCustom;
    QLineEdit *iROM;
    QString link;
};
#endif // MAINWINDOW_H
