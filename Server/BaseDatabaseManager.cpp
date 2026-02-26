#include "BaseDatabaseManager.h"
#include <QDebug>
#include <QUuid>

BaseDatabaseManager::BaseDatabaseManager(const QString &dbPath,
                                         const QString &connectionName)
    : m_dbPath(dbPath)
{

    m_connectionName = connectionName.isEmpty()
                           ? "DB_" + QUuid::createUuid().toString(QUuid::WithoutBraces).left(12)
                           : connectionName;
}

BaseDatabaseManager::~BaseDatabaseManager()
{
    close();
}

bool BaseDatabaseManager::open()
{
    if (isOpen()) {
        return true;
    }

    m_db = QSqlDatabase::addDatabase("QSQLITE", m_connectionName);
    m_db.setDatabaseName(m_dbPath);

    if (!m_db.open()) {
        qWarning() << "Couldn't open the database " << m_dbPath << " -> " << m_db.lastError().text();
        return false;
    }

    qDebug() << "The base is open:" << m_dbPath << "(connection:" << m_connectionName << ")";
    return true;
}

bool BaseDatabaseManager::isOpen() const
{
    return m_db.isValid() && m_db.isOpen();
}

void BaseDatabaseManager::close()
{
    if (isOpen()) {
        m_db.close();
    }
    // if (QSqlDatabase::contains(m_connectionName)) {
    //     QSqlDatabase::removeDatabase(m_connectionName);
    // }
}

bool BaseDatabaseManager::prepareAndBind(QSqlQuery &query,
                                         const QString &sql,
                                         const QVariantMap &params)
{
    if (!query.prepare(sql)) {
        qWarning() << "Error prepare:" << query.lastError().text()
            << "\nSQL:" << sql;
        return false;
    }

    for (auto it = params.constBegin(); it != params.constEnd(); ++it) {
        query.bindValue(it.key(), it.value());
    }

    return true;
}

bool BaseDatabaseManager::exec(const QString &sql, const QVariantMap &params)
{
    QSqlQuery query(m_db);
    if (!prepareAndBind(query, sql, params)) {
        return false;
    }

    if (!query.exec()) {
        qWarning() << "Error exec:" << query.lastError().text()
            << "\nSQL:" << sql.simplified();
        return false;
    }

    return true;
}

bool BaseDatabaseManager::execInsertGetId(const QString &sql,
                                          const QVariantMap &params,
                                          qint64 &outLastInsertId)
{
    if (!exec(sql, params)) {
        return false;
    }

    QSqlQuery query(m_db);
    query.exec("SELECT last_insert_rowid()");
    if (query.next()) {
        outLastInsertId = query.value(0).toLongLong();
        return true;
    }

    return false;
}

std::optional<QVariant> BaseDatabaseManager::scalar(const QString &sql,
                                                    const QVariantMap &params)
{
    QSqlQuery query = select(sql, params);
    if (query.next()) {
        return query.value(0);
    }
    return std::nullopt;
}

std::optional<QVariantMap> BaseDatabaseManager::getFirstRow(const QString &sql,
                                                            const QVariantMap &params)
{
    QSqlQuery query = select(sql, params);
    if (!query.next()) {
        return std::nullopt;
    }

    QVariantMap row;
    QSqlRecord rec = query.record();
    for (int i = 0; i < rec.count(); ++i) {
        row[rec.fieldName(i)] = query.value(i);
    }

    return row;
}

std::vector<QVariantMap> BaseDatabaseManager::getRows(const QString &sql,
                                                      const QVariantMap &params)
{
    std::vector<QVariantMap> result;
    QSqlQuery query = select(sql, params);

    while (query.next()) {
        QVariantMap row;
        QSqlRecord rec = query.record();
        for (int i = 0; i < rec.count(); ++i) {
            row[rec.fieldName(i)] = query.value(i);
        }
        result.push_back(std::move(row));
    }

    return result;
}

QSqlQuery BaseDatabaseManager::select(const QString &sql,
                                      const QVariantMap &params)
{
    QSqlQuery query(m_db);
    if (!prepareAndBind(query, sql, params)) {

        return query;
    }

    if (!query.exec()) {
        qWarning() << "error select:" << query.lastError().text()
            << "\nSQL:" << sql.simplified();
    }

    return query;
}

QString BaseDatabaseManager::lastErrorText() const
{
    if (m_db.isValid()) {
        return m_db.lastError().text();
    }
    return "No connection active";
}
