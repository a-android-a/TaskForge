#pragma once

#include <QSslSocket>
#include <QString>


class ApiClient:public QSslSocket{
    Q_OBJECT;
private:
    QString m_nameHost;
    int m_nPort;
public:
    ApiClient(QObject* parent);
    void setHostName  (const QString &hostName);
    void setHostPort(const uint16_t  port);
};
