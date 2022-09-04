#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QTextStream>
#include <QtCore/QRegExp>
#include <QDate>
#include <iostream>

const char* const pHelp = \
"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"\
"~ call the app with:                                                           ~\n"\
"~     AutoIncreaseBuildNr <versionfile>                                        ~\n"\
"~ or for help with                                                             ~\n"\
"~     AutoIncreaseBuildNr -h                                                   ~\n"\
"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QStringList szArgs = app.arguments();

    QFile fileLog(QString("AutoVersion日志.txt"));
    if (fileLog.open(QIODevice::WriteOnly/*|QIODevice::Append*/))
    {
        QTextStream out(&fileLog);
        foreach (QString strArg,szArgs) {
            out << strArg << "\n";
        }
        fileLog.close();
    }

    // 1.) extract arguments
    if(szArgs.count() < 2)
    {
        std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
        std::cout << "Command line errors: " << std::endl;
        std::cout << "the tool was called with an invalid set of arguments." << std::endl;
        std::cout << pHelp << std::endl;
        return 1;
    }

    QString strArgs1 = szArgs.at(1);
    if((0 == strArgs1.compare(QLatin1String("-h"), Qt::CaseInsensitive)) ||
       (0 == strArgs1.compare(QLatin1String("/h"), Qt::CaseInsensitive)) ||
       (0 == strArgs1.compare(QLatin1String("-?"), Qt::CaseInsensitive)) ||
       (0 == strArgs1.compare(QLatin1String("/?"), Qt::CaseInsensitive)) ||
       (0 == strArgs1.compare(QLatin1String("-help"), Qt::CaseInsensitive)) ||
       (0 == strArgs1.compare(QLatin1String("/help"), Qt::CaseInsensitive)))
    {
        std::cout << pHelp << std::endl;
        return 0;
    }

    if(0 && !QFile::exists(strArgs1))
    {
        std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
        std::cout << "File not found: " << strArgs1.toLatin1().constData() << std::endl;
        std::cout << pHelp << std::endl;
        return 2;
    }

    // 2.) read version file
    QFile versionFile(strArgs1);
    if(!versionFile.open(QFile::ReadWrite))
    {
        std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
        std::cout << "Error openeing the version file for reading: " << strArgs1.toLatin1().constData() << std::endl;
        std::cout << "Error: " << versionFile.errorString().toLatin1().constData() << std::endl;
        std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
        return 3;
    }

    QStringList slContent;
    {
        QTextStream stream(&versionFile);
        if( strArgs1.contains(".h") )
            stream.setCodec("utf-8");
        while(!stream.atEnd())
        {
            slContent.append(stream.readLine());
        }
    }
    versionFile.close();

    //处理.h格式文件
    if( strArgs1.endsWith(".h",Qt::CaseInsensitive) )
    {
        QString strProductName;
        int nProductCode = 0;
        // 3.) change build nr
        for(int i = 2; i < szArgs.count(); i ++ )
        {
            QString strArgi = szArgs.at(i);
            if( strArgi.startsWith("-pn") )
            {
                strArgi.remove(0,3);
                strProductName = strArgi;
            }
            else if( strArgi.startsWith("-pc") )
            {
                strArgi.remove(0,3);
                nProductCode = strArgi.toInt();
            }
        }

        // 3.) change build nr
        for(int i = 0; i < slContent.count(); ++i)
        {
            if( !strProductName.isEmpty() )
            {// change product
                QString& strLine = slContent[i];
                QString strProduct = QString("PRODUCT_NAME");

                strLine = strLine.trimmed();
                QStringList szWordsOfLine = strLine.split(QRegExp("\\s"), QString::SkipEmptyParts);
                if(3 == szWordsOfLine.count() && szWordsOfLine.at(0) == "#define" && szWordsOfLine.at(1) == strProduct)
                {
                    szWordsOfLine[2] = QString("\"%1\"").arg(strProductName);
                    strLine = szWordsOfLine.join(" ");
                    continue;
                }
            }
            if( nProductCode > -1 )
            {// change product
                QString& strLine = slContent[i];
                QString strProduct = QString("PRODUCT_CODE");

                strLine = strLine.trimmed();
                QStringList szWordsOfLine = strLine.split(QRegExp("\\s"), QString::SkipEmptyParts);
                if(3 == szWordsOfLine.count() && szWordsOfLine.at(0) == "#define" && szWordsOfLine.at(1) == strProduct)
                {
                    szWordsOfLine[2] = QString("%1").arg(nProductCode);
                    strLine = szWordsOfLine.join(" ");
                    continue;
                }
            }

            {//change version
                QString& rszLine = slContent[i];
                QStringList slVersions;
                slVersions << "FILE_VERSION_COMMA|," << "FILE_VERSION_DOT|.";
                foreach ( QString strVersion, slVersions )
                {
                    QString strVersionF,strVersionS;
                    strVersionF = strVersion.split("|").at(0);
                    strVersionS = strVersion.split("|").at(1);

                    if(rszLine.contains(QLatin1String("#define")) && rszLine.contains(strVersionF))
                    {
                        rszLine = rszLine.trimmed();
                        QStringList szWordsOfLine = rszLine.split(QRegExp("\\s"), QString::SkipEmptyParts);
                        if(3 == szWordsOfLine.count() && szWordsOfLine.at(0) == "#define" && szWordsOfLine.at(1) == strVersionF)
                        {
                            QString strCurrentDate = QDate::currentDate().toString(QString("yyyy%1MM%1dd").arg(strVersionS));
                            QString strVer = szWordsOfLine.at(2);
                            QString strVersionSymbol = "";
                            if( strVer.contains("\"") )
                            {
                                strVer.replace(QString("\""),QString(""));
                                strVersionSymbol = QString("\"");
                            }
                            QStringList szVersions = strVer.split(strVersionS,QString::SkipEmptyParts);
                            if( strVer.contains(strCurrentDate) && szVersions.size() > 3 )
                            {
                                int nBuildNumber = szVersions.at(3).toInt();
                                std::cout << "Old version was " << nBuildNumber << " and is now incremented by 1." << std::endl;
                                nBuildNumber++;
                                szWordsOfLine[2] = strVersionSymbol+strCurrentDate+strVersionS+QString::number(nBuildNumber)+strVersionSymbol;
                            }
                            else
                            {
                                szWordsOfLine[2] = QString("%1%2%3%4%1").arg(strVersionSymbol).arg(strCurrentDate).arg(strVersionS).arg("0");
                            }
                            rszLine = szWordsOfLine.join(" ");

                            QFile file(QString("../Verson_%1.txt").arg(strProductName));if (file.open(QIODevice::WriteOnly)){QTextStream out(&file);out << szWordsOfLine[2].remove("\"") << "\n";file.close();}
                            break;
                        }
                    }
                }
            }
        }
    }

    //处理.pri格式文件,把一些宏放到pri下。避免因为编译单元太大导致的编译太慢，pri文件只在变更的时候才被重写
    if( strArgs1.endsWith(".pri",Qt::CaseInsensitive) )
    {
        bool bNeedReWrite = false;
        // 3.) change build nr
        for(int i = 2; i < szArgs.count(); i ++ )
        {
            QString strArgi = szArgs.at(i);
            if( strArgi.startsWith("-r") )
            {
                strArgi.remove(0,2);
                for(int j = 0; j < slContent.count(); ++j)
                {
                    QString strLine = slContent[j];
                    strArgi = strArgi.simplified();
                    strLine = strLine.simplified();
                    if( strArgi.compare( strLine, Qt::CaseInsensitive) == 0  )
                    {
                        slContent.removeAt(j);
                        j--;
                        bNeedReWrite = true;
                    }
                }
            }
            else if( strArgi.startsWith("-a") || strArgi.startsWith("-i") )
            {
                strArgi.remove(0,2);
                bool bFinded = false;
                int j = 0;
                for(; j < slContent.count(); ++j)
                {
                    QString strLine = slContent[j];
                    strArgi = strArgi.simplified();
                    strLine = strLine.simplified();
                    if( strArgi.compare( strLine, Qt::CaseInsensitive) == 0  )
                    {
                        bFinded = true;
                        break;
                    }
                }
                if( !bFinded )
                {
                    if( szArgs.at(i).startsWith("-i") )
                    {
                        slContent.insert(0,strArgi);
                    }
                    else
                    {
                        slContent.append(strArgi);
                    }
                    bNeedReWrite = true;
                }
            }
        }
        if( !bNeedReWrite )
            return 0;
    }

    fileLog.setFileName(QString("AutoVersion日志.txt"));
    // 4.) write version file again.
    if(!versionFile.open(QFile::WriteOnly) || !fileLog.open(QIODevice::WriteOnly|QIODevice::Append))
    {
        std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
        std::cout << "Error openeing the version file for writing: " << strArgs1.toLatin1().constData() << std::endl;
        std::cout << "Error: " << versionFile.errorString().toLatin1().constData() << std::endl;
        std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
        return 4;
    }

    QTextStream out(&fileLog);

    QTextStream writeStream(&versionFile);
    if( strArgs1.contains(".h") )
        writeStream.setCodec("utf-8");
    for(int i = 0; i < slContent.count(); ++i)
    {
        writeStream << slContent.at(i) << "\r\n" << flush;
        out << slContent.at(i) << "\r\n" << flush;
    }
    std::cout << "version file written" << std::endl;
    fileLog.close();

    return 0;
 }
