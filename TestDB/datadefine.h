#ifndef DATADEFINE_H
#define DATADEFINE_H

#include <QSharedPointer>
#include <QString>
#include "idataentity.h"

class DataDBHead: public IDataEntity
{
public:
    int type = 0;
    int subtype = 0;
    int ret = 0;

    QSharedPointer<IDataEntity> req;
    QSharedPointer<IDataEntity> resp;
};

typedef struct Account_t {
    QString account;
    QString password;
} AccountEntity;

class DataDBReqTest: public IDataEntity
{
public:
   AccountEntity ac;

};

class DataDBRespTest: public IDataEntity
{
public:
    QList<AccountEntity> list;
};

#endif // DATADEFINE_H
