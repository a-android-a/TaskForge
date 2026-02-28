#pragma once

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QString>
#include <QVariantMap>
#include <QVariant>


class BaseDatabaseManager
{
public:
    explicit BaseDatabaseManager(const QString &dbPath,
                                 const QString &connectionName = QString());
    virtual ~BaseDatabaseManager();


    bool open();
    bool isOpen() const;
    void close();




    bool exec(const QString &sql,
              const QVariantMap &params = QVariantMap());


    bool execInsertGetId(const QString &sql,
                         const QVariantMap &params,
                         qint64 &outLastInsertId);


    QVariant scalar(const QString &sql,
                                   const QVariantMap &params = QVariantMap());


    QVariantMap getFirstRow(const QString &sql,
                                           const QVariantMap &params = QVariantMap());


   QVector<QVariantMap> getRows(const QString &sql,
                                     const QVariantMap &params = QVariantMap());


    QSqlQuery select(const QString &sql,
                     const QVariantMap &params = QVariantMap());


    QString lastErrorText() const;

protected:

    bool prepareAndBind(QSqlQuery &query,
                        const QString &sql,
                        const QVariantMap &params);

    QString m_dbPath;
    QString m_connectionName;
    QSqlDatabase m_db;
};


