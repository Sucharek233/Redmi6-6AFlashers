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
    QStringList command; command << "/C" << "fastboot.exe getvar " + get;
    getVar.setWorkingDirectory(getUserPath() + "/AppData/Local/Temp/Sucharek/platform-tools");
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

    if (percentage.contains("%")) {
        QString fix = percentage.left(percentage.count() - 1);
        percentage = fix;
    }
    return percentage;
}

void fThread::process(QString app, QStringList command, QString path)
{
    QProcess process;
    process.setWorkingDirectory(path);
    process.start(app, command);
    process.waitForFinished(18000000);
}

QString getROMFolderName(QString path)
{
    QDir search(path);
    QStringList dirs = search.entryList(QDir::NoDotAndDotDot | QDir::Dirs);
    QString name;
    foreach(QString folder, dirs) {name = folder; break;}
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
            updateText = "Copying files... "; emit update(updateText);
            QFile::copy(":/files/drivers.zip", dir + "drivers.zip");
            QFile::copy(":/files/platform-tools.zip", dir + "platform-tools.zip");
            QFile::copy(":/files/7z.exe", dir + "7z.exe");
            QFile::copy(":/files/curl.exe", dir + "curl.exe");

            updateText += "Done\nExtracting Platform Tools (r33.0.2)... "; emit update(updateText);
            process("cmd", QStringList() << "/C" << "7z.exe" << "x" <<  "platform-tools.zip" << "-oplatform-tools" << "-y", dir);

            updateText += "Done\nExtracting drivers... "; emit update(updateText);
            process("cmd", QStringList() << "/C" << "7z.exe" << "x" <<  "drivers.zip" << "-odrivers" << "-y", dir);
            updateText += "Done\nRetrieving codename info... "; //NOT DONE YET!!!!!!!!!!!!

            int errorCheck = 0;
            if (arch == "x64") {
                emit update("Installing drivers...");
                errorCheck = system(dir.toUtf8() + "drivers/drivers_x64.exe");
            } else if (arch == "x86") {
                emit update("Installing 32-bit (x86) drivers...");
                errorCheck = system(dir.toUtf8() + "drivers/drivers_x86.exe");
            }
            if (errorCheck == 256) {
                emit update("Finished\n\n"
                       "Please proceed to the next step, where your\n"
                       "phone will be checked for any mismatches.\n\n");
                emit update("enable");
            } else {
                emit update("Driver installation failed.");
                QString reason = "Reason: ";
                if (errorCheck == 1) {reason += "Access denied or file not found.<br>"
                                                "Solution: Retry and allow administrative rights.";}
                else if (errorCheck == -2147483648) {reason += "Installation canceled or wrong architecture.<br>"
                                                               "Solution: Don't cancel or close the driver installation window or try x86 drivers.";}
                else {reason += "Unknown reason.<br>"
                                "Solution: Unknown solution.";}
                emit msgBox("Driver install error", "Drivers failed to install!<br>" +
                                                    reason +
                                                    "<br>Error code: " + QString::number(errorCheck) + "<br><br>", 0);
            }
            stopRunning();
        } else if (function == 2) {
            QProcess fd;
            fd.setWorkingDirectory(dir + "platform-tools");
            command.clear(); command << "/C" << "fastboot.exe devices";
            for (int i = 1; i < 101; i++) {
                emit update("Detecting device... (tries: " + QString::number(i) + "/100)");
                sleep(1);
                fd.start("cmd", command); fd.waitForFinished();
                QString output = fd.readAll();
                if (output != "") {
                    QString textToDisplay = "Device detected\n"
                                            "Serial number: " + output.left(output.count() - 10) + "\n";
                    emit update(textToDisplay);

                    int enableNext = 0;

                    output = getvar("unlocked");
                    textToDisplay += output;
                    if (output == "unlocked: yes\r\n") {
                        textToDisplay += "Bootloader unlocked, PASS.\n"; enableNext += 1;
                    } else if (output == "unlocked: no\r\n") {
                        textToDisplay += "Bootloader locked, cannot continue, FAIL.\n";
                    } else {
                        textToDisplay += "Could not get bootloader info, continuing anyway.\n"; enableNext += 1;
                    }
                    emit update(textToDisplay);

                    codename = getvar("product");
                    textToDisplay += codename;
                    if (codename == "product: cereus\r\n") {
                        textToDisplay += "Device codename verified, PASS.\n\nFlashing ROM for for Redmi 6."; enableNext += 1;
                    } else if (codename == "product: cactus\r\n") {
                        textToDisplay += "Device codename verified, PASS.\n\nFlashing ROM for for Redmi 6A."; enableNext += 1;
                    } else if (codename == "product: \r\n") {
                        textToDisplay += "Could not get device codename, FAIL.\n\n"
                                         "Cannon continue.";
                    } else {
                        textToDisplay += "Device mismatch. This is not Redmi 6/6A, FAIL.\n\n"
                                         "Cannot continue";
                    }
                    if (enableNext > 1) {emit update("enable");} else {emit update("close");}
                    emit update(textToDisplay);

                    break;
                } else {
                    if (i > 99) {
                    emit update("Could not detect device.\n"
                                "Cannot continue.");
                    emit msgBox("Could not detect device", "do the rest :)", 0);
                    }
                }
            }
            stopRunning();
        } else if (function == 3) {
            QString checkInternet;
            QString dlProg;
            QString extProg;

            int internetBreak = 0;
            checkInternet = "Checking internet connection... (can take up to 15 seconds)"; emit update(checkInternet);
            QTcpSocket check;
            check.connectToHost("www.google.com", 80);
            bool connected = check.waitForConnected();
            if (connected == true) {
                checkInternet = "Checking internet connection... Online\n"; emit update(checkInternet);
            } else {
                checkInternet = "Checking internet connection... Offline\n"
                                 "Cannot continue.";
                emit update(checkInternet);
                emit msgBox("No internet", "You have no internet access.\n"
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
            if (romLink == "auto") {
                if (codename == "product: cereus\r\n") {
                    romLink = "https://bigota.d.miui.com/V11.0.4.0.PCGMIXM/cereus_global_images_V11.0.4.0.PCGMIXM_20200527.0000.00_9.0_global_f6d253e00b.tgz";
                } else if (codename == "product: cactus\r\n") {
                    romLink = "https://bigota.d.miui.com/V11.0.8.0.PCBMIXM/cactus_global_images_V11.0.8.0.PCBMIXM_20200509.0000.00_9.0_global_5fe1e27073.tgz";
                } else {
                    emit msgBox("How???", "Ok, how are you even here?\n"
                                          "Did my codename check fail?\n"
                                          "Do you have the correct codename and this doesn't work?\n"
                                          "Please open an issue on my gihub page if you got here with a wrong codename or correct one and this message displayed.\n\n"
                                          "Thank you.", 1);
                }
            }
            command.clear(); command << "/C" << dir + "curl.exe " + romLink + " --output " + dir + "ROM/ROM.tgz --insecure 2>" + dir + "ROM/out.txt";
            getROMReady.startDetached("cmd", command);
            emit progBar(-1);
            int avoidComplete = 0;
            while(true) {
                QString percentage = getPercentage(dir + "ROM/");
                dlProg = "Downloading ROM... (" + percentage + "%)"; emit update(checkInternet + dlProg);
                emit progBar(percentage.toInt());

                avoidComplete += 1;
                if (avoidComplete > 40) {if (percentage == "100") {sleep(2); break;}}

                msleep(100);
            }
            dlProg = "ROM downloaded\n\n"; emit update(checkInternet + dlProg); emit progBar(0);

            extProg = "Extracting ROM files... (0%, 1/2)"; emit update(checkInternet + dlProg + extProg);
            QProcess extract; command.clear();
            command <<  "/C" << dir + "7z.exe" << "e" << dir + "ROM/ROM.tgz" << "-o" + dir + "ROM/" << "-y" << "-bsp1" << ">" + dir + "ROM/out.txt";
            extract.startDetached("cmd", command);
            avoidComplete = 0;
            while(true) {
                QString percentage = getPercentage(dir + "ROM/");
                int overallPercentage = percentage.toInt() / 2;
                extProg = "Extracting ROM files... (" + QString::number(overallPercentage) + "%, 1/2)"; emit update(checkInternet + dlProg + extProg);
                emit progBar(overallPercentage);

                avoidComplete += 1;
                if (avoidComplete > 40) {if (percentage == "100") {break;}}

                msleep(100);
            }
            extProg = "Extracting ROM files... (2/2)"; emit update(checkInternet + dlProg + extProg);
            emit progBar(50);
            command.clear(); command << "/C" << dir + "7z.exe" << "x" << dir + "ROM/ROM.tar" << "-o" + dir + "ROM/" << "-y" << "-bsp1" << ">" + dir + "ROM/out.txt";
            extract.startDetached("cmd", command);
            avoidComplete = 0;
            while(true) {
                QString percentage = getPercentage(dir + "ROM/");
                int overallPercentage = 50 + (percentage.toInt() / 2);
                extProg = "Extracting ROM files... (" + QString::number(overallPercentage) + "%, 2/2)"; emit update(checkInternet + dlProg + extProg);
                emit progBar(overallPercentage);

                avoidComplete += 1;
                if (avoidComplete > 40) {if (percentage == "100") {sleep(2); break;}}

                msleep(100);
            }
            extProg = "Extracting ROM files... Done\nDeleting leftover files... "; emit update(checkInternet + dlProg + extProg);
            emit progBar(0);
            QFile::remove(dir + "ROM/ROM.tgz"); QFile::remove(dir + "ROM/ROM.tar"); QFile::remove(dir + "ROM/out.txt"); QFile::remove(dir + "ROM/mOut.txt");
            emit progBar(50);
            extProg += "Done\nRenaming folder to a reasonable name... "; emit update(checkInternet + dlProg + extProg);
            emit progBar(75);
            QDir rename; rename.rename(dir + "ROM/" + getROMFolderName(dir + "ROM"), dir + "ROM/ROM");
            emit progBar(100);
            extProg += "Done\n\n";
            sleep(2);
            emit progBar(500);

            emit update(checkInternet + dlProg + extProg + "Finished");
            emit update("enable");
            }
            stopRunning();
        } else if (function == 4) {
            QProcess checkConnection;
            checkConnection.setWorkingDirectory(dir + "platform-tools");
            command.clear(); command << "/C" << "fastboot.exe devices";
            int connectionBreak = 0;
            for (int i = 1; i < 101; i++) {
                emit update("Detecting device... (tries: " + QString::number(i) + "/100)"); sleep(1);
                checkConnection.start("cmd", command); checkConnection.waitForFinished();
                QString output = checkConnection.readAll();
                if (output != "") {
                    emit update("Device connection verified.\n"
                                "A cmd will open, where you will be\n"
                                "able to see the progress\n"
                                "Starting flash in 10 seconds...");
                    sleep(10);
                    break;
                } else {
                    if (i > 99) {
                        emit update("Could not detect device.\n"
                                    "Cannot continue.");
                        emit msgBox("Could not detect device", "do the rest :)", 0);
                        connectionBreak = 1;
                    }
                }
            }
            if (connectionBreak == 0) {
            QFile::copy(dir + "platform-tools/fastboot.exe", dir + "ROM/ROM/fastboot.exe");
            QFile::copy(dir + "platform-tools/AdbWinApi.dll", dir + "ROM/ROM/AdbWinApi.dll");
            QFile::copy(dir + "platform-tools/AdbWinUsbApi.dll", dir + "ROM/ROM/AdbWinUsbApi.dll");

            emit update("Flashing...");
            if (option == 1) {
                system("cmd /C \"cd " + dir.toUtf8() + "ROM/ROM &&" + dir.toUtf8() + "ROM/ROM/flash_all.bat && "
                                                    "echo Warning! This is NOT logged. If you received any errors, please copy this whole output. && "
                                                    "echo Finished && pause\"");
            } else if (option == 2) {
                system("cmd /C \"cd " + dir.toUtf8() + "ROM/ROM &&" + dir.toUtf8() + "ROM/ROM/flash_all_except_data_storage.bat && "
                                                    "echo Warning! This is NOT logged. If you received any errors, please copy this whole output. && "
                                                    "echo Finished && pause\"");
            }

            emit update("enable");
            emit update("Flashing finished!\n"
                        "If you don't want to clean temporary files,\n"
                        "close this window now.\n"
                        "If you want to clean them,\n"
                        "click on the next button.\n\n"
                        "You should have your phone flashed now.\n"
                        "Enjoy!");
            }
            stopRunning();
        } else if (function == 5) {
            emit update("Deleting temporary files...");
            QDir remove(dir); remove.removeRecursively();
            emit update("green");
            emit update("Temporary files deleted.\n"
                        "You can close this window now.\n\n"
                        "Thank you for using my program.");
            emit update("close");
            stopRunning();
        }
    }
}

void fThread::stopRunning()
{
    isRunning = 0;
}
