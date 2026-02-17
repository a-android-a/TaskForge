#pragma once

#include <QSslSocket>
#include <QString>


class ApiClient:public QSslSocket{
    Q_OBJECT;
private:
    QString m_hostName;
    int m_nPort;
public:
    ApiClient(QObject* parent);
    void setHostName  (const QString &hostName);
    void setHostPort  (const uint16_t  port);
    void sendToServer (const QString &data);
    void connectToServer();
    void authenticate();
};
