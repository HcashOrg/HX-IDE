#ifndef IDEUTIL_H
#define IDEUTIL_H

#include <QString>
#include <QStringList>
#include <QJsonArray>
class IDEUtil
{
public:
    static QString toJsonFormat(QString instruction,QJsonArray parameters);

    //根据模板填充文件，如果文件不存在就创建
    static void TemplateFile(const QString &filePath);
    static void GetAllFileFolder(QString dirPath, QStringList &folder);
    static void GetAllFile(QString dirPath,QStringList &files,const QStringList & limit = QStringList());

    static bool isFileExist(const QString &filePath,const QString &dirPath);
    static  bool deleteDir(const QString &dirName);
private:
    IDEUtil();
};

#endif // IDEUTIL_H
