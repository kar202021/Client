#ifndef CLIENTLOGIC_H
#define CLIENTLOGIC_H

#include <QObject>
#include <QTcpSocket>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDateTime>

class ClientLogic : public QObject
{
    Q_OBJECT
public:
    explicit ClientLogic(QObject *parent = nullptr);
    ~ClientLogic();

    void connectToServer(const QString &host, quint16 port);
    void disconnectFromServer();
    void sendMessage(const QString &message);

    bool isConnected() const;

signals:
    void connectionStatusChanged(bool connected);
    void logMessage(const QString &message);
    void messageReceived(const QString &sender, const QString &message);

private slots:
    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void attemptReconnect();

private:
    QTcpSocket *m_socket;
    QTimer *m_reconnectTimer;
    QString m_host;
    quint16 m_port;
    bool m_autoReconnect;
    bool m_connectionAttempted = false;
};

#endif // CLIENTLOGIC_H
