#include <QDebug>
#include <QSqlQuery>
#include "dbmanager.h"
#include "dbthread.h"

//数据库写线程
DBThread::DBThread(CDBManager* p)
{
    m_pDbManager = p;
}

void DBThread::stop()
{
    m_mutex.lock();
    m_bStop = true;
    m_condition.wakeAll();
    m_mutex.unlock();
    quit();
    wait(THREAD_WAIT_TIEMOUT);
    m_bStop=false;
    m_reqList.clear();
}

void DBThread::run()
{
    while(!m_bStop)
    {
        if (m_bStop)
            break;
        m_mutex.lock();
        DataEntityPtr item;
        if (m_reqList.size()<=0 && !m_bStop)
        {
            qDebug()<<" lock thread id = "<<QThread::currentThreadId();
            m_condition.wait(&m_mutex);
        }
        if(m_bStop)
        {
            m_mutex.unlock();
            break;
        }
        if (m_reqList.size()>0)
        {
            item = m_reqList.front();
            m_reqList.pop_front();
        }
        m_mutex.unlock();
        if(!item.isNull())
        {
            processItem(item);
        }
    }
    qDebug()<<" quit run thread id = %1"<<QThread::currentThreadId();
}

void DBThread::wake()
{
    m_mutex.lock();
    m_condition.wakeOne();
    m_mutex.unlock();
}

void DBThread::startRun()
{
    m_bStop = false;
    start();
}

void DBThread::addReq(DataEntityPtr p)
{
    m_mutex.lock();
    m_reqList.push_back(p);
    m_condition.wakeOne();
    m_mutex.unlock();
}

DBReadThread::DBReadThread(CDBManager *p)
    :DBThread(p)
{
    m_pDbManager = p;
    initFuncMap();
}

void DBReadThread::initFuncMap()
{
    m_funMap.insert(DB_READTYPE_TEST, &DBReadThread::dBTest);
}

void DBReadThread::processItem(DataEntityPtr pItem)
{
    QSharedPointer<DataDBHead> pReq = pItem.staticCast<DataDBHead>();
    QSharedPointer<DataDBHead> data(new DataDBHead);
    data->type = pReq->type;
    data->subtype = pReq->subtype;
    data->ret = 0;
    data->req = pItem;
    if (m_funMap.contains(pReq->type))
    {
        bool bEmit = true;
        (this->*m_funMap.value(pReq->type))(data, bEmit);
        if (bEmit)
            emit m_pDbManager->readyMessage(data);
        return;
    }
    else
    {
        qDebug()<<QString("not func contain subtype=%1")<<data->subtype;
    }
}

void DBReadThread::dBTest(DataEntityPtr pEntity, bool &bEmit)
{
    Q_UNUSED(pEntity)
    Q_UNUSED(bEmit)
    QSqlQuery query(*m_pDbManager->getQuery());
    if (query.isValid())
    {
        QSharedPointer<DataDBHead> pResp = pEntity.staticCast<DataDBHead>();
        QSharedPointer<DataDBRespTest> pTest(new DataDBRespTest);
        pResp->resp = pTest;
        QString sql = "select * from TableAccount";
        query.prepare(sql);
        if (query.exec())
        {
            while (query.next()) {
                AccountEntity ac;
                ac.account = query.value(0).toString();
                ac.password = query.value(1).toString();
                pTest->list.push_back(ac);
            }
        }
        else
        {
            qDebug()<<" insert error ";
        }
        query.finish();
    }
    else
    {
        qDebug()<<" qeury is invalid ";
    }
}


DBWriteThread::DBWriteThread(CDBManager *p)
    :DBThread(p)
{
    m_pDbManager = p;
    initFuncMap();
}

void DBWriteThread::initFuncMap()
{
    m_funMap.insert(DB_WRITETYPE_TEST, &DBWriteThread::dBTest);
}

void DBWriteThread::processItem(DataEntityPtr pItem)
{
    QSharedPointer<DataDBHead> pReq = pItem.staticCast<DataDBHead>();
    if (m_funMap.contains(pReq->type))
    {
        bool bEmit = true;
        (this->*m_funMap.value(pReq->type))(pItem, bEmit);
//        if (bEmit)
//            emit m_pDbManager->readyMessage(pReq);
        return;
    }
    else
    {

    }
}

void DBWriteThread::dBTest(DataEntityPtr pEntity, bool &bEmit)
{
    Q_UNUSED(pEntity)
    Q_UNUSED(bEmit)
    QSqlQuery query(*m_pDbManager->getQuery());
    if (query.isValid())
    {
        QSharedPointer<DataDBHead> pReq = pEntity.staticCast<DataDBHead>();
        QSharedPointer<DataDBReqTest> pTest = pReq->req.staticCast<DataDBReqTest>();
        QString sql = "insert or replace into TableAccount (account,pwd) values (:account,:pwd)";
        query.prepare(sql);
        QVariantList va1,va2;
        for (int i=1; i<100; i++)
        {
            va1<<QVariant(pTest->ac.account+QString::number(i));
            va2<<QVariant(pTest->ac.password);
//            query.addBindValue(QVariant(pTest->ac.account));
//            query.addBindValue(QVariant(pTest->ac.password));
        }
        query.addBindValue(va1);
        query.addBindValue(va2);
        if (!query.execBatch())
        {
            qDebug()<<" insert error ";
        }
        query.finish();
    }
    else
    {
        qDebug()<<" qeury is invalid ";
    }
}
