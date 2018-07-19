#ifndef DISTRIBUTORWINDOW_H
#define DISTRIBUTORWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QLabel;
class QPushButton;
class QTcpServer;
class QNetworkSession;

QT_END_NAMESPACE
namespace Ui {
class DistributorWindow;
}

class DistributorWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit DistributorWindow(QWidget *parent = 0);
    ~DistributorWindow();

private slots:
    void sessionOpened();
    void SendMsg();
    void on_btnQuit_clicked();

private:
    Ui::DistributorWindow *ui;
    //QLabel *lblStatus;

private:
    QTcpServer *tcpServer;
    QStringList msgs;
    QNetworkSession *netSession;
};

#endif // DISTRIBUTORWINDOW_H
