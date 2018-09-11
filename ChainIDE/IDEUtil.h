#ifndef IDEUTIL_H
#define IDEUTIL_H

#include <qlogging.h>
#include <QString>
#include <QStringList>
#include <QJsonArray>
#include <QJsonObject>

class IDEUtil
{
public:
    static QString toJsonFormat(const QString & instruction,const QJsonArray &parameters,bool addRPCStyle = false);
    static QString toHttpJsonFormat(const QString &instruction,const QVariantMap &parameters);

    //根据模板填充文件，如果文件不存在就创建，模板从资源文件中获取
    static void TemplateFile(const QString &filePath,const QString &sourceName = "initTemplate");

    //获取文件、文件夹方法
    static void GetAllFileFolder(const QString & dirPath, QStringList &folder);
    static void GetAllFile(const QString & dirPath,QStringList &files,const QStringList & limit = QStringList());

    static bool isFileExist(const QString &filePath,const QString &dirPath);
    static bool deleteDir(const QString &dirName);

    //取第一参数目录地下一集目录
    static QString getNextDir(const QString &topDir,const QString &filePath);

    //新建目录，如果存在，后面加1
    static QString createDir(const QString &dirpath);

    static void msecSleep(int msec);

    //qDebug导出
    static void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);

    //在文件夹中选中文件
    static void showInExplorer(const QString &filePath);

    //获取一个数值的位数
    template<typename T>
    static int getDigit(T data){
        int num = -1;
        T actData = data;
        do{
            actData/=10;
            ++num;
        }while(0 != actData);
        return num;
    }
private:
    IDEUtil();
};

#endif // IDEUTIL_H
