#pragma once

#include <QTcpServer>
#include <QSslSocket>
#include <QList>
#include <QDebug>
#include <QString>
#include <QByteArray>
#include "UsersDatabaseManager.h"
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
    QList<QSslSocket*> m_clients;
    User       authenticate      (const QString &passwd,   const QString &login ,int &statusCode );
    QString    hashPasswordPBKDF2(const QString &password, const QByteArray &salt);
    QByteArray generateSalt      (int bytes = 16);
    QString toSha256(const QString &str);
    UsersDatabaseManager usersDB;
};
