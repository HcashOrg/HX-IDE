#include "IDEUtil.h"

#include <QString>
#include <QStringList>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QTime>
#include <QDateTime>
#include <QFile>
#include <QMutex>
#include <QDesktopServices>
#include <QUrl>
#include <QProcess>

QString IDEUtil::toJsonFormat(const QString &instruction, const QJsonArray & parameters,bool addRPCStyle)
{
    QJsonObject object;
    if(addRPCStyle)
    {
        object.insert("jsonrpc", "2.0");
    }
    object.insert("id", 32800);
    object.insert("method", instruction);
    object.insert("params",parameters);

    return QJsonDocument(object).toJson();
}

QString IDEUtil::toHttpJsonFormat(const QString &instruction, const QVariantMap &parameters)
{
    QJsonObject object;
    object.insert("jsonrpc","2.0");
    object.insert("id",45);
    object.insert("method",instruction);
    object.insert("params",QJsonObject::fromVariantMap(parameters));
    return QJsonDocument(object).toJson();
}

void IDEUtil::TemplateFile(const QString &filePath,const QString &sourceName)
{
    QFileInfo testfile(filePath);
    if(!testfile.exists())
    {
        QDir dstDir(testfile.absoluteDir());
        dstDir.mkpath(dstDir.path());
    }
    //不存在就创建模板
    //根据文件类型，判定模板种类
    QFile templete(QString(":/template/%1.%2").arg(sourceName).arg(QFileInfo(filePath).suffix()));

    if( templete.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QByteArray ba = templete.readAll();
        templete.close();
        //写入文件
        QFile file(filePath);
        if(file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            file.write(ba);
        }
        file.close();
    }
    else
    {
        QFile file(filePath);
        if(file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
        }
        file.close();
    }
}


void IDEUtil::GetAllFileFolder(const QString & dirPath, QStringList &folder)
{

    QDir dir(dirPath);

    if(QFileInfo(dirPath).isDir() && !folder.contains(dirPath))
    {
        folder<<QFileInfo(dirPath).absoluteFilePath();
    }
    dir.setFilter(QDir::Dirs);

    foreach(QFileInfo fullDir, dir.entryInfoList())
    {

        if(fullDir.fileName() == "." || fullDir.fileName() == "..") continue;

        folder<<fullDir.absoluteFilePath();

        GetAllFileFolder(fullDir.absoluteFilePath(),  folder);
    }

}

void IDEUtil::GetAllFile(const QString & dirPath, QStringList &files,const QStringList & limit)
{
    QDir dir(dirPath);
    dir.setFilter(QDir::Dirs|QDir::Files);
    QFileInfoList list = dir.entryInfoList();

    foreach (QFileInfo info, list) {
        if(info.fileName() == "." || info.fileName() == "..") continue;
        if(info.isFile())
        {
            if(limit.empty() || limit.contains(info.suffix()))
            {
                files.append(info.absoluteFilePath());
            }

        }
        else if(info.isDir())
        {
            GetAllFile(info.absoluteFilePath(),files,limit);
        }
    }

}

bool IDEUtil::isFileExist(const QString &filePath, const QString &dirPath)
{
    QDir dir(dirPath);
    dir.setFilter(QDir::Files);
    foreach (QFileInfo fullFile, dir.entryInfoList()) {
        if(fullFile.fileName() == QFileInfo(filePath).fileName() ||
           fullFile.fileName() == QFileInfo(filePath).fileName()+"."+fullFile.suffix()){
            return true;
        }
    }
    return false;
}

bool IDEUtil::deleteDir(const QString &dirName)
{
    QDir directory(dirName);
    if (!directory.exists())
    {
        return true;
    }

    QString srcPath = QDir::toNativeSeparators(dirName);
    if (!srcPath.endsWith(QDir::separator()))
        srcPath += QDir::separator();

    QStringList fileNames = directory.entryList(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden);
    bool error = false;
    for (QStringList::size_type i=0; i != fileNames.size(); ++i)
    {
        QString filePath = srcPath + fileNames.at(i);
        QFileInfo fileInfo(filePath);
        if (fileInfo.isFile() || fileInfo.isSymLink())
        {
            QFile::setPermissions(filePath, QFile::WriteOwner);
            if (!QFile::remove(filePath))
            {
                qDebug() << "remove file" << filePath << " faild!";
                error = true;
            }
        }
        else if (fileInfo.isDir())
        {
            if (!deleteDir(filePath))
            {
                error = true;
            }
        }
    }

    if (!directory.rmdir(QDir::toNativeSeparators(directory.path())))
    {
        qDebug() << "remove dir" << directory.path() << " faild!";
        error = true;
    }

    return !error;
}

QString IDEUtil::getNextDir(const QString &topDir, const QString &filePath)
{
    QString top = topDir;
    top.replace("\\","/");
    QString file = filePath;
    file.replace("\\","/");

    if(!file.startsWith(top)) return "";
    QString left = file.mid(top.length()+1);
    return top+"/"+left.mid(0,left.indexOf("/"));

}

QString IDEUtil::createDir(const QString &dirpath)
{
    //如果存在，则后面+1，直到不存在
    QString path = dirpath;
    while(QDir(path).exists())
    {
        path += "1";
    }
    QDir dir(path);
    dir.mkpath(dir.path());
    return path;
}

void IDEUtil::msecSleep(int msec)
{
    QTime n=QTime::currentTime();
    QTime now;
    do{
         now=QTime::currentTime();
       }while (n.msecsTo(now)<=msec);
}

void IDEUtil::myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    // 加锁
    static QMutex mutex;
    mutex.lock();

    QByteArray localMsg = msg.toLocal8Bit();

    QString strMsg("");
    switch(type)
    {
    case QtDebugMsg:
        strMsg = QString("Debug:");
        break;
    case QtWarningMsg:
        strMsg = QString("Warning:");
        break;
    case QtCriticalMsg:
        strMsg = QString("Critical:");
        break;
    case QtFatalMsg:
        strMsg = QString("Fatal:");
        break;
    }

    // 设置输出信息格式
    QString strDateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd");
    QString strMessage = QString("DateTime:%1 Message:%2").arg(strDateTime).arg(localMsg.constData());

    // 输出信息至文件中（读写、追加形式），超过50M删除日志
    QFileInfo info("log.txt");
    if(info.size() > 1024*1024*50) QFile::remove("log.txt");
    QFile file("log.txt");
    file.open(QIODevice::ReadWrite | QIODevice::Append);
    QTextStream stream(&file);
    stream << strMessage << "\r\n";
    file.flush();
    file.close();

    // 解锁
    mutex.unlock();
}

void IDEUtil::showInExplorer(const QString &filePath)
{
    if(QFileInfo(filePath).isDir())
    {
        QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
        return;
    }

#ifdef _WIN32
    QStringList args;
    args << "/select," << QDir::toNativeSeparators(filePath);
    QProcess::startDetached("explorer", args);
#elif TARGET_OS_MAC
    QStringList args;
    args << "-e";
    args << "tell application \"Finder\"";
    args << "-e";
    args << "activate";
    args << "-e";
    args << "select POSIX file \""+filePath+"\"";
    args << "-e";
    args << "end tell";
    QProcess::startDetached("osascript", args);
#endif
}

IDEUtil::IDEUtil()
{

}

