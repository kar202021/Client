#include "clientlogic.h"

ClientLogic::ClientLogic(QObject *parent) : QObject(parent),
    m_socket(new QTcpSocket(this)),
    m_reconnectTimer(new QTimer(this)),
    m_autoReconnect(true)
{
    connect(m_socket, &QTcpSocket::connected, this, &ClientLogic::onConnected);
    connect(m_socket, &QTcpSocket::disconnected, this, &ClientLogic::onDisconnected);
    connect(m_socket, &QTcpSocket::readyRead, this, &ClientLogic::onReadyRead);
    connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::error),
            [this](QAbstractSocket::SocketError) {
        emit logMessage("Connection error: " + m_socket->errorString());
    });

    m_reconnectTimer->setInterval(5000);
    connect(m_reconnectTimer, &QTimer::timeout, this, &ClientLogic::attemptReconnect);
}

ClientLogic::~ClientLogic()
{
    disconnectFromServer();
}

void ClientLogic::connectToServer(const QString &host, quint16 port)
{
    if(m_socket->state() == QAbstractSocket::ConnectedState)
        return;

    m_host = host;
    m_port = port;

    emit logMessage(QString("Connecting to %1:%2...").arg(host).arg(port));
    m_socket->connectToHost(host, port);
}

void ClientLogic::disconnectFromServer()
{
    m_autoReconnect = false;
    m_reconnectTimer->stop();
    if(m_socket->state() == QAbstractSocket::ConnectedState) {
        m_socket->disconnectFromHost();
    }
}

void ClientLogic::sendMessage(const QString &message)
{
    if(!isConnected()) {
        emit logMessage("Not connected to server");
        return;
    }

    QJsonObject jsonObj;
    jsonObj["message"] = message;
    jsonObj["timestamp"] = QDateTime::currentDateTime().toString();

    QByteArray data = QJsonDocument(jsonObj).toJson();
    m_socket->write(data);

    emit logMessage("You: " + message);
}

bool ClientLogic::isConnected() const
{
    return m_socket->state() == QAbstractSocket::ConnectedState;
}

void ClientLogic::onConnected()
{
    m_reconnectTimer->stop();
    emit connectionStatusChanged(true);
    emit logMessage("Connected to server");
}

void ClientLogic::onDisconnected()
{
    emit connectionStatusChanged(false);
    emit logMessage("Disconnected from server");

    if(m_autoReconnect) {
        emit logMessage("Attempting to reconnect in 5 seconds...");
        m_reconnectTimer->start();
    }
}

void ClientLogic::onReadyRead()
{
    while(m_socket->bytesAvailable() > 0) {
        QByteArray data = m_socket->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if(doc.isNull()) {
            emit logMessage("Received invalid JSON data");
            continue;
        }

        QJsonObject obj = doc.object();
        QString sender = obj.contains("sender") ? obj["sender"].toString() : "Unknown";
        QString message = obj["message"].toString();

        emit messageReceived(sender, message);
    }
}

void ClientLogic::attemptReconnect()
{
    if(m_socket->state() != QAbstractSocket::ConnectedState) {
        emit logMessage("Reconnecting...");
        m_socket->connectToHost(m_host, m_port);
    }
}
