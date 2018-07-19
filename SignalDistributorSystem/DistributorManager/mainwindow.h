#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QComboBox;
class QDataWidgetMapper;
class QItemSelectionModel;
class QLabel;
class QLineEdit;
class QPushButton;
class QSqlRelationalTableModel;
class QSqlTableModel;
class QStringListModel;
class QTextEdit;
class QDialogButtonBox;
class QSqlError;
class QTableView;
QT_END_NAMESPACE

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void about();
    void serverSumbit();

    void accountSubmit();
    void accountAdd();
    void accountDelete();

private:

    Ui::MainWindow *ui;
    void showError(const QSqlError& error);
    void createMenuBar();
    //初始化服务器管理界面
    void initServerPage();
    //初始化用户管理界面
    void initUserPage();
    //初始化账户管理界面
    void initAccountPage();
private:

    //服务器管理界面
    QSqlTableModel *serverModel;
    QPushButton *btnSubmitServer;
    QPushButton *btnRevertServer;
    QDialogButtonBox *serverBox;

    //账户管理
    QSqlRelationalTableModel *accountModel;
    QTableView *accountView;
    QPushButton *btnSubmitAccount;
    QPushButton * btnRevertAccount;
    QPushButton *btnAddAccount;
    QPushButton *btnDeleteAccount;
    QDialogButtonBox *accountBox;
};

#endif // MAINWINDOW_H
