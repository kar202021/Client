#ifndef CLIENTWINDOW_H
#define CLIENTWINDOW_H

#include <QMainWindow>
#include "business/clientlogic.h"

namespace Ui {
class ClientWindow;
}

class ClientWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ClientWindow(QWidget *parent = nullptr);
    ~ClientWindow();

private slots:
    void on_connectButton_clicked();
    void on_sendButton_clicked();
    void updateConnectionStatus(bool connected);
    void displayMessage(const QString &sender, const QString &message);
    void logMessage(const QString &message);

private:
    Ui::ClientWindow *ui;
    ClientLogic *m_logic;
};

#endif // CLIENTWINDOW_H
