
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include"initserverinfo.h"
#include"accountdialog.h"

#include<QtWidgets>
#include<QMenu>
#include<QAction>

#include<QDebug>

#include<QSqlTableModel>
#include<QSqlRelationalTableModel>
#include<QSqlRelationalDelegate>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setCentralWidget(ui->tabMain);
    initServer();
    initServerPage();
    initAccountPage();
    createMenuBar();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showError(const QSqlError &error)
{
    QMessageBox::critical(this,"Unable to initialize databse","Error initailizing databse:"+error.text());
}

void MainWindow::initAccountPage()
{
    accountModel = new QSqlRelationalTableModel(this);
    accountModel->setTable("account");
    accountModel->setEditStrategy(QSqlTableModel::OnManualSubmit);

    accountModel->setRelation(4,QSqlRelation("server","id","name"));
    accountModel->setHeaderData(0,Qt::Horizontal,tr("ID"));
    accountModel->setHeaderData(1,Qt::Horizontal,tr("账号名称"));
    accountModel->setHeaderData(2,Qt::Horizontal,tr("资金账号"));
    accountModel->setHeaderData(3,Qt::Horizontal,tr("交易密码"));
    accountModel->setHeaderData(4,Qt::Horizontal,tr("Server Name"));

    int rowCount = accountModel->rowCount();
    int colCount=accountModel->columnCount();

    if(!accountModel->select())
    {
        showError(accountModel->lastError());
    }

    accountView = new QTableView;
    accountView->setModel(accountModel);
    accountView->hideColumn(0);
    accountView->setItemDelegate(new QSqlRelationalDelegate(accountView));
    accountView->resizeColumnsToContents();


    btnSubmitAccount = new QPushButton(tr("Submit"));
    btnSubmitAccount->setDefault(true);
    btnRevertAccount = new QPushButton(tr("Revert"));
    btnAddAccount = new QPushButton(tr("Add"));
    btnDeleteAccount = new QPushButton(tr("Delete"));

    accountBox=new QDialogButtonBox(Qt::Vertical);
    accountBox->addButton(btnSubmitAccount,QDialogButtonBox::ActionRole);
    accountBox->addButton(btnRevertAccount,QDialogButtonBox::ActionRole);
    accountBox->addButton(btnAddAccount,QDialogButtonBox::ActionRole);
    accountBox->addButton(btnDeleteAccount,QDialogButtonBox::ActionRole);

    connect(btnSubmitAccount,&QPushButton::clicked,this,&MainWindow::accountSubmit);
    connect(btnRevertAccount,&QPushButton::clicked,accountModel,&QSqlRelationalTableModel::revertAll);
    connect(btnAddAccount,&QPushButton::clicked,this,&MainWindow::accountAdd);
    connect(btnDeleteAccount,&QPushButton::clicked,this,&MainWindow::accountDelete);

    QHBoxLayout *accountLayout = new QHBoxLayout;
    accountLayout->addWidget(accountView);
    accountLayout->addWidget(accountBox);
    ui->tpAccount->setLayout(accountLayout);
}

void MainWindow::initServerPage()
{
    //定义服务器信息实体
    serverModel = new QSqlTableModel(this);
    serverModel->setTable("server");
    serverModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    serverModel->select();

    serverModel->setHeaderData(0,Qt::Horizontal,tr("ID"));
    serverModel->setHeaderData(1,Qt::Horizontal,tr("期货公司"));
    serverModel->setHeaderData(2,Qt::Horizontal,tr("期货公司代码"));

    serverModel->setHeaderData(3,Qt::Horizontal,tr("网络线路"));
    serverModel->setHeaderData(4,Qt::Horizontal,tr("API类型"));
    serverModel->setHeaderData(5,Qt::Horizontal,tr("交易连接"));
    serverModel->setHeaderData(6,Qt::Horizontal,tr("服务器地址"));
    serverModel->setHeaderData(7,Qt::Horizontal,tr("服务器端口"));
    serverModel->setHeaderData(8,Qt::Horizontal,tr("Name"));

    QTableView *serverView=new QTableView;
    serverView->setModel(serverModel);
    serverView->resizeColumnsToContents();
    serverView->hideColumn(0);

    btnSubmitServer = new QPushButton(tr("Submit"));
    btnSubmitServer->setDefault(true);
    btnRevertServer = new QPushButton(tr("Revert"));

    serverBox=new QDialogButtonBox(Qt::Vertical);
    serverBox->addButton(btnSubmitServer,QDialogButtonBox::ActionRole);
    serverBox->addButton(btnRevertServer,QDialogButtonBox::ActionRole);

    connect(btnSubmitServer,&QPushButton::clicked,this,&MainWindow::serverSumbit);
    connect(btnRevertServer,&QPushButton::clicked,serverModel,&QSqlTableModel::revertAll);

    QHBoxLayout *serverLayout = new QHBoxLayout;
    serverLayout->addWidget(serverView);
    serverLayout->addWidget(serverBox);
    ui->tpServer->setLayout(serverLayout);
}

void MainWindow::initUserPage()
{

}

void MainWindow::createMenuBar()
{
    QAction *quitAction = new QAction(tr("&Exit"),this);

    QAction *addAccountAction=new QAction(tr("&Add account..."),this);
    //QAction *addServerAction = new Action(tr("&Add server..."),this);

    QAction *abountAction = new QAction(tr("&About"),this);
    QAction *abountQtAction = new QAction(tr("About &Qt"),this);

    quitAction->setShortcut(QKeySequence::Quit);

    QMenu *sysMenu = menuBar()->addMenu(tr("&System"));
    sysMenu->addAction(quitAction);

    QMenu *dataMenu = menuBar()->addMenu(tr("&Data"));
    dataMenu->addAction(addAccountAction);
    //dataMenu->addAction(addServerAction);

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(abountAction);
    helpMenu->addAction(abountQtAction);

    connect(quitAction,&QAction::triggered,this,&MainWindow::close);
    connect(addAccountAction,&QAction::triggered,this,&MainWindow::accountAdd);
    //connect()
    connect(abountAction,SIGNAL(triggered(bool)),this,SLOT(about()));
    connect(abountQtAction,SIGNAL(triggered(bool)),qApp,SLOT(aboutQt()));

}



/***slots ***/

void MainWindow::about()
{
    QMessageBox::about(this, tr("About Distributor Manager"),
            tr("<p>The <b>Distributor Manager</b> shows how to manage "
               "user data,account data and server data. </p>"));
}
void MainWindow::serverSumbit()
{
    serverModel->database().transaction();
    if(serverModel->submitAll())
    {
        serverModel->database().commit();
    }
    else
    {
        serverModel->database().rollback();
        showError(serverModel->lastError());
    }
}

void MainWindow::accountSubmit()
{
    accountModel->database().transaction();
    if(accountModel->submitAll())
    {
        accountModel->database().commit();
    }
    else
    {
        accountModel->database().rollback();
        showError(accountModel->lastError());
    }
}

void MainWindow::accountAdd()
{

    AccountDialog *dialog = new AccountDialog(accountModel,this);
    int accepted = dialog->exec();
    if(accepted==1)
    {
        int lastRow = accountModel->rowCount()-1;
        accountView->selectRow(lastRow);
        accountView->scrollToBottom();
    }
    else
    {
        QSqlError err = accountModel->lastError();
        QString errText = err.text().trimmed();

        if(errText.isEmpty())
        {
            return;
        }
        showError(err);
    }
}

void MainWindow::accountDelete()
{
    QModelIndexList selection = accountView->selectionModel()->selectedRows(0);
    if(!selection.empty())
    {
        QModelIndex index = selection.at(0);
        accountModel->removeRow(index.row());
        accountModel->database().transaction();
        if(accountModel->submitAll())
        {
            accountModel->database().commit();
        }
        else
        {
            accountModel->database().rollback();
            showError(accountModel->lastError());
        }
    }
    else
    {
        QMessageBox::information(this,"提示","请选择要删除的账号");
    }
}
