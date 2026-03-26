#pragma once
#include<QList>
#include "BaseDatabaseManager.h"
#include "User.h"

class UsersDatabaseManager : public BaseDatabaseManager
{
public:
    explicit UsersDatabaseManager(const QString& path = "DB/Users.db"): BaseDatabaseManager(path, "users_conn") {}

    User getUserById(int id);
    QList<User>  getAllUsers();
    User getUserByLoginHash(const QString& loginHash);

    bool createUser(
        const QString& name,
        const QString& surname,
        const QString& jobTitle,
        const QString& login,
        const QString& password);

    bool updateUserBasicInfo(int id,
                             const QString& name,
                             const QString& surname,
                             const QString& jobTitle);

    bool setBannedStatus(int id, bool banned);

};
