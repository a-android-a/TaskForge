#include "SslServer.h"
#include "UsersDatabaseManager.h"
#include <QFile>
#include <QSslKey>
#include <QSslCertificate>
#include <QSslSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>
#include <QSqlQuery>

SslServer::SslServer(QObject *parent):QTcpServer(parent)
{
    UsersDatabaseManager usersDB("DB/Users.db");
    usersDB.open();
    if(!usersDB.isOpen()){
       qWarning() << "Failed to open Users database" << usersDB.lastErrorText();
    }
}

SslServer::~SslServer()
{
    for (auto *client : m_clients) {
        client->disconnectFromHost();
    }
}

void SslServer::incomingConnection(qintptr socketDescriptor)
{
    QSslSocket *sslSocket = new QSslSocket(this);


    QFile keyFile("server.key");
    if (!keyFile.open(QIODevice::ReadOnly)) {
        qWarning() << "Mistake: the server.key was not found in the current folder!";
        delete sslSocket;
        return;
    }
    QSslKey key(keyFile.readAll(), QSsl::Rsa, QSsl::Pem, QSsl::PrivateKey);
    if (key.isNull()) {
        qWarning() << "Error: invalid or encrypted private key!";
        delete sslSocket;
        return;
    }
    sslSocket->setPrivateKey(key);

    QFile certFile("server.crt");
    if (!certFile.open(QIODevice::ReadOnly)) {
        qWarning() << "Mistake: the server.crt was not found in the current folder!";
        delete sslSocket;
        return;
    }
    QSslCertificate cert(certFile.readAll());
    if (cert.isNull()) {
        qWarning() << "Error: Invalid certificate!";
        delete sslSocket;
        return;
    }
    sslSocket->setLocalCertificate(cert);


    if (!sslSocket->setSocketDescriptor(socketDescriptor)) {
        qWarning() << "setSocketDescriptor failed:" << sslSocket->errorString();
        delete sslSocket;
        return;
    }


    connect(sslSocket, &QSslSocket::encrypted, this, &SslServer::onEncrypted);
    connect(sslSocket, &QSslSocket::readyRead, this, &SslServer::onReadyRead);
    connect(sslSocket, &QSslSocket::disconnected, this, &SslServer::onDisconnected);
    connect(sslSocket, QOverload<const QList<QSslError>&>::of(&QSslSocket::sslErrors),
            this, &SslServer::onSslErrors);


    sslSocket->startServerEncryption();

    qDebug() << "New connection: waiting for SSL handshake...";
}

void SslServer::onEncrypted()
{
    QSslSocket *socket = qobject_cast<QSslSocket*>(sender());
    if (!socket) return;

    qDebug().nospace() << "SSL handshake is successful! The client is connected:"
                       << socket->peerAddress().toString() << ":" << socket->peerPort();

    m_clients.append(socket);
    socket->write("Hi");
}

void SslServer::onReadyRead()
{
    QSslSocket *socket = qobject_cast<QSslSocket*>(sender());
    if (!socket) return;

    while (socket->canReadLine()) {
        QByteArray data = socket->readLine();
        if (data.isEmpty()) continue;

        QString message = QString::fromUtf8(data).trimmed();
        qInfo()<< "Message from" << socket->peerAddress().toString() << ":" << message;

        QJsonParseError parseError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(message.toUtf8(), &parseError);

        if (parseError.error != QJsonParseError::NoError) {
            qWarning() << "JSON parse error:" << parseError.errorString();
            return;
        }


        if (!jsonDoc.isObject()) {
            qWarning() << "JSON is not an object";
            return;
        }

        QJsonObject jsonObj = jsonDoc.object();


        QString type = jsonObj.value("type").toString();



        if (type == "authorization") {
            qInfo()<<"authorization";

        } else if (type == "command") {

        }


    }
}

void SslServer::onDisconnected()
{
    QSslSocket *socket = qobject_cast<QSslSocket*>(sender());
    if (!socket) return;

    qInfo() << "The client has disconnected:" << socket->peerAddress().toString();
    m_clients.removeOne(socket);
    socket->deleteLater();
}

void SslServer::onSslErrors(const QList<QSslError> &errors)
{
    QSslSocket *socket = qobject_cast<QSslSocket*>(sender());
    if (!socket) return;

    for (const QSslError &error : errors) {
        qWarning() << "SSL error:" << error.errorString();
    }


    socket->ignoreSslErrors();
}
