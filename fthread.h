#ifndef FTHREAD_H
#define FTHREAD_H

#include <QThread>
#include <QDebug>
#include <QProcess>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QTcpSocket>

class fThread : public QThread
{
   Q_OBJECT

public:
    QString getDir() {return dir;}

    void process(QString app, QStringList command, QString path);

public slots:
    void stopRunning();

    void setPEStat(QString stat) {status = stat;};

    void switchFunctions(int funct) {function = funct;};

    void setFlashOption(int opt) {option = opt;};

    void setDriverArch(QString architecture) {arch = architecture;}

    void setROMLink(QString ROM) {romLink = ROM;}

protected:
   virtual void run();

signals:
   void update(QString);

   void msgBox(QString, QString, int);

   void progBar(int);

private:
    bool isRunning;

    QString dir;

    QString status;

    int function;

    int option;

    QString arch = "x64";

    QString romLink = "auto";
};

#endif // FTHREAD_H
