#include "traderwindow.h"
#include "ui_traderwindow.h"

#include<QtWidgets>
#include<QtNetwork>

TraderWindow::TraderWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TraderWindow)
{
    ui->setupUi(this);
    cboHost = new QComboBox;
    txtPort= new QLineEdit;

    btnGetMsg = new QPushButton(tr("Get Msg"));

    tcpSocket = new QTcpSocket(this);
    blockSize=0;
    netSession=Q_NULLPTR;

    cboHost->setEditable(true);

    QString name = QHostInfo::localHostName();
    if(!name.isEmpty())
    {
        cboHost->addItem(name);
        QString domain = QHostInfo::localDomainName();
        if(!domain.isEmpty())
        {
            cboHost->addItem(name+"."+domain);
        }
    }
    if(name!=QLatin1String("localhost"))
    {
        cboHost->addItem("localhost");
    }

    QList<QHostAddress> ipAddrList = QNetworkInterface::allAddresses();

    //Add non-localhost address
    for(int i=0;i<ipAddrList.size();i++)
    {
        if(!ipAddrList.at(i).isLoopback())
        {
            cboHost->addItem(ipAddrList.at(i).toString());
        }
    }

    //Add localhost address
    for(int i=0;i<ipAddrList.size();i++)
    {
        if(ipAddrList.at(i).isLoopback())
        {
            cboHost->addItem(ipAddrList.at(i).toString());
        }
    }
    txtPort->setValidator(new QIntValidator(1,6555,this));

    QLabel *lblHost = new QLabel(tr("&Server name"));
    lblHost->setBuddy(cboHost);

    QLabel *lblPort = new QLabel(tr("Server &port"));
    lblPort->setBuddy(txtPort);

    lblStatus = new QLabel(tr("This requires that you run the server as well"));

    btnGetMsg->setDefault(true);
    btnGetMsg->setEnabled(false);

    QPushButton *btnQuit = new QPushButton(tr("Quit"));

    QDialogButtonBox *btnBox = new QDialogButtonBox;
    btnBox->addButton(btnGetMsg,QDialogButtonBox::ActionRole);
    btnBox->addButton(btnQuit,QDialogButtonBox::RejectRole);


    connect(cboHost,&QComboBox::editTextChanged,this,&TraderWindow::enableGetMsgButton);
    connect(txtPort,&QLineEdit::textChanged,this,&TraderWindow::enableGetMsgButton);
    connect(btnGetMsg,&QPushButton::clicked,this,&TraderWindow::requestNewMsg);
    connect(btnQuit,&QPushButton::clicked,this,&TraderWindow::close);
    connect(tcpSocket,&QTcpSocket::readyRead,this,&TraderWindow::readMsg);
    typedef void (QAbstractSocket::*QAbstractSocketErrorSignal)(QAbstractSocket::SocketError);
    connect(tcpSocket,static_cast<QAbstractSocketErrorSignal>(&QAbstractSocket::error),this,&TraderWindow::displayError);

    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->addWidget(lblHost,0,0);
    gridLayout->addWidget(cboHost,0,1);
    gridLayout->addWidget(lblPort,1,0);
    gridLayout->addWidget(txtPort,1,1);
    gridLayout->addWidget(lblStatus,2,0,1,2);
    gridLayout->addWidget(btnBox,3,0,1,2);

    txtPort->setFocus();

    setLayout(gridLayout);

    QNetworkConfigurationManager manager;
    if (manager.capabilities() & QNetworkConfigurationManager::NetworkSessionRequired) {
        // Get saved network configuration
        QSettings settings(QSettings::UserScope, QLatin1String("QtProject"));
        settings.beginGroup(QLatin1String("QtNetwork"));
        const QString id = settings.value(QLatin1String("DefaultNetworkConfiguration")).toString();
        settings.endGroup();

        // If the saved network configuration is not currently discovered use the system default
        QNetworkConfiguration config = manager.configurationFromIdentifier(id);
        if ((config.state() & QNetworkConfiguration::Discovered) !=
            QNetworkConfiguration::Discovered) {
            config = manager.defaultConfiguration();
        }

        netSession = new QNetworkSession(config, this);
        connect(netSession, &QNetworkSession::opened, this, &TraderWindow::sessionOpened);

        btnGetMsg->setEnabled(false);
        lblStatus->setText(tr("Opening network session."));
        netSession->open();
    }
}

TraderWindow::~TraderWindow()
{
    delete ui;
}

void TraderWindow::requestNewMsg()
{
    btnGetMsg->setEnabled(false);

    blockSize=0;
    tcpSocket->abort();
    tcpSocket->connectToHost(cboHost->currentText(),txtPort->text().toInt());


}

void TraderWindow::readMsg()
{
    QDataStream in(tcpSocket);
    in.setVersion(QDataStream::Qt_4_0);
    if(blockSize==0)
    {
        if(tcpSocket->bytesAvailable()<(int)sizeof(quint16))
        {
            return;
        }
        in>>blockSize;
    }
    if(tcpSocket->bytesAvailable()<blockSize)
    {
        return;
    }
    QString nextMsg;
    in>>nextMsg;
    if(nextMsg==currMsg)
    {
            QTimer::singleShot(0,this,&TraderWindow::requestNewMsg);
            return;
    }
    currMsg=nextMsg;
    lblStatus->setText(currMsg);
    btnGetMsg->setEnabled(true);
}

void TraderWindow::displayError(QAbstractSocket::SocketError socketErr)
{switch (socketErr) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::information(this, tr("Fortune Client"),
                                 tr("The host was not found. Please check the "
                                    "host name and port settings."));
        break;
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::information(this, tr("Fortune Client"),
                                 tr("The connection was refused by the peer. "
                                    "Make sure the fortune server is running, "
                                    "and check that the host name and port "
                                    "settings are correct."));
        break;
    default:
        QMessageBox::information(this, tr("Fortune Client"),
                                 tr("The following error occurred: %1.")
                                 .arg(tcpSocket->errorString()));
    }

    btnGetMsg->setEnabled(true);}

void TraderWindow::enableGetMsgButton()
{
    btnGetMsg->setEnabled((!netSession||netSession->isOpen())
                          &&!cboHost->currentText().isEmpty()
                          &&!txtPort->text().isEmpty());
}

void TraderWindow::sessionOpened()
{  // Save the used configuration
    QNetworkConfiguration config = netSession->configuration();
    QString id;
    if (config.type() == QNetworkConfiguration::UserChoice)
        id = netSession->sessionProperty(QLatin1String("UserChoiceConfiguration")).toString();
    else
        id = config.identifier();

    QSettings settings(QSettings::UserScope, QLatin1String("QtProject"));
    settings.beginGroup(QLatin1String("QtNetwork"));
    settings.setValue(QLatin1String("DefaultNetworkConfiguration"), id);
    settings.endGroup();

    lblStatus->setText(tr("This program requires that you run the Server example as well."));

    enableGetMsgButton();
}
