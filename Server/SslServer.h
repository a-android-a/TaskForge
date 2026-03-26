#pragma once

#include <QTcpServer>
#include <QSslSocket>
#include <QList>
#include <QDebug>
#include <QString>
#include <QByteArray>
#include "UsersDatabaseManager.h"
#include "TaskDatabaseManager.h"
#include <QByteArray>
class SslServer : public QTcpServer
{
    Q_OBJECT

private:
    QList<QSslSocket*> m_clients;
    User       authenticate      (const QString &passwd,   const QString &login ,int &statusCode );



    UsersDatabaseManager usersDB;
    TaskDatabaseManager  tasksDB;
public:
    explicit SslServer(QObject *parent = nullptr);
    ~SslServer() override;
    static QByteArray generateSalt      (int bytes = 16);
    static QString    toSha256(const QString &str);
    static QString    hashPasswordPBKDF2(const QString &password, const QByteArray &salt);

protected:
    void incomingConnection(qintptr socketDescriptor) override;

private slots:
    void onEncrypted();
    void onReadyRead();
    void onDisconnected();
    void onSslErrors(const QList<QSslError> &errors);



};
