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

    static bool WriteContractToFile(const QString &filePath, const DataDefine::AddressContractDataPtr &data);
    static bool ReadContractFromFile(const QString &filePath, DataDefine::AddressContractDataPtr &results);
};

#endif // CONVENIENTOP_H
