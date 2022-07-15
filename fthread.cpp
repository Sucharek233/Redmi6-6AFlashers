#include "fthread.h"

QString codename;

QString getUserPath()
{
    QStringList homePath = QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
    return homePath.first().split(QDir::separator()).last();
}

QString getvar(QString get)
{
    QProcess getVar;
    QStringList command; command << "/C" << "fastboot getvar " + get;
    getVar.setWorkingDirectory(getUserPath() + "/AppData/Local/Temp/Sucharek/");
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

void fThread::process(QString app, QStringList command, QString path)
{
    QProcess process;
    process.setWorkingDirectory(path);
    process.start(app, command);
    process.waitForFinished();
}

QString getROMFolderName(QString path)
{
    QDir search(path);
    QStringList dirs = search.entryList(QDir::NoDotAndDotDot | QDir::Dirs);
    QString name;
    foreach(QString folder, dirs) {
        if (folder.count() < 30) {} else {name = folder; break;}
    }
    return name;
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

            QString updateText;
            updateText = "Copying files... "; update(updateText);
            QFile::copy(":/files/drivers.zip", dir + "drivers.zip");
            QFile::copy(":/files/platform-tools.zip", dir + "platform-tools.zip");

            updateText += "Done\nExtracting Platform Tools... (r33.0.2)... "; update(updateText);
            process("powershell", QStringList() << "Expand-Archive" <<  "platform-tools.zip", dir);

            updateText += "Done\nExtracting drivers... "; update(updateText);
            process("powershell", QStringList() << "Expand-Archive" <<  "drivers.zip", dir);
            updateText += "Done\nRetrieving codename info... "; //NOT DONE YET!!!!!!!!!!!!

            int errorCheck = 0;
            if (arch == "x64") {
                update("Installing drivers...");
                errorCheck = system(dir.toUtf8() + "drivers/drivers_x64.exe");
            } else if (arch == "x86") {
                update("Installing 32-bit (x86) drivers...");
                errorCheck = system(dir.toUtf8() + "drivers/drivers_x86.exe");
            }
            if (errorCheck == 256) {
                update("Finished\n\n"
                       "Please proceed to the next step, where your\n"
                       "phone will be checked for any mismatches.\n\n");
                update("enable");
            } else {
                update("Driver installation failed.");
                QString reason = "Reason: ";
                if (errorCheck == 1) {reason += "Access denied or file not found.<br>"
                                                "Solution: Retry and allow administrative rights.";}
                else if (errorCheck == -2147483648) {reason += "Installation canceled or wrong architecture.<br>"
                                                               "Solution: Don't cancel or close the driver installation window or try x86 drivers.";}
                else {reason += "Unknown reason.<br>"
                                "Solution: Unknown solution.";}
                msgBox("Driver install error", "Drivers failed to install!<br>" +
                                               reason +
                                               "<br>Error code: " + QString::number(errorCheck) + "<br><br>", 0);
            }
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

                    codename = getvar("product");
                    textToDisplay += codename;
                    if (codename == "product: cereus\r\n") {
                        textToDisplay += "Device codename verified, PASS.\nFlashing ROM for for Redmi 6.\n"; update("enable");
                    } else if (codename == "product: cactus\r\n") {
                        textToDisplay += "Device codename verified, PASS.\nFlashing ROM for for Redmi 6A.\n"; update("enable");
                    } else if (codename == "product: \r\n") {
                        textToDisplay += "Could not get device codename, FAIL.";
                    } else {
                        textToDisplay += "Device mismatch. This is not Redmi 6/6A, FAIL.";
                    }
                    update(textToDisplay);

                    break;
                } else {
                    if (i > 99) {
                    update("Could not detect device.\n"
                           "Cannot continue.");
                    msgBox("Could not detect device", "do the rest :)", 0);
                    }
                }
            }
            stopRunning();
        } else if (function == 3) {
            QString checkInternet;
            QString dlProg;
            QString extProg;

            int internetBreak = 0;
            checkInternet = "Checking internet connection... (can take up to 15 seconds)"; update(checkInternet);
            QTcpSocket check;
            check.connectToHost("www.google.com", 80);
            bool connected = check.waitForConnected();
            if (connected == true) {
                checkInternet = "Checking internet connection... Online\n"; update(checkInternet);
            } else {
                checkInternet = "Checking internet connection... Offline\n"
                                 "Cannot continue.";
                update(checkInternet);
                msgBox("No internet", "You have no internet access.\n"
                                      "Please check your internet connection and click the Retry button.\n\n"
                                      "Here's a list of things you can try:\n"
                                      "Repluging your ethernet cable.\n"
                                      "Turning WiFi off and on.\n"
                                      "Restarting your router.\n"
                                      "Restarting your PC.", 0);
                internetBreak = 1;
            }

            if (internetBreak == 0) {
            QProcess getROMReady;
            QString romLink;
            if (codename == "product: cereus\r\n") {
                romLink = "https://bigota.d.miui.com/V11.0.4.0.PCGMIXM/cereus_global_images_V11.0.4.0.PCGMIXM_20200527.0000.00_9.0_global_f6d253e00b.tgz";
            } else if (codename == "product: cactus\r\n") {
                romLink = "https://bigota.d.miui.com/V11.0.8.0.PCBMIXM/cactus_global_images_V11.0.8.0.PCBMIXM_20200509.0000.00_9.0_global_5fe1e27073.tgz";
            } else {
                msgBox("How???", "Ok, how are you even here?\n"
                                 "Did my codename check fail?\n"
                                 "Do you have the correct codename and this doesn't work?\n"
                                 "Please open an issue on my gihub page if you got here with a wrong codename or correct one and this message displayed.\n\n"
                                 "Thank you.", 1);
            }
            command.clear(); command << "/C" << "curl " + romLink + " --output " + dir + "ROM/ROM.tgz 2>" + dir + "ROM/out.txt";
            getROMReady.startDetached("cmd", command);
            int avoidComplete = 0;
            while(true) {
                QString percentage = getPercentage(dir + "ROM/");
                dlProg = "Downloading ROM... (" + percentage + "%)"; update(checkInternet + dlProg);

                avoidComplete += 1;
                if (avoidComplete > 40) {if (percentage == "100") {break;}}

                msleep(100);
            }
            dlProg = "ROM downloaded\n\n"; update(checkInternet + dlProg);

            extProg = "Extracting ROM files... "; update(checkInternet + dlProg + extProg);
            process("powershell", QStringList() << "tar zxvf" << "ROM.tgz", dir + "ROM");
            extProg += "Done\nDeleting leftover files... "; update(checkInternet + dlProg + extProg);
            QFile::remove(dir + "ROM/ROM.tgz"); QFile::remove(dir + "ROM/out.txt"); QFile::remove(dir + "ROM/mOut.txt");
            extProg += "Done\nRenaming folder to a reasonable name... "; update(checkInternet + dlProg + extProg);
            QDir rename; rename.rename(dir + "ROM/" + getROMFolderName(dir + "ROM"), dir + "ROM/ROM");
            extProg += "Done\n\n";

            update(checkInternet + dlProg + extProg + "Finished");
            update("enable");
            }
            stopRunning();
        } else if (function == 4) {
            QProcess checkConnection;
            checkConnection.setWorkingDirectory(dir + "platform-tools");
            command.clear(); command << "/C" << "fastboot devices";
            int connectionBreak = 0;
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
                } else {
                    if (i > 99) {
                        update("Could not detect device.\n"
                               "Cannot continue.");
                        msgBox("Could not detect device", "do the rest :)", 0);
                        connectionBreak = 1;
                    }
                }
            }
            if (connectionBreak == 0) {
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
                   "If you don't want to clean temporary files,\n"
                   "close this window now.\n"
                   "If you want to clean them,\n"
                   "click on the next button.\n\n"
                   "You should have your phone flashed now.\n"
                   "Enjoy!");
            }
            stopRunning();
        } else if (function == 5) {
            update("Deleting temporary files...");
            QDir remove(dir); remove.removeRecursively();
            update("green");
            update("Temporary files deleted.\n"
                   "You can close this window now.\n\n"
                   "Thank you for using my program.");
            update("close");
            stopRunning();
        }
    }
}

void fThread::stopRunning()
{
    isRunning = 0;
}
