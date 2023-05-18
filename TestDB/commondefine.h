#ifndef COMMONDEFINE_H
#define COMMONDEFINE_H

#include <QSharedPointer>
#include <functional>
#include "datadefine.h"

#define THREAD_WAIT_TIEMOUT 10*1000

#define DB_READTYPE_TEST    1
#define DB_WRITETYPE_TEST   1

typedef QSharedPointer<IDataEntity> DataEntityPtr;
typedef QSharedPointer<DataDBHead>  DataDBHeadPtr;

#define RegiserAllCustomType() \
    qRegisterMetaType<IDataEntity>("IDataEntity");\
    qRegisterMetaType<DataEntityPtr>("DataEntityPtr");\
    qRegisterMetaType<DataDBHead>("DataDBHead");\
    qRegisterMetaType<DataDBHeadPtr>("DataDBHeadPtr");

Q_DECLARE_METATYPE(QSharedPointer<IDataEntity>)
Q_DECLARE_METATYPE(QList<QSharedPointer<IDataEntity>>)
Q_DECLARE_METATYPE(QSharedPointer<DataDBHead>)
Q_DECLARE_METATYPE(QList<QSharedPointer<DataDBHead>>)

#endif // COMMONDEFINE_H
