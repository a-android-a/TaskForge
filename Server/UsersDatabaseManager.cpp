#include"UsersDatabaseManager.h"
#include"User.h"
#include"SslServer.h"
User UsersDatabaseManager::getUserByLoginHash(const QString& loginHash)
{
    QVariantMap params;
    params[":loginHash"] = loginHash;


    QVariantMap row = getFirstRow(
        "SELECT id, created_at, name, surname, job_title, "
        "       login_hash, password_hash, is_banned, password_salt "
        "FROM Users "
        "WHERE login_hash = :loginHash",
        params
        );

    if (row.isEmpty()) {
        return User();
        qInfo()<<"User, not found";
    }

    User u;
    u.id            = row.value("id").toInt();
    u.createdAt     = QDateTime::fromString(row.value("created_at").toString(), Qt::ISODate);
    u.name          = row.value("name").toString();
    u.surname       = row.value("surname").toString();
    u.jobTitle      = row.value("job_title").toString();
    u.loginHash     = row.value("login_hash").toString();
    u.passwordHash  = row.value("password_hash").toString();
    u.password_salt = row.value("password_salt").toString();
    u.isBanned      = row.value("is_banned").toInt();

    return u;
}

bool UsersDatabaseManager::createUser(
    const QString& name,
    const QString& surname,
    const QString& jobTitle,
    const QString& login,
    const QString& password)
{
    // 1. Хешируем логин
    QString loginHash = SslServer::toSha256(login);

    // 2. Генерируем соль
    QByteArray salt = SslServer::generateSalt(16);
    QString saltBase64 = salt.toBase64();

    // 3. Хешируем пароль SHA256
    QString passwordSha256 = SslServer::toSha256(password);

    // 4. Хешируем PBKDF2(passwordSha256, salt)
    QString passwordHash = SslServer::hashPasswordPBKDF2(passwordSha256, salt);

    // 5. Сохраняем в базу
    qint64 newId = -1;

    bool ok = execInsertGetId(
        "INSERT INTO \"Users\" ("
        "   created_at, name, surname, job_title, "
        "   login_hash, password_hash, password_salt, is_banned"
        ") VALUES ("
        "   CURRENT_TIMESTAMP, :name, :surname, :job_title, "
        "   :login_hash, :password_hash, :password_salt, 0"
        ")",
        {
            {":name",          name},
            {":surname",       surname},
            {":job_title",     jobTitle},
            {":login_hash",    loginHash},
            {":password_hash", passwordHash},
            {":password_salt", saltBase64}
        },
        newId
        );

    return ok && newId > 0;
}
QList<User> UsersDatabaseManager::getAllUsers()
{
    QList<User> users;

    QSqlQuery query(this->m_db);
    query.prepare("SELECT id, created_at, name, surname, job_title, "
                  "login_hash, password_hash, is_banned, password_salt "
                  "FROM Users");

    if (!query.exec()) {
        qWarning() << "getAllUsers error:" << query.lastError().text();
        return users;
    }

    while (query.next()) {
        User u;
        u.id            = query.value("id").toInt();
        u.createdAt     = QDateTime::fromString(query.value("created_at").toString(), Qt::ISODate);
        u.name          = query.value("name").toString();
        u.surname       = query.value("surname").toString();
        u.jobTitle      = query.value("job_title").toString();
        u.loginHash     = query.value("login_hash").toString();
        u.passwordHash  = query.value("password_hash").toString();
        u.isBanned      = query.value("is_banned").toInt();
        u.password_salt = query.value("password_salt").toString();

        users.append(u);
    }

    return users;
}


bool UsersDatabaseManager::setBannedStatus(int id, bool banned)
{
    return exec(
        "UPDATE Users SET is_banned = :b WHERE id = :id",
        {{":b", banned ? 1 : 0}, {":id", id}}
        );
}

