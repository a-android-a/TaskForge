#pragma once

#include <QString>
#include <QJsonObject>
#include <QObject>

class Config : public QObject {
    Q_OBJECT

private:
    Config();

    bool parseJson(const QJsonObject& obj);

    QString m_serverHost      = "127.0.0.1";
    quint16 m_serverPort      = 8080;
    QString m_theme           = "dark";
    QString m_language        = "en";
    QString m_defaultUserName = "User";
    QString m_pCertificate    = "server.crt";

    QString m_filePath;

public:
    static Config& instance();
    bool load();
    bool save() const;

    //get
    QString getServerHost() const      { return m_serverHost; }
    quint16 getServerPort() const      { return m_serverPort; }
    QString getTheme() const           { return m_theme; }
    QString getLanguage() const        { return m_language; }
    QString getDefaultUserName() const { return m_defaultUserName; }
    QString getCertificate()     const {return m_pCertificate;}
    //set
    void setServerHost(const QString& host);
    void setServerPort(quint16 port);
    void setTheme(const QString& theme);
    void setSettingsFile(const QString& path);
    void setLanguage(const QString& lang);
    void setDefaultUserName(const QString& name);
    void setCertificate(const QString& cert);

signals:
    void configChanged();


};
