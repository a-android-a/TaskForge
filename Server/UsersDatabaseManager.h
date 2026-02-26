#pragma once

#include "BaseDatabaseManager.h"
#include "User.h"

class UsersDatabaseManager : public BaseDatabaseManager
{
public:
    explicit UsersDatabaseManager(const QString& path = "DB/Users.db"): BaseDatabaseManager(path, "users_conn") {}

    std::optional<User> getUserById(int id);

    std::optional<User> getUserByLoginHash(const QString& loginHash);

    bool createUser(
        const QString& name,
        const QString& surname,
        const QString& jobTitle,
        const QString& loginHash,
        const QString& passwordHash
        );

    bool updateUserBasicInfo(int id,
                             const QString& name,
                             const QString& surname,
                             const QString& jobTitle);

    bool setBannedStatus(int id, bool banned);

};
