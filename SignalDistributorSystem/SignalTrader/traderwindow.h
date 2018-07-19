#ifndef TRADERWINDOW_H
#define TRADERWINDOW_H

#include <QMainWindow>
#include<QTcpSocket>

QT_BEGIN_NAMESPACE
class QComboBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QTcpSocket;
class QNetworkSession;

QT_END_NAMESPACE
namespace Ui {
class TraderWindow;
}

class TraderWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit TraderWindow(QWidget *parent = 0);
    ~TraderWindow();

private slots:
    void requestNewMsg();
    void readMsg();
    void displayError(QAbstractSocket::SocketError socketErr);
    void enableGetMsgButton();
    void sessionOpened();
private:
    Ui::TraderWindow *ui;
    QComboBox *cboHost;
    QLineEdit *txtPort;
    QLabel *lblStatus;
    QPushButton *btnGetMsg;

    QTcpSocket * tcpSocket;
    QString currMsg;
    quint16 blockSize;

    QNetworkSession *netSession;
};

#endif // TRADERWINDOW_H
