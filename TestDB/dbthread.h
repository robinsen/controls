#ifndef DBTHREAD_H
#define DBTHREAD_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QMap>
#include "commondefine.h"

class CDBManager;

//数据库写线程
class DBThread: public QThread
{
    Q_OBJECT
public:
    DBThread(CDBManager* p);

    void stop();
    void run();
    void wake();
    void startRun();

    void addReq(DataEntityPtr p);

signals:
    void readyMessage(DataEntityPtr);

protected:
    virtual void processItem(DataEntityPtr) = 0;

public:
    CDBManager*   m_pDbManager = nullptr;

private:
    int                  m_type;
    bool                 m_bStop = false;

    QMutex               m_mutex;    //线程同步
    QWaitCondition       m_condition;
    QList<DataEntityPtr> m_reqList;
};

//数据库读线程
class DBReadThread: public DBThread
{
    Q_OBJECT
public:
    DBReadThread(CDBManager* p);

private:
    void processItem(DataEntityPtr);

    void initFuncMap();

    typedef void(DBReadThread::*ReqFun)(DataEntityPtr, bool&);
    QMap<int, ReqFun> m_funMap;
    void dBTest(DataEntityPtr pEntity,bool& bEmit);

};

//数据库写线程
class DBWriteThread: public DBThread
{
    Q_OBJECT
public:
    DBWriteThread(CDBManager* p);

private:
    void processItem(DataEntityPtr);

    void initFuncMap();

    typedef void(DBWriteThread::*ReqFun)(DataEntityPtr, bool&);
    QMap<int, ReqFun> m_funMap;
    void dBTest(DataEntityPtr pEntity,bool& bEmit);

};

#endif // DBTHREAD_H
