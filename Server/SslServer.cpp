#include "SslServer.h"
#include <QFile>
#include <QSslKey>
#include <QSslCertificate>
#include <QSslSocket>
SslServer::SslServer(QObject *parent)
    : QTcpServer(parent)
{

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
        qWarning() << "Error: the server.key was not found in the current folder!";
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
        qWarning() << "Error: the server.crt was not found in the current folder!";
        delete sslSocket;
        return;
    }
    QSslCertificate cert(certFile.readAll());
    if (cert.isNull()) {
        qWarning() << "Error: invalid certificate!";
        delete sslSocket;
        return;
    }
    sslSocket->setLocalCertificate(cert);


    if (!sslSocket->setSocketDescriptor(socketDescriptor)) {
        qWarning() << "setSocketDescriptor failed:" << sslSocket->errorString();
        delete sslSocket;
        return;
    }


    connect(sslSocket, &QSslSocket::encrypted,    this, &SslServer::onEncrypted);
    connect(sslSocket, &QSslSocket::readyRead,    this, &SslServer::onReadyRead);
    connect(sslSocket, &QSslSocket::disconnected, this, &SslServer::onDisconnected);
    connect(sslSocket,  QOverload<const QList<QSslError>&>::of(&QSslSocket::sslErrors),
            this, &SslServer::onSslErrors);


    sslSocket->startServerEncryption();


}

void SslServer::onEncrypted()
{
    QSslSocket *socket = qobject_cast<QSslSocket*>(sender());
    if (!socket) return;



    m_clients.append(socket);

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


        QByteArray broadcast = data;  /
        for (QSslSocket *client : qAsConst(m_clients)) {
            if (client->state() == QAbstractSocket::ConnectedState) {
                client->write(broadcast);
            }
        }
    }
}

void SslServer::onDisconnected()
{
    QSslSocket *socket = qobject_cast<QSslSocket*>(sender());
    if (!socket) return;

    qInfo() << "the client has disconnected" << socket->peerAddress().toString();
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
