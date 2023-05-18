#include <QVariant>
#include <QSqlError>
#include <QSharedPointer>
#include <QStandardPaths>
#include "dbmanager.h"
#include "dbtable.h"

CDBManager::CDBManager()
{
    initDB();
    initTable();
    initThread();
}

CDBManager::~CDBManager()
{
    m_readThread->stop();
    m_writeThread->stop();
}

void CDBManager::addReadReq(DataEntityPtr req)
{
    m_readThread->addReq(req);
}

void CDBManager::addWriteReq(DataEntityPtr req)
{
    m_writeThread->addReq(req);
}

void CDBManager::initDB()
{
    m_pSqlite = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE"));
    QStringList strList = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation);
    m_pSqlite->setDatabaseName(strList.at(2)+"/TestDB");
    m_pSqlite->setConnectOptions("QSQLITE_BUSY_TIMEOUT=10000");
    if(m_pSqlite->open())
    {
        m_pQuery = new QSqlQuery(*m_pSqlite);
        m_pQuery->exec(QString("PRAGMA synchronous = OFF"));
        m_pQuery->exec(QString("PRAGMA journal_mode=WAL"));
        m_pQuery->clear();
    }
    else
    {

    }
}

int CDBManager::initTable()
{
    int ret  = 0;
    if (m_pQuery)
    {
        QSqlQuery& query = *m_pQuery;
        QString  strSql ="SELECT count(*) FROM sqlite_master WHERE type='table' AND name='TableAccount';";
        if(query.exec(strSql))
        {
            if(query.next() && query.value(0).toInt()==0)
            {
                strSql = createTableAccount;
                if (! query.exec(strSql))
                {
                    ret = query.lastError().nativeErrorCode().toInt();
                    return ret;
                }
            }
        }
        else
        {
            ret = query.lastError().nativeErrorCode().toInt();
            return ret;
        }

    }
    return ret;
}

void CDBManager::initThread()
{
    m_readThread.reset(new DBReadThread(this));
    m_writeThread.reset(new DBWriteThread(this));
    m_readThread->start();
    m_writeThread->start();
}
