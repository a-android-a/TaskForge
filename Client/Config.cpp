#include "Config.h"
#include <QCoreApplication>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QDebug>

Config& Config::instance() {
    static Config config;
    return config;
}

Config::Config() {
    m_filePath = QCoreApplication::applicationDirPath() + "config.json";
}

bool Config::load() {
    QFile file(m_filePath);
    if (!file.exists() || !file.open(QIODevice::ReadOnly)) {
        qWarning() << "Config file not found or cannot open:" << m_filePath;
        return false;
    }

    QByteArray data = file.readAll();
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);

    if (error.error != QJsonParseError::NoError) {
        qWarning() << "JSON parse error:" << error.errorString();
        return false;
    }

    if (!doc.isObject()) {
        qWarning() << "Root is not JSON object";
        return false;
    }

    return parseJson(doc.object());
}

bool Config::parseJson(const QJsonObject& obj) {
    // server
    QJsonObject server = obj.value("server").toObject();
    m_serverHost = server.value("host").toString(m_serverHost);
    m_serverPort = server.value("port").toInt(m_serverPort);

    // appearance
    QJsonObject appearance = obj.value("appearance").toObject();
    m_theme    = appearance.value("theme").toString(m_theme);
    m_language = appearance.value("language").toString(m_language);

    // user
    QJsonObject user = obj.value("user").toObject();
    m_defaultUserName = user.value("defaultName").toString(m_defaultUserName);


    emit configChanged();
    return true;
}

bool Config::save() const {
    QJsonObject root;

    QJsonObject server;
    server["host"]        = m_serverHost;
    server["port"]        = m_serverPort;
    server["certificate"] = m_pCertificate;
    root  ["server"] = server;

    QJsonObject appearance;
    appearance["theme"]     = m_theme;
    appearance["language"]  = m_language;
    root      ["appearance"]= appearance;

    QJsonObject user;
    user["defaultName"] = m_defaultUserName;
    root["user"] = user;

    QJsonDocument doc(root);
    QByteArray data = doc.toJson(QJsonDocument::Indented);

    QFile file(m_filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Cannot write config:" << m_filePath;
        return false;
    }

    file.write(data);
    file.close();
    return true;
}


void Config::setServerHost(const QString& host) {
    if (m_serverHost != host) {
        m_serverHost = host;
        save();
        emit configChanged();
    }
}
void Config::setSettingsFile(const QString &path){
    m_filePath = path;
}

