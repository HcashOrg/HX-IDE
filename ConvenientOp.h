#ifndef CONVENIENTOP_H
#define CONVENIENTOP_H

#include <QObject>

#include "DataDefine.h"

class ConvenientOp : public QObject
{
    Q_OBJECT
public:
    explicit ConvenientOp(QObject *parent = 0);

    static void ShowSyncCommonDialog(const QString &data);

    //合约方便操作
    static bool WriteContractToFile(const QString &filePath, const DataDefine::AddressContractDataPtr &data);
    static bool ReadContractFromFile(const QString &filePath, DataDefine::AddressContractDataPtr &results);

    static void AddContract(const QString &owneraddr,const QString &contractaddr,const QString &contractname = "");
    static void DeleteContract(const QString &ownerOrcontract);

    //文件查询操作
    static QString GetMetaJsonFile(const QString &filePath);

    //获取文件夹路径对应合约类型
    static QStringList GetContractSuffixByDir(const QString &dirPath);

    //导入合约源码
    static bool ImportContractFile(const QString &parentDir);


};

#endif // CONVENIENTOP_H
