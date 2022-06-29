#ifndef FTHREAD_H
#define FTHREAD_H

#include <QThread>
#include <QDebug>
#include <QProcess>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QMessageBox>

class fThread : public QThread
{
   Q_OBJECT

public slots:
    void stopRunning();

    void setPEStat(QString stat) {status = stat;};

    void switchFunctions(int funct) {function = funct;};

    void setFlashOption(int opt) {option = opt;};

protected:
   virtual void run();

signals:
   void update(QString);

private:
    bool isRunning;

    QString status;

    int function;

    int option;

};

#endif // FTHREAD_H
