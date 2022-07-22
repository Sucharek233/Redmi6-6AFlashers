#include "mainwindow.h"
#include "ui_mainwindow.h"

int scene = 0;
int delScene = -1;

QString stylesheet;
QString msgBoxStylesheet;

QString dir;

QProgressBar* progBar;
int crashStop;
int createBar;

void MainWindow::setInfoLabelText(QString text)
{
    QSizePolicy labelSize;
    QFont size;

    lText = new QLabel;
    lText->setText(text);
    labelSize.setHorizontalPolicy(QSizePolicy::Expanding);
    labelSize.setVerticalPolicy(QSizePolicy::Expanding);
    lText->setSizePolicy(labelSize);
    lText->setAlignment(Qt::AlignTop);
    size.setPointSize(14);
    lText->setFont(size);
    ui->gridLayout_Content->addWidget(lText, 0, 0);

    //additional setup
    lProg = new QLabel;
    lProg->setSizePolicy(labelSize);
    lProg->setAlignment(Qt::AlignTop);
    size.setPointSize(14);
    lProg->setFont(size);
}

void MainWindow::switchScenes(int scene)
{
    if (scene == 1) {
        ui->pushButton_Next->setEnabled(false);
        ui->label_Intro->setStyleSheet(stylesheet + "color: green;");
        ui->label_Prep->setStyleSheet(stylesheet + "color: cyan;");
        ui->labelMain->setText("Preparation");
        setInfoLabelText("Now I will begin the process of installing the\n"
                         "necessary (pre-downloaded) programs to do security\n"
                         "checks to make sure you're ready for the flash.\n"
                         "This shouldn't take long.\n\n\n");
        ui->gridLayout_Content->addWidget(lProg, 1, 0);
        fT.switchFunctions(1);
        fT.start();
    } else if (scene == 2) {
        ui->label_Prep->setStyleSheet(stylesheet + "color: green;");
        ui->label_Check->setStyleSheet(stylesheet + "color: cyan;");
        ui->labelMain->setText("Check");
        setInfoLabelText("The check will now begin.\n"
                         "Please connect your phone in fastboot mode.\n"
                         "You can do that by holding\n"
                         "power and volume down buttons.\n"
                         "After you done that, plug in your phone and\n"
                         "click the Next button.");
    } else if (scene == 3) {
        ui->pushButton_Next->setEnabled(false);
        fT.switchFunctions(2);
        ui->gridLayout_Content->addWidget(lProg, 1, 0);
        fT.start();
    } else if (scene == 4) {
        ui->label_Check->setStyleSheet(stylesheet + "color: green;");
        ui->label_Download->setStyleSheet(stylesheet + "color: cyan;");
        ui->labelMain->setText("ROM download");
        setInfoLabelText("Now the ROM will download.\n"
                         "You can restart your device by "
                         "holding the power button.\n"
                         "After it downloads, please put your phone back \n"
                         "in fastboot and click on the Next button.\n");
        ui->pushButton_Next->setEnabled(false);
        ui->gridLayout_Content->addWidget(lProg, 1, 0);
        fT.switchFunctions(3);
        fT.start();
    } else if (scene == 5) {
        ui->label_Download->setStyleSheet(stylesheet + "color: green;");
        ui->label_Flash->setStyleSheet(stylesheet + "color: cyan;");
        ui->labelMain->setText("Flash");
        setInfoLabelText("Finally, you are ready to flash.\n"
                         "Before I'll start flashing, there will be\n"
                         "one last final check, just to make sure everything\n"
                         "is ready to go.\n"
                         "Make sure your phone is plugged in, and click on the\n"
                         "next button.");
    } else if (scene == 6) {
        ui->pushButton_Next->setEnabled(false);

        QMessageBox flashType;
        flashType.setStyleSheet(msgBoxStylesheet);
        flashType.setWindowTitle("Select flash type");
        flashType.setText("Please choose one of the flash types:");
        QAbstractButton* ca = flashType.addButton("Clean all", QMessageBox::YesRole);
        flashType.addButton("Save user data", QMessageBox::NoRole);
        flashType.exec();
        if (flashType.clickedButton() == ca) {fT.setFlashOption(1);
        } else {fT.setFlashOption(2);}

        ui->gridLayout_Content->addWidget(lProg, 1, 0);
        fT.switchFunctions(4);
        fT.start();
    } else if (scene == 7) {
        ui->pushButton_Next->setEnabled(false);
        ui->label_Flash->setStyleSheet(stylesheet + "color: green;");
        ui->label_Cleanup->setStyleSheet(stylesheet + "color: cyan;");
        ui->labelMain->setText("Temporary files cleanup");
        setInfoLabelText("Files used for the flash will now delete.\n"
                         "After it's done, please close this window.");
        ui->gridLayout_Content->addWidget(lProg, 1, 0);
        fT.switchFunctions(5);
        fT.start();
    }
}
void MainWindow::deleteScenes(int scene)
{
    if (scene == 0) {
        delete lText;
        ui->pushButton_Resume->setVisible(false);
    } else if (scene == 1) {
        delete lText;
        delete lProg;
    } else if (scene == 3) {
        delete lText;
        delete lProg;
    } else if (scene == 4) {
        delete lText;
        delete lProg;
        delete progBar;
    } else if (scene == 6) {
        delete lText;
        delete lProg;
    }
}

void MainWindow::setDlProgText(QString text)
{
    if (text == "enable") {
        ui->pushButton_Next->setEnabled(true);
    } else if (text == "green") {
        ui->label_Cleanup->setStyleSheet(stylesheet + "color: green;");
    } else if (text == "close") {
        ui->pushButton_Next->setVisible(false);
        ui->pushButton_Close->setVisible(true);
    } else {
        lProg->setText(text);
    }
}


void MainWindow::msgBoxThread(QString title, QString text, int exec)
{
    QMessageBox msgBox;
    msgBox.setWindowTitle(title);
    msgBox.setText(text);
    msgBox.setStyleSheet(msgBoxStylesheet);
    if (exec == 1) {msgBox.exec();}
    if (title == "Driver install error") {
        text += "Please click Retry to retry the installation.<br>"
                "If you already have the drivers installed, click Ignore.<br>"
                "If you have a 32-bit system, click Install x86 drivers.<br>"
                "If this message keeps showing up, click Open driver folder and try opening both .exe files.<br><br>"
                "If none of these worked for you, please report the error code "
                "on my <a href=\"https://github.com/Sucharek233/Redmi6-6AFlashers/issues\">Github repository</a> (open an issue there).";
        msgBox.setText(text);
        msgBox.setTextFormat(Qt::RichText);
        QPushButton* retry = msgBox.addButton("Retry", QMessageBox::YesRole);
        QPushButton* x86 = msgBox.addButton("Install x86 drivers", QMessageBox::YesRole);
        QPushButton* open = msgBox.addButton("Open driver folder", QMessageBox::YesRole);
        QPushButton* ignore = msgBox.addButton("Ignore", QMessageBox::NoRole);
        msgBox.exec();
        if (msgBox.clickedButton() == retry) {
            fT.setDriverArch("x64");
            fT.start();
        } else if (msgBox.clickedButton() == ignore) {
            setDlProgText("Driver installation failed, ignored.\n"
                          "Please click on the Next button to continue.");
            ui->pushButton_Next->setEnabled(true);
        } else if (msgBox.clickedButton() == x86) {
            fT.setDriverArch("x86");
            fT.start();
        } else if (msgBox.clickedButton() == open) {
            QString dir = fT.getDir() + "drivers"; dir.replace("/", "\\");
            system("explorer " + dir.toUtf8());
            QMessageBox msgBox;
            msgBox.setWindowTitle("Installing drivers on your own");
            msgBox.setText("An explorer window should be now opened on your screen.<br>"
                           "Please open \"drivers_x64.exe\" or \"drivers_x86.exe\" to install these drivers.<br>"
                           "If you can't install any of these, please open an issue on my "
                           "<a href=\"https://github.com/Sucharek233/Redmi6-6AFlashers/issues\">Github repository</a> "
                           "and tell me, what is the installer doing, if you want, record a video or take screenshots of it.");
            msgBox.setStyleSheet(msgBoxStylesheet);
            msgBox.setTextFormat(Qt::RichText);
            QPushButton* success = msgBox.addButton("Installation succeeded", QMessageBox::YesRole);
            QPushButton* fail = msgBox.addButton("Installation failed", QMessageBox::NoRole);
            msgBox.exec();
            if (msgBox.clickedButton() == success) {
                setDlProgText("Finished\n\n"
                              "Please proceed to the next step, where your\n"
                              "phone will be checked for any mismatches.");
                ui->pushButton_Next->setEnabled(true);
            } else if (msgBox.clickedButton() == fail) {
                setDlProgText("Drivers not installed, cannot continue.");
                ui->pushButton_Next->setVisible(false);
                ui->pushButton_Close->setVisible(true);
            }
        }
    } else if (title == "Could not detect device") {
        QPushButton* retry = msgBox.addButton("Retry", QMessageBox::YesRole);
        QPushButton* cancel = msgBox.addButton("Cancel", QMessageBox::NoRole);
        msgBox.setText("Device cannot be detected\n"
                       "Please make sure your phone is in fastboot mode and connected into your PC.\n\n"
                       "If this issue persists, here's a list of things you can try:\n"
                       "Restarting your computer.\n"
                       "Using a different cable.\n"
                       "Trying a different port on your PC or laptop.\n"
                       "Using a different PC or a laptop.");
        msgBox.exec();
        if (msgBox.clickedButton() == retry) {
            fT.start();
        } else if (msgBox.clickedButton() == cancel) {
            ui->pushButton_Next->setVisible(false);
            ui->pushButton_Close->setVisible(true);
        }
    } else if (title == "No internet") {
        QPushButton* retry = msgBox.addButton("Retry", QMessageBox::YesRole);
        QPushButton* cancel = msgBox.addButton("Cancel", QMessageBox::NoRole);
        msgBox.exec();
        if (msgBox.clickedButton() == retry) {
            fT.start();
        } else if (msgBox.clickedButton() == cancel) {
            ui->pushButton_Next->setVisible(false);
            ui->pushButton_Close->setVisible(true);
        }
    }
}

void MainWindow::progressBar(int percentage)
{
    if (percentage >= 0) {
        ui->gridLayout_Content->addWidget(progBar);
        progBar->setValue(percentage);
        progBar->update();
        crashStop += 1;
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->pushButton_Close->setVisible(false);

    setInfoLabelText("Hello, and welcome to this automatic flash setup.\n"
                     "You will be walked trough steps that involve downloading, \n"
                     "extracting, preparing and finally flashing.\n"
                     "Things such as a locked bootloader, device mismatch, etc.\n"
                     "will be told to you trough different alerts.\n\n"
                     "This app is free and open source, released on GitHub.\n\n"
                     "Anyways, let's get started.");

    progBar = new QProgressBar;
    progBar->setRange(0, 100);
    progBar->setTextVisible(false);

    QObject::connect(&fT,SIGNAL(update(const QString&)),SLOT(setDlProgText(const QString&)), Qt::AutoConnection);
    QObject::connect(&fT,SIGNAL(msgBox(const QString&, const QString&, const int&)),SLOT(msgBoxThread(const QString&, const QString&, const int&)), Qt::AutoConnection);
    QObject::connect(&fT,SIGNAL(progBar(const int&)),SLOT(progressBar(const int&)), Qt::AutoConnection);

    stylesheet = "border: 2px solid black; ";

    msgBoxStylesheet = "QMessageBox QLabel {font-size: 20px;} "
                       "QMessageBox QPushButton {font-size: 16px;}";
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_Next_clicked()
{
    scene += 1;
    delScene += 1;
    deleteScenes(delScene);
    switchScenes(scene);
}

void MainWindow::on_pushButton_Close_clicked()
{
    close();
}

void MainWindow::on_pushButton_Resume_clicked()
{
    QStringList homePath = QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
    QString mainPath = homePath.first().split(QDir::separator()).last() + "/AppData/Local/Temp/Sucharek/";
    int isGood = 0;
    QDir check(mainPath);
    if (check.exists()) {
        isGood += 1;
        check.setPath(mainPath + "platform-tools");
        if (check.exists()) {
            isGood += 1;
        }
    }
    if (isGood > 1) {
    if (startFrom.exec() == QDialog::Accepted) {
        ui->pushButton_Resume->setVisible(false);

        QString option = startFrom.getOption();
        if (option == "prep") {
            ui->pushButton_Next->click();
        } else if (option == "check") {
            ui->label_Intro->setStyleSheet(stylesheet + "color: green;");
            scene += 1; delScene += 1;
            ui->pushButton_Next->click();
        } else if (option == "dl") {
            ui->label_Intro->setStyleSheet(stylesheet + "color: green;");
            ui->label_Prep->setStyleSheet(stylesheet + "color: green;");
            ui->label_Check->setStyleSheet(stylesheet + "color: green;");
            scene += 3; delScene += 3;
            ui->pushButton_Next->click();
        } else if (option == "flash") {
            ui->label_Intro->setStyleSheet(stylesheet + "color: green;");
            ui->label_Prep->setStyleSheet(stylesheet + "color: green;");
            ui->label_Check->setStyleSheet(stylesheet + "color: green;");
            ui->label_Download->setStyleSheet(stylesheet + "color: green;");
            scene += 4; delScene += 4;
            ui->pushButton_Next->click();
        } else if (option == "cleanup") {
            ui->label_Intro->setStyleSheet(stylesheet + "color: green;");
            ui->label_Prep->setStyleSheet(stylesheet + "color: green;");
            ui->label_Check->setStyleSheet(stylesheet + "color: green;");
            ui->label_Download->setStyleSheet(stylesheet + "color: green;");
            scene += 6; delScene += 6;
            ui->pushButton_Next->click();
        }
    }
    } else {
        QMessageBox notFound;
        notFound.setWindowTitle("Cannot resume");
        notFound.setText("Directory not found.\n"
                         "Cannot load important files.\n\n"
                         "Cannot resume from action.");
        notFound.setStyleSheet(msgBoxStylesheet);
        notFound.exec();
    }
}
