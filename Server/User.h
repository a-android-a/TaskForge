#pragma once
#include <QString>
#include <QDateTime>
#include <QJsonObject>
#include <optional>

struct User
{
    int         id          = -1;
    QDateTime   createdAt;
    QString     name;
    QString     surname;
    QString     jobTitle;
    QString     loginHash;
    QString     passwordHash;
    QString     password_salt;
    bool        isBanned        = false;

    bool isValid() const { return id > 0; }

    QString fullName() const {
        if (name.isEmpty() && surname.isEmpty()) return "";
        if (name.isEmpty()) return surname;
        if (surname.isEmpty()) return name;
        return name + " " + surname;
    }


    QJsonObject toClientJson() const
    {
        QJsonObject obj;
        obj["id"]         = id;
        obj["createdAt"]  = createdAt.toString(Qt::ISODate);
        obj["name"]       = name;
        obj["surname"]    = surname;
        obj["jobTitle"]   = jobTitle;
        obj["fullName"]   = fullName();
        obj["isBanned"]   = isBanned;
        return obj;
    }
};
