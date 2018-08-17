#ifndef CONVENIENTOP_H
#define CONVENIENTOP_H

#include <QObject>
#include <QWidget>

#include "datamanager/DataManagerStruct.h"

class ConvenientOp : public QObject
{
    Q_OBJECT
public:
    explicit ConvenientOp(QObject *parent = 0);

    //显示提示框
    static void ShowSyncCommonDialog(const QString &data);

    //窗口居中
    static void MoveWidgetCenter(QWidget *widget);

    //合约方便操作
    static bool WriteContractToFile(const QString &filePath, const DataManagerStruct::AddressContractDataPtr &data);
    static bool ReadContractFromFile(const QString &filePath, DataManagerStruct::AddressContractDataPtr &results);

    static void AddContract(const QString &owneraddr,const QString &contractaddr,const QString &contractname = "");
    static void DeleteContract(const QString &ownerOrcontract);

    //文件查询操作
    static QString GetMetaJsonFile(const QString &filePath);

    //获取文件夹路径对应合约类型
    static QStringList GetContractSuffixByDir(const QString &dirPath);

    //导入合约源码
    static bool ImportContractFile(const QString &parentDir);

    //导出合约字节码
    static bool ExportContractFile(const QString &gpcFilePath);


};

#endif // CONVENIENTOP_H
