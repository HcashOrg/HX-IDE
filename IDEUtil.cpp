#include "IDEUtil.h"

#include <QJsonObject>
#include <QJsonDocument>
#include <QFileInfo>
#include <QDir>
#include <QDebug>

QString IDEUtil::toJsonFormat(QString instruction, QJsonArray parameters)
{
    QJsonObject object;
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

QString IDEUtil::toUbcdHttpJsonFormat(const QString &instruction, const QJsonArray &parameters)
{
    QJsonObject object;
    object.insert("id",1);
    object.insert("method",instruction);
    object.insert("params",parameters);
    return QJsonDocument(object).toJson();

}

void IDEUtil::TemplateFile(const QString &filePath)
{
    QFileInfo testfile(filePath);
    if(!testfile.exists())
    {
        QDir dstDir(testfile.absoluteDir());
        dstDir.mkpath(dstDir.path());
    }
    //不存在就创建模板
    //根据文件类型，判定模板种类
    QFile templete(QString(":/template/initTemplate%1").arg(filePath.mid(filePath.lastIndexOf("."))));

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
}


void IDEUtil::GetAllFileFolder(QString dirPath, QStringList &folder)
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

void IDEUtil::GetAllFile(QString dirPath, QStringList &files,const QStringList & limit)
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
                qDebug()<<limit;
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
    qDebug()<<dirName;
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

IDEUtil::IDEUtil()
{

}
