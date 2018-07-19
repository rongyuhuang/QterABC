#include"accountdialog.h"

AccountDialog::AccountDialog(QSqlRelationalTableModel *accountModel, QWidget *parent)
    :QDialog(parent)
{
    accModel = accountModel;
    QGroupBox *inputBox = initInputWidgets();
    QDialogButtonBox* buttonBox = initButtons();

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(inputBox);
    layout->addWidget(buttonBox);
    setLayout(layout);
    setWindowTitle("Add Account");
}


/***private slots**/

void AccountDialog::addAccount()
{
    QString accName = txtAccName->text();
    QString accID = txtAccID->text();
    QString password = txtPassword->text();

    if(accName.isEmpty()||accID.isEmpty()||password.isEmpty())
    {
        QMessageBox::information(this,tr("提示"),tr("Both of account name,account id and password can not be empty"),QMessageBox::StandardButton::Ok);
    }
    else
    {
    QModelIndex serverIndex = accModel->relationModel(4)->index(
                cboServer->currentIndex(),0);
    int serverID = serverIndex.data().toInt();
    int rowCount = accModel->rowCount();
    int colCount = accModel->columnCount();

    accModel->insertRows(rowCount,1);
    accModel->setData(accModel->index(rowCount,0),rowCount+1);
    accModel->setData(accModel->index(rowCount,1),accName);
    accModel->setData(accModel->index(rowCount,2),accID);
    accModel->setData(accModel->index(rowCount,3),password);
    accModel->setData(accModel->index(rowCount,4),serverID);


    accModel->database().transaction();
    if(accModel->submitAll())
    {
        accModel->database().commit();
    }
    else
    {
        accModel->database().rollback();
    }
    accept();
    }
}
/***private function***/


QGroupBox* AccountDialog::initInputWidgets()
{
    QGroupBox* box = new QGroupBox(tr("Account Information"));

    QLabel *lblAccName = new QLabel(tr("Account Name"));
    QLabel *lblAccID = new QLabel(tr("Account ID"));
    QLabel *lblPassword = new QLabel(tr("Password"));
    QLabel *lblServer = new QLabel(tr("Server Name"));

    txtAccName = new QLineEdit;
    lblAccName->setBuddy(txtAccName);
    txtAccID = new QLineEdit;
    lblAccID->setBuddy(txtAccID);
    txtPassword = new QLineEdit;
    txtPassword->setEchoMode(QLineEdit::PasswordEchoOnEdit);//编辑时正常显示，完成编辑后，根据平台以密码形式显示
    lblPassword->setBuddy(txtPassword);

    cboServer = new QComboBox();
    cboServer->setModel(accModel->relationModel(4));
    cboServer->setModelColumn(8);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(lblAccName,0,0);
    layout->addWidget(txtAccName,0,1);
    layout->addWidget(lblAccID,1,0);
    layout->addWidget(txtAccID,1,1);
    layout->addWidget(lblPassword,2,0);
    layout->addWidget(txtPassword,2,1);
    layout->addWidget(lblServer,3,0);
    layout->addWidget(cboServer,3,1);

    box->setLayout(layout);

    return box;

}

QDialogButtonBox* AccountDialog::initButtons()
{
    QPushButton * btnAdd = new QPushButton(tr("&Add"));
    QPushButton *btnClose = new QPushButton(tr("&Close"));

    btnAdd->setDefault(true);

    connect(btnAdd,&QPushButton::clicked,this,&AccountDialog::addAccount);
    connect(btnClose,&QPushButton::clicked,this,&AccountDialog::close);

    QDialogButtonBox *box = new QDialogButtonBox;
    box->addButton(btnAdd,QDialogButtonBox::ResetRole);
    box->addButton(btnClose,QDialogButtonBox::RejectRole);

    return box;
}


