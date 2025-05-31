#include "clientwindow.h"
#include "ui_clientwindow.h"

ClientWindow::ClientWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ClientWindow),
    m_logic(new ClientLogic(this))
{
    ui->setupUi(this);

    // Connect signals from logic to UI
    connect(m_logic, &ClientLogic::connectionStatusChanged,
            this, &ClientWindow::updateConnectionStatus);
    connect(m_logic, &ClientLogic::messageReceived,
            this, &ClientWindow::displayMessage);
    connect(m_logic, &ClientLogic::logMessage,
            this, &ClientWindow::logMessage);

    // Set default values
    ui->hostLineEdit->setText("127.0.0.1");
    ui->portSpinBox->setValue(12345);
}

ClientWindow::~ClientWindow()
{
    delete ui;
}

void ClientWindow::on_connectButton_clicked()
{
    if(m_logic->isConnected()) {
        m_logic->disconnectFromServer();
        ui->connectButton->setText("Connect");
    } else {
        QString host = ui->hostLineEdit->text();
        quint16 port = ui->portSpinBox->value();
        m_logic->connectToServer(host, port);
        ui->connectButton->setText("Disconnect");
    }
}

void ClientWindow::on_sendButton_clicked()
{
    QString message = ui->messageLineEdit->text();
    if(!message.isEmpty()) {
        m_logic->sendMessage(message);
        ui->messageLineEdit->clear();
    }
}

void ClientWindow::updateConnectionStatus(bool connected)
{
    ui->statusLabel->setText(connected ? "Connected" : "Disconnected");
    ui->connectButton->setText(connected ? "Disconnect" : "Connect");
}

void ClientWindow::displayMessage(const QString &sender, const QString &message)
{
    ui->chatTextEdit->append(QString("[%1] %2").arg(sender, message));
}

void ClientWindow::logMessage(const QString &message)
{
    ui->logTextEdit->append(message);
}
