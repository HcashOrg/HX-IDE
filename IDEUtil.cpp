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




IDEUtil::IDEUtil()
{

}
