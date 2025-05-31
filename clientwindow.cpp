#include "clientwindow.h"
#include "ui_clientwindow.h"


ClientWindow::ClientWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ClientWindow)
{
    ui->setupUi(this);
    
    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::readyRead, this, &ClientWindow::readData);
    connect(socket, &QTcpSocket::disconnected, this, &ClientWindow::disconnected);
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(handleSocketError(QAbstractSocket::SocketError)));
    connectToServer();
}
void ClientWindow::handleSocketError(QAbstractSocket::SocketError error) {
    Q_UNUSED(error);  // Avoid unused parameter warning
    ui->textEdit->append("Socket Error: " + socket->errorString());
}
void ClientWindow::connectToServer() {
    while (true) {
        QString ip = ui->lineEdit_IP->text();  // Get IP from UI
        int port = ui->spinBox_Port->value();  // Get port from UI
        socket->connectToHost(ip, port); // ("127.0.0.1", 12345);
        if (socket->waitForConnected(1000)) {
            ui->textEdit->append("Connected to server");
            break;
        } else {
            ui->textEdit->append("Retrying in 1 seconds...");
            QThread::sleep(5);  // Wait before retrying
        }
    }
}
ClientWindow::~ClientWindow()
{
    delete ui;
}
void ClientWindow::readData() {
    buffer += socket->readAll();

    while (true) {
        // 1. Wait until we have at least 4 bytes (header)
        if (expectedSize == 0 && buffer.size() >= 4) {
//            expectedSize = qFromBigEndian<quint32>(buffer.mid(0, 4).constData());
            expectedSize = qFromBigEndian(*reinterpret_cast<const quint32*>(buffer.mid(0, 4).constData()));
            buffer.remove(0, 4);  // Remove header
        }

        // 2. Wait until full message is received
    if (expectedSize > 0 && static_cast<quint32>(buffer.size()) >= expectedSize) {
    QByteArray jsonData = buffer.mid(0, expectedSize);
            buffer.remove(0, expectedSize);
            expectedSize = 0;  // Reset for next message

            // Process JSON
            QJsonDocument doc = QJsonDocument::fromJson(jsonData);
            if (!doc.isNull()) {
                QJsonObject obj = doc.object();
                ui->textEdit->append("Received: " + obj["message"].toString());
            } else {
                ui->textEdit->append("Invalid JSON received!");
            }
        } else {
            break;  // Wait for more data
        }
    }
}
void ClientWindow::sendData() {
    if (socket->state() == QAbstractSocket::ConnectedState) {
        QJsonObject obj;
        obj["message"] = ui->lineEdit->text();
        obj["timestamp"] = QDateTime::currentDateTime().toString();

        QJsonDocument doc(obj);
        QByteArray data = doc.toJson();

        // Prepend 4-byte message length (big-endian)
        QByteArray header;
        header.resize(4);
        qToBigEndian<quint32>(data.size(), header.data());

        socket->write(header + data);  // Send [HEADER][JSON]
        ui->textEdit->append("Sent: " + ui->lineEdit->text());
    }
}
void ClientWindow::disconnected()
{
    ui->textEdit->append("Disconnected from server");
}
