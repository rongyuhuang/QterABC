#ifndef ACCOUNTDIALOG_H
#define ACCOUNTDIALOG_H

#include<QtWidgets>
#include<QtSql>

class AccountDialog:public QDialog
{
    Q_OBJECT

public:
    AccountDialog(QSqlRelationalTableModel *accountModel,QWidget *parent=0);

private slots:
    void addAccount();

private:
    QGroupBox* initInputWidgets();
    QDialogButtonBox* initButtons();
private:
    QSqlRelationalTableModel *accModel;

    QLineEdit *txtAccName;
    QLineEdit *txtAccID;
    QLineEdit *txtPassword;
    QComboBox *cboServer;
};
#endif // ACCOUNTDIALOG_H
