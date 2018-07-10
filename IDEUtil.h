#ifndef IDEUTIL_H
#define IDEUTIL_H

#include <QString>
#include <QStringList>
#include <QJsonArray>
#include <QJsonObject>
class IDEUtil
{
public:
    static QString toJsonFormat(QString instruction,const QJsonArray &parameters);
    static QString toHttpJsonFormat(const QString &instruction,const QVariantMap &parameters);

    //根据模板填充文件，如果文件不存在就创建
    static void TemplateFile(const QString &filePath);
    static void GetAllFileFolder(QString dirPath, QStringList &folder);
    static void GetAllFile(QString dirPath,QStringList &files,const QStringList & limit = QStringList());

    static bool isFileExist(const QString &filePath,const QString &dirPath);
    static  bool deleteDir(const QString &dirName);

    //取第一参数目录地下一集目录
    static QString getNextDir(const QString &topDir,const QString &filePath);

    //新建目录，如果存在，后面加1
    static QString createDir(const QString &dirpath);

    static void msecSleep(int msec);
private:
    IDEUtil();
};

#endif // IDEUTIL_H
