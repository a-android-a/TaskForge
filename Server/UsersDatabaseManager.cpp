#include"UsersDatabaseManager.h"

std::optional<User> UsersDatabaseManager::getUserByLoginHash(const QString& loginHash)
{
    auto row = getFirstRow(
        "SELECT id, created_at, name, surname, job_title, "
        "       login_hash, password_hash, is_banned "
        "FROM \"Users \" "
        "WHERE login_hash = :lh",
        {{":lh", loginHash}}
        );

    if (!row) {
        return std::nullopt;
    }

    User u;
    u.id           = row->value("id").toInt();
    u.createdAt    = QDateTime::fromString(row->value("created_at").toString(), Qt::ISODate);
    u.name         = row->value("name").toString();
    u.surname      = row->value("surname").toString();
    u.jobTitle     = row->value("job_title").toString();
    u.loginHash    = row->value("login_hash").toString();
    u.passwordHash = row->value("password_hash").toString();
    u.isBanned     = row->value("is_banned").toInt() != 0;

    return u;
}

bool UsersDatabaseManager::createUser(
    const QString& name,
    const QString& surname,
    const QString& jobTitle,
    const QString& loginHash,
    const QString& passwordHash)
{
    qint64 newId = -1;

    bool ok = execInsertGetId(
        "INSERT INTO \"Users \" ("
        "   created_at, name, surname, job_title, "
        "   login_hash, password_hash, is_banned"
        ") VALUES ("
        "   CURRENT_TIMESTAMP, :name, :surname, :job, "
        "   :lh, :ph, 0"
        ")",
        {
            {":name",    name},
            {":surname", surname},
            {":job",     jobTitle},
            {":lh",      loginHash},
            {":ph",      passwordHash}
        },
        newId
        );

    return ok && newId > 0;
}

bool UsersDatabaseManager::setBannedStatus(int id, bool banned)
{
    return exec(
        "UPDATE \"Users \" SET is_banned = :b WHERE id = :id",
        {{":b", banned ? 1 : 0}, {":id", id}}
        );
}
