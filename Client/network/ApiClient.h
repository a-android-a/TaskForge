#pragma once

#include <QSslSocket>
#include <QString>
#include"../Config.h"
#include "../../Server/User.h"
class ApiClient:public QSslSocket{
    Q_OBJECT;
private:
    QString m_hostName;
    int m_nPort;
    User user;

public:
    ApiClient(QObject* parent);
    void setHostName  (const QString &hostName);
    void setHostPort  (const uint16_t  port);
    void sendToServer (const QString &data);
    void connectToServer();
    void authenticate(const QString &login, const QString password);
signals:
    void authorizationFailed();
    void authorizationOk();
private slots:
    void slotReadyRead();

};
