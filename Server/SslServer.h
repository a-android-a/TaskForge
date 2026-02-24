#pragma once

#include <QTcpServer>
#include <QSslSocket>
#include <QList>
#include <QDebug>

class SslServer : public QTcpServer
{
    Q_OBJECT

public:
    explicit SslServer(QObject *parent = nullptr);
    ~SslServer() override;

protected:
    void incomingConnection(qintptr socketDescriptor) override;

private slots:
    void onEncrypted();
    void onReadyRead();
    void onDisconnected();
    void onSslErrors(const QList<QSslError> &errors);

private:
    QList<QSslSocket*> m_clients;  // Все подключённые клиенты
};
