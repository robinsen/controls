#ifndef CDBMANAGER_H
#define CDBMANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QThread>
#include <QMutex>
#include <QMap>
#include <QWaitCondition>
#include "dbthread.h"

class CDBManager: public QObject
{
    Q_OBJECT
public:
    CDBManager();
    ~CDBManager();

    void addReadReq(DataEntityPtr req);
    void addWriteReq(DataEntityPtr req);

    QSqlQuery*    getQuery() { return m_pQuery; }
    QSqlDatabase* getDB() { return m_pSqlite; }

signals:
    void readyMessage(DataEntityPtr);

private:
    int  initTable();
    void initDB();
    void initThread();

private:
    QSqlQuery*    m_pQuery = nullptr;
    QSqlDatabase* m_pSqlite = nullptr;

    QSharedPointer<DBReadThread>  m_readThread;
    QSharedPointer<DBWriteThread>  m_writeThread;
};

#endif // CDBMANAGER_H
