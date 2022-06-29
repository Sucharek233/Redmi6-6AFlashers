#include "mainwindow.h"
#include "ui_mainwindow.h"

int scene = 0;
int delScene = -1;

QString stylesheet;

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
        flashType.setWindowTitle("Select flash type");
        flashType.setText("Please choose one of the flash types:");
        QAbstractButton* ca = flashType.addButton("Clean all", QMessageBox::YesRole);
        flashType.addButton("Save user data", QMessageBox::NoRole);
        flashType.exec();
        if (flashType.clickedButton() == ca) {qDebug() << "clean all"; fT.setFlashOption(1);
        } else {qDebug() << "save data"; fT.setFlashOption(2);}

        ui->gridLayout_Content->addWidget(lProg, 1, 0);
        fT.switchFunctions(4);
        fT.start();
    } else if (scene == 7) {
        ui->pushButton_Next->setEnabled(false);
        ui->label_Flash->setStyleSheet(stylesheet + "color: green;");
        ui->label_Cleanup->setStyleSheet(stylesheet + "color: cyan;");
        ui->labelMain->setText("Temporary file cleanup");
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
    } else if (scene == 1) {
        delete lText;
        delete lProg;
    } else if (scene == 3) {
        delete lText;
        delete lProg;
    } else if (scene == 4) {
        delete lText;
        delete lProg;
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
    } else {
        lProg->setText(text);
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setInfoLabelText("Hello, and welcome to this automatic flash setup.\n"
                     "You will be walked trough steps that involve downloading, \n"
                     "extracting, preparing and finally flashing.\n"
                     "Things such as a locked bootloader, device mismatch, etc.\n"
                     "will be told to you trough different alerts.\n\n"
                     "This app is free and open source, released on GitHub.\n\n"
                     "Anyways, let's get started.");

    QObject::connect(&fT,SIGNAL(update(const QString&)),SLOT(setDlProgText(const QString&)), Qt::QueuedConnection);

    stylesheet = "border: 2px solid black; ";
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
