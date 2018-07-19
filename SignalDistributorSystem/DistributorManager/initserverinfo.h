#ifndef INITSERVERINFO_H
#define INITSERVERINFO_H

#include<QTextCodec>

#include<QSqlDatabase>
#include<QSqlQuery>
#include<QSqlDriver>
#include<QSqlRecord>
#include<QSqlError>

#include<QTime>
#include<QtDebug>
#include<QString>
#include<QFile>
#include<QXmlStreamReader>
#include<QMessageBox>

#include<QDomDocument>

void initServer()
{
    bool fileExist=false;
    if(QFile("server.db").exists())
    {
        fileExist=true;
    }
    QTextCodec::setCodecForLocale(QTextCodec::codecForLocale());//设置中文显示

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");//选择SQLite
    //db.setHostName("localhost");//设置数据库主机名
    //db.setDatabaseName(":memory:");//设置数据库名称（当前为内存数据库）
    db.setDatabaseName("server.db");//设置数据库名称（当前为内存数据库）
    db.open();//打开连接
    if(fileExist)
    {
        return;
    }
    //创建数据库表
    QSqlQuery query;
    bool success = query.exec("create table server(id int primary key,"
                              "brokername varchar,brokerid varchar, isp varchar,api varchar,"
                              "istrade boolean,server varchar,port int,name varchar)");
    if(success)
    {
        qDebug()<<QObject::tr("数据库建表成功\n");
    }
    else
    {
        qDebug()<<QObject::tr("数据库建表失败\n");
    }
    //查询数据库表
    query.exec("select * from server");
    QSqlRecord record = query.record();
    qDebug()<<QObject::tr("表server字段数：")<<record.count()<<"\n";

    //插入记录
    QTime t;
    t.start();
    query.prepare("insert into server values(?,?,?,?,?,?,?,?,?)");
    //TODO:从xml文件中读取期货公司服务器信息
    QString fname("D:\\Demos\\CTPTest\\GXECfg\\CTPMore.xml");
    QFile file(fname);
    if(file.exists()==false)
    {
        return;
    }
    if(!file.open(QIODevice::ReadOnly))
    {
QMessageBox::information(nullptr,"读取CTPMore.xml","读取失败");
return;
    }
    QDomDocument doc;
    if(!doc.setContent(&file))
    {
        QMessageBox::information(nullptr,"构造QDomDocument失败","");
        file.close();
        return;
    }
    file.close();

    QDomNodeList brokers = doc.elementsByTagName("broker");
    int rowID=1;
    QString name;
    for(int i=0;i<brokers.count();i++)
    {
        QDomNode node = brokers.at(i);
        QDomElement ele=node.toElement();
        QString brokerID = ele.attribute("brokerid");
        QString brokerName = ele.attribute("brokername").split('_').at(0);

        QDomNodeList groups = ele.elementsByTagName("group");
        for(int grpIndex=0;grpIndex<groups.count();grpIndex++)
        {
            QDomNode grpNode = groups.at(grpIndex);
            QDomElement grpEle = grpNode.toElement();

            QString isp=grpEle.attribute("groupname");

            QDomNodeList qhNodes= grpEle.elementsByTagName("hq").at(0).toElement().elementsByTagName("server");
            QDomNode hqNode = qhNodes.at(0);
            QDomElement hqEle = hqNode.toElement();
            QString hqServer = hqEle.attribute("server");
            int hqPort = hqEle.attribute("port").toInt();

            query.bindValue(0,rowID);
            query.bindValue(1,brokerName);
            query.bindValue(2,brokerID);
            query.bindValue(3,isp);
            query.bindValue(4,"CTP");
            query.bindValue(5,false);
            query.bindValue(6,hqServer);
            query.bindValue(7,hqPort);
            name = brokerName+"_行情_"+isp;
            query.bindValue(8,name);
//            query.bindValue(0,brokerName);
//            query.bindValue(1,brokerID);
//            query.bindValue(2,isp);
//            query.bindValue(3,"CTP");
//            query.bindValue(4,false);
//            query.bindValue(5,hqServer);
//            query.bindValue(6,hqPort);

            QDomNodeList tradeNodes = grpEle.elementsByTagName("trade").at(0).toElement().elementsByTagName("server");
            QDomNode tradeNode = tradeNodes.at(0);
            QDomElement tradeEle = tradeNode.toElement();
            QString tradeServer=tradeEle.attribute("server");
            int tradePort = tradeEle.attribute("port").toInt();

//            query.bindValue(0,brokerName);
//            query.bindValue(1,brokerID);
//            query.bindValue(2,isp);
//            query.bindValue(3,"CTP");
//            query.bindValue(4,true);
//            query.bindValue(5,tradeServer);
//            query.bindValue(6,tradePort);

            success=query.exec();
            rowID++;
            query.bindValue(0,rowID);
            query.bindValue(1,brokerName);
            query.bindValue(2,brokerID);
            query.bindValue(3,isp);
            query.bindValue(4,"CTP");
            query.bindValue(5,true);
            query.bindValue(6,tradeServer);
            query.bindValue(7,tradePort);
            name = brokerName+"_交易_"+isp;
            query.bindValue(8,name);
            success=query.exec();
            rowID++;
//            qDebug()<<QObject::tr("Broker:%1,BrokerID:%2,ISP:%3,Quote:tcp://%4:%5,Trade:tcp//%6:%7\n").arg(brokerName)
//                      .arg(brokerID).arg(isp).arg(hqServer).arg(hqPort).arg(tradeServer).arg(tradePort);
        }
    }

    if(!success)
    {
        QSqlError lastErr=query.lastError();
        qDebug()<<lastErr.driverText()<<QObject::tr("插入数据失败\n%1").arg(lastErr.text());
    }
    qDebug()<<QObject::tr("插入%1条数据耗时:%2ms\n").arg(rowID).arg(t.elapsed());

    //排序
    t.restart();
    success = query.exec("select * from server order by id desc");
    if(success)
    {
        qDebug()<<QObject::tr("排序%1条数据耗时%2ms").arg(rowID).arg(t.elapsed());
    }
    else
    {
        qDebug()<<"排序失败";
    }
    success = query.exec("create table account(id int primary key,"
                              "accountname varchar,accountid varchar, password varchar, serverid int)");
    query.exec("insert into account values(1,'sim01','20017856','qws123',2)");
    query.exec("insert into account values(2,'sim02','20017858','qws122',6)");
    query.exec("insert into account values(3,'sim04','20017866','qws124',8)");
}

void initAccountData()
{
    return;
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");//选择SQLite
    //db.setHostName("localhost");//设置数据库主机名
    db.setDatabaseName(":memory:");//设置数据库名称（当前为内存数据库）
    //db.setDatabaseName("server.db");//设置数据库名称（当前为内存数据库）
    db.open();//打开连接
    //创建数据库表
    QSqlQuery query;
    bool success = query.exec("create table account(id int primary key,"
                              "account varchar,password varchar, serverid int)");
    query.exec("insert into account values(1,'sim01','qws123',2)");
    query.exec("insert into account values(2,'sim02','qws122',6)");
    query.exec("insert into account values(2,'sim04','qws124',8)");
}

#endif // INITSERVERINFO_H
