#include "distributorwindow.h"
#include "ui_distributorwindow.h"

#include<QtWidgets>
#include<QtNetwork>
#include<stdlib.h>

DistributorWindow::DistributorWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DistributorWindow)
{

//    lblStatus = new QLabel;
//    QPushButton *btnQuit = new QPushButton(tr("Quit"));
//    btnQuit->setAutoDefault(false);

//    QHBoxLayout *btnLayout = new QHBoxLayout;
//    btnLayout->addStretch(1);
//    btnLayout->addWidget(btnQuit);
//    btnLayout->addStretch(1);

//    QVBoxLayout *layout = new QVBoxLayout;
//    layout->addWidget(lblStatus);
//    layout->addLayout(btnLayout);
//    setLayout(layout);
    ui->setupUi(this);

    tcpServer = Q_NULLPTR;
    netSession=0;

    setWindowFlags(windowFlags()&~Qt::WindowContextHelpButtonHint);
    //lblStatus->setTextInteractionFlags(Qt::TextBrowserInteraction);

    QNetworkConfigurationManager manager;
    if(manager.capabilities()&QNetworkConfigurationManager::NetworkSessionRequired)
    {
        //Get saved network configuration
        QSettings settings(QSettings::UserScope,QLatin1String("QtProject"));
        settings.beginGroup(QLatin1String("QtNetwork"));
        const QString id = settings.value(QLatin1String("DefaultNetworkConfiguration")).toString();
        settings.endGroup();;

        //If the saved network configuration is not currently disconvered use the system default

        QNetworkConfiguration config = manager.configurationFromIdentifier(id);
        if((config.state()&QNetworkConfiguration::Discovered)!=
                QNetworkConfiguration::Discovered)
        {
            config = manager.defaultConfiguration();
        }
        netSession = new QNetworkSession(config,this);
        connect(netSession,&QNetworkSession::opened,this,&DistributorWindow::sessionOpened);

        lblStatus->setText("Opening network session.");

       // netSession->open();
    }
    else
    {
        sessionOpened();
    }

    msgs<<tr("easy speak than done")
       <<tr("rain cats and dogs")
      <<tr("haha");


//    connect(btnQuit,&QPushButton::clicked,this,&DistributorWindow::close);

//    connect(tcpServer,&QTcpServer::newConnection,this,&DistributorWindow::SendMsg);


}

DistributorWindow::~DistributorWindow()
{
    delete ui;
}


void DistributorWindow::sessionOpened()
{
    //Save the used configuration
    if(netSession)
    {
        QNetworkConfiguration config = netSession->configuration();
        QString id;
        if(config.type()==QNetworkConfiguration::UserChoice)
        {
            id = netSession->sessionProperty(QLatin1String("UserChoiceConfiguration")).toString();
        }
        else
        {
            id = config.identifier();
        }
        QSettings settings(QSettings::UserScope,QLatin1String("QtProject"));
        settings.beginGroup(QLatin1String("QtNetwork"));
        settings.setValue(QLatin1String("DefaultNetworkConfiguration"),id);
        settings.endGroup();
    }
    tcpServer = new QTcpServer(this);
    if(!tcpServer->listen())
    {
        QMessageBox::critical(this,tr("Signal Distributor "),
                              tr("Unable to start the server:%1").arg(tcpServer->errorString()));
        close();
        return;
    }

    QString ipAddr;
    QList<QHostAddress> ipAddrList = QNetworkInterface::allAddresses();

    if(ipAddrList.isEmpty())
    {
        ipAddr = QHostAddress(QHostAddress::LocalHost).toString();
    }
    else
    {
        for(int i=00;i<ipAddrList.size();i++)
        {
            if(ipAddrList.at(i)!=QHostAddress::LocalHost
                    &&ipAddrList.at(i).toIPv4Address())
            {
                ipAddr = ipAddrList.at(i).toString();
                break;
            }
        }
    }
    lblStatus->setText((tr("The server is running on [IP：%1,Port:%2")
                        .arg(ipAddr).arg(tcpServer->serverPort())));

}

void DistributorWindow::SendMsg()
{
    QByteArray block;
    QDataStream out(&block,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);

    out<<(quint16)0;
    out<<msgs.at(qrand()%msgs.size());
    out.device()->seek(0);
    out<<(quint16)(block.size()-sizeof(quint16));

    QTcpSocket *clientConn = tcpServer->nextPendingConnection();
    connect(clientConn,&QTcpSocket::disconnected,clientConn,&QTcpSocket::deleteLater);

    clientConn->write(block);
    clientConn->disconnectFromHost();
}

void DistributorWindow::on_btnQuit_clicked()
{ QByteArray block;
    QDataStream out(&block,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);

    out<<(quint16)0;
    out<<msgs.at(qrand()%msgs.size());
    out.device()->seek(0);
    out<<(quint16)(block.size()-sizeof(quint16));

    QTcpSocket *clientConn = tcpServer->nextPendingConnection();
    connect(clientConn,&QTcpSocket::disconnected,clientConn,&QTcpSocket::deleteLater);

    clientConn->write(block);
    clientConn->disconnectFromHost();

}
