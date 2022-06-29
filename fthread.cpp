#include "fthread.h"

QString dir;

QString getUserPath()
{
    QStringList homePath = QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
    return homePath.first().split(QDir::separator()).last();
}

QString getvar(QString get)
{
    QProcess getVar;
    QStringList command; command << "/C" << "fastboot getvar " + get;
    getVar.setWorkingDirectory(dir);
    getVar.start("cmd", command); getVar.waitForFinished();
    QString output = getVar.readAllStandardError();
    return output.left(output.count() - 30);
}

QString getPercentage(QString path)
{
    QFile get1(path + "out.txt");
    get1.open(QIODevice::ReadOnly);
    QString contents = get1.readAll();
    get1.close();

    QString mContents = contents.replace("\r", "\n");

    QFile get2(path + "mOut.txt");
    get2.open(QIODevice::ReadWrite); get2.resize(0); get2.write(mContents.toUtf8()); get2.close();
    get2.open(QIODevice::ReadOnly);
    QString lastLine;
    while(!get2.atEnd()) {
        lastLine = get2.readLine();
    }
    get2.close();
    QString percentage = lastLine.left(4).simplified();
    if (lastLine == "\n") {percentage = "100";}

    return percentage;
}

void fThread::run()
{
    isRunning = 1;
    while(isRunning == 1)
    {
        dir = getUserPath() + "/AppData/Local/Temp/Sucharek/";
        QStringList command;
        if (function == 1) {
            QDir createFolder; createFolder.mkdir(dir);
            createFolder.mkdir(dir + "/ROM");

            update("Copying files...");
            QFile::copy(":/files/drivers.zip", dir + "drivers.zip");
            QFile::copy(":/files/platform-tools.zip", dir + "platform-tools.zip");

            update("Extracting Platform Tools... (r33.0.2)");
            QProcess extract;
            command << "Expand-Archive" <<  "platform-tools.zip";
            extract.setWorkingDirectory(dir);
            extract.start("powershell", command);
            extract.waitForFinished();
            update("Extracting drivers...");
            command.clear(); command << "Expand-Archive" << "drivers.zip";
            extract.start("powershell", command);
            extract.waitForFinished();

            update("Installing drivers...");
            system(dir.toUtf8() + "drivers/drivers_x64.exe");
            update("Finished\n\n"
                   "Please proceed to the next step, where your\n"
                   "phone will be checked for any mismatches.\n\n"
                   "Note: If driver installation fails, press Win + R on\n"
                   "your keyboard, type \"%temp%/Sucharek/drivers\"\n"
                   "and open the driver installation .exe file to retry.");
            update("enable");
            stopRunning();
        } else if (function == 2) {
            QProcess fd;
            fd.setWorkingDirectory(dir + "platform-tools");
            command.clear(); command << "/C" << "fastboot devices";
            for (int i = 1; i < 101; i++) {
                update("Detecting device... (tries: " + QString::number(i) + "/100)");
                sleep(1);
                fd.start("cmd", command); fd.waitForFinished();
                QString output = fd.readAll();
                if (output != "") {
                    QString textToDisplay = "Device detected\n"
                                            "Serial number: " + output.left(output.count() - 10) + "\n";
                    update(textToDisplay);

                    output = getvar("unlocked");
                    textToDisplay += output;
                    if (output == "unlocked: yes\r\n") {
                        textToDisplay += "Bootloader unlocked, PASS.\n"; update("enable");
                    } else if (output == "unlocked: no\r\n") {
                        textToDisplay += "Bootloader locked, cannot continue, FAIL.\n";
                    } else {
                        textToDisplay += "Could not get bootloader info, continuing anyway.\n"; update("enable");
                    }
                    update(textToDisplay);

                    output = getvar("product");
                    textToDisplay += output;
                    if (output == "product: cereus\r\n") { //for 6A, replace cereus with cactus
                        textToDisplay += "Device name (codename) verified, PASS.\n"; update("enable");
                    } else if (output == "product: \r\n") {
                        textToDisplay += "Could not get device codename, FAIL.";
                    } else {
                        textToDisplay += "Device mismatch. This is not theselectedphone, FAIL.";
                    }
                    update(textToDisplay);

                    break;
                } else {
                    update("Could not detect device.\n"
                           "Cannot continue.");
                }
            }
            stopRunning();
        } else if (function == 3) {
            QString dlProg;
            QString extProg;

            QProcess getROMReady;
            QString romLink = "https://bigota.d.miui.com/V11.0.4.0.PCGMIXM/cereus_global_images_V11.0.4.0.PCGMIXM_20200527.0000.00_9.0_global_f6d253e00b.tgz";
            //For 6A: https://bigota.d.miui.com/V11.0.8.0.PCBMIXM/cactus_global_images_V11.0.8.0.PCBMIXM_20200509.0000.00_9.0_global_5fe1e27073.tgz
            command.clear(); command << "/C" << "curl " + romLink + " --output " + dir + "ROM/ROM.tgz 2>" + dir + "ROM/out.txt";
            getROMReady.startDetached("cmd", command);
            int avoidComplete = 0;
            while(true) {
                QString percentage = getPercentage(dir + "ROM/");
                dlProg = "Downloading ROM... (" + percentage + "%)"; update(dlProg);

                avoidComplete += 1;
                if (avoidComplete > 40) {if (percentage == "100") {break;}}

                msleep(100);
            }
            dlProg = "ROM downloaded\n\n"; update(dlProg);

            getROMReady.setWorkingDirectory(dir + "ROM");
            command.clear(); command << "tar zxvf" << "ROM.tgz";
            extProg = "Extracting ROM files... "; update(dlProg + extProg);
            getROMReady.start("powershell", command); getROMReady.waitForFinished();
            extProg += "Done\nDeleting leftover files... "; update(dlProg + extProg);
            QFile::remove(dir + "ROM/ROM.tgz"); QFile::remove(dir + "ROM/out.txt"); QFile::remove(dir + "ROM/mOut.txt");
            extProg += "Done\nRenaming folder to a reasonable name... "; update(dlProg + extProg);
            QDir rename; rename.rename(dir + "ROM/ido_xhdpi_global_images_V8.5.2.0.LAIMIED_20170824.0000.00_5.1_global", dir + "ROM/ROM");
            extProg += "Done\n\n";

            update(dlProg + extProg + "Finished");

            update("enable");
            stopRunning();
        } else if (function == 4) {
            QProcess checkConnection;
            checkConnection.setWorkingDirectory(dir + "platform-tools");
            command.clear(); command << "/C" << "fastboot devices";
            for (int i = 1; i < 101; i++) {
                update("Detecting device... (tries: " + QString::number(i) + "/100)"); sleep(1);
                checkConnection.start("cmd", command); checkConnection.waitForFinished();
                QString output = checkConnection.readAll();
                if (output != "") {
                    update("Device connection verified.\n"
                           "A cmd will open, where you will be\n"
                           "able to see the progress\n"
                           "Starting flash in 10 seconds...");
                    sleep(10);
                    break;
                }
            }
            QFile::copy(dir + "platform-tools/fastboot.exe", dir + "ROM/ROM/fastboot.exe");

            update("Flashing...");
            if (option == 1) {
                system("cmd /C \"" + dir.toUtf8() + "ROM/ROM/flash_all.bat && "
                                                    "echo Warning! This is NOT logged. If you received any errors, please copy this whole output. && "
                                                    "echo Finished && pause\"");
            } else if (option == 2) {
                system("cmd /C \"" + dir.toUtf8() + "ROM/ROM/flash_all_except_data_storage.bat && "
                                                    "echo Warning! This is NOT logged. If you received any errors, please copy this whole output. && "
                                                    "echo Finished && pause\"");
            }

            update("enable");
            update("Flashing finished!\n"
                   "If you want to clean temporary files,\n"
                   "close this window now.\n"
                   "If you want to clean them,\n"
                   "click on the next button.\n\n"
                   "You should have your phone flashed now.\n"
                   "Enjoy!");
            stopRunning();
        } else if (function == 5) {
            update("Deleting temporary files...");
            QDir remove(dir); remove.removeRecursively();
            update("green");
            update("Temporary files deleted.\n"
                   "You can close this window now.\n\n"
                   "Thank you for using my program.");
            stopRunning();
        }
    }
}

void fThread::stopRunning()
{
    isRunning = 0;
}
