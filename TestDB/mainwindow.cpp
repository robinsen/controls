#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "datadefine.h"
#include "commondefine.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    RegiserAllCustomType()
    m_pDBManager = new CDBManager();
    connect(m_pDBManager, SIGNAL(readyMessage(DataEntityPtr)), this, SLOT(slot_db_readyMsg(DataEntityPtr)), Qt::QueuedConnection);
}

MainWindow::~MainWindow()
{
    if (m_pDBManager)
        delete m_pDBManager;
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    DataDBHeadPtr p(new DataDBHead);
    QSharedPointer<DataDBReqTest> req(new DataDBReqTest);
    p->type = DB_WRITETYPE_TEST;
    req->ac.account = "test1";
    req->ac.password = "123";
    p->req = req;
    m_pDBManager->addWriteReq(p);

    DataDBHeadPtr p1(new DataDBHead);
    p1->type = DB_READTYPE_TEST;
    m_pDBManager->addReadReq(p1);
}

void MainWindow::slot_db_readyMsg(DataEntityPtr pResp)
{
    if (!pResp.isNull())
    {
        DataDBHeadPtr head = pResp.staticCast<DataDBHead>();
        if (!head.isNull())
        {
            if (head->type == DB_READTYPE_TEST)
            {
                QSharedPointer<DataDBRespTest> data = head->resp.staticCast<DataDBRespTest>();
                if (data->list.size()>0)
                {
                    foreach (AccountEntity ac, data->list) {
                        qDebug()<<" data ac = "<<ac.account<<" pwd = "<<ac.password;
                    }
                }
            }
        }
    }
}
