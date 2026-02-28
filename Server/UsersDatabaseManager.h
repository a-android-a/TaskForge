#pragma once

#include "BaseDatabaseManager.h"
#include "User.h"

class UsersDatabaseManager : public BaseDatabaseManager
{
public:
    explicit UsersDatabaseManager(const QString& path = "DB/Users.db"): BaseDatabaseManager(path, "users_conn") {}

    User getUserById(int id);

    User getUserByLoginHash(const QString& loginHash);

    bool createUser(
        const QString& name,
        const QString& surname,
        const QString& jobTitle,
        const QString& loginHash,
        const QString& PasswordHash,
        const QString& saltBase64);

    bool updateUserBasicInfo(int id,
                             const QString& name,
                             const QString& surname,
                             const QString& jobTitle);

    bool setBannedStatus(int id, bool banned);

};
