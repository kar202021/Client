#ifndef CLIENTWINDOW_H
#define CLIENTWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDateTime>
#include <QThread>
#include <QtEndian>

QT_BEGIN_NAMESPACE
namespace Ui { class ClientWindow; }
QT_END_NAMESPACE

class ClientWindow : public QMainWindow
{
    Q_OBJECT

public:
    ClientWindow(QWidget *parent = nullptr);
    ~ClientWindow();

private slots:
    void connectToServer();
    void readData();
    void sendData();
    void disconnected();
    void handleSocketError(QAbstractSocket::SocketError error);

private:
    Ui::ClientWindow *ui;
    QTcpSocket *socket;

    QByteArray buffer;
    quint32 expectedSize = 0;
};

#endif // CLIENTWINDOW_H
