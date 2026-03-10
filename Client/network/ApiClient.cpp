#include "ApiClient.h"
#include "../Config.h"

#include <QSslConfiguration>
#include <QList>
#include <QAbstractSocket>
#include <QMessageBox>
#include <QCoreApplication>
#include <QByteArray>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>

ApiClient::ApiClient(QObject* parent): QSslSocket(parent)
{
    connect(this, &QSslSocket::readyRead, this, &ApiClient::slotReadyRead);
    connect(this, &QSslSocket::errorOccurred,this, [](QAbstractSocket::SocketError err){ qWarning() << "Error:" << err; });

}
void ApiClient::authenticate(const QString &login, const QString password){
    QJsonObject messageObj;

    messageObj["type"    ] = "authorization";
    messageObj["time"    ] =  QTime::currentTime().toString("hh:mm:ss");;
    messageObj["login"   ] = login;
    messageObj["password"] = password;

    QJsonDocument doc(messageObj);


    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
    jsonData += '\n';
    this->write(jsonData);
}
void ApiClient::connectToServer()
{
    Config::instance().setSettingsFile("config.json");

    qInfo()<< Config::instance().getServerHost();
    qInfo()<< Config::instance().getServerPort();
    qInfo()<< Config::instance().getCertificate();

    QSslConfiguration sslConfig = this->sslConfiguration();
    QList<QSslCertificate> caCertificates = sslConfig.caCertificates();
    QList<QSslCertificate> newCerts = QSslCertificate::fromPath(Config::instance().getCertificate());

    if (newCerts.isEmpty()) {

        QMessageBox::warning(nullptr,
                             tr("Certificate Error"),
                             tr("Failed to load server certificate (server.crt).\n"
                                "Please ensure the file is in the application directory."));


    } else {

        caCertificates.append(newCerts.constFirst());

    }
    sslConfig.setCaCertificates(caCertificates);
    this->setSslConfiguration(sslConfig);


    connect(this, &QAbstractSocket::errorOccurred, this, [this](QAbstractSocket::SocketError err) {
        QString errMsg = tr("Failed to connect to server:\n%1").arg(this->errorString());
        QMessageBox::warning(nullptr, tr("Connection Error"), errMsg);
    });


    this->connectToHostEncrypted(Config::instance().getServerHost(), 4433);

}
void ApiClient::sendToServer(const QString &data)
{

    QJsonObject messageObj;

    messageObj["type"] = "text";
    messageObj["time"] =  QTime::currentTime().toString("hh:mm:ss");;


    QJsonDocument doc(messageObj);


    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
    jsonData += '\n';

    this->write(jsonData);


}
void ApiClient::slotReadyRead(){

        QByteArray rawLine = this->readAll();
        QString message = QString::fromUtf8(rawLine).trimmed();
        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(message .toUtf8(), &parseError);

        if (parseError.error != QJsonParseError::NoError) {
            qDebug() << "JSON error:" << parseError.errorString();
           // continue;
        }


        if (!doc.isObject()) {

          //  continue;
        }

        QJsonObject obj = doc.object();

        QString type = obj["type"].toString();

        qInfo()<<type;
        if(type == "authorization_response"){
            QString status = obj["status"].toString();
            if(status == "ok"){
                emit authorizationOk();
            }else  {

                qInfo()<<"status:error";
                emit authorizationFailed();
            }
        }



}
