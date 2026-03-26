#pragma once

#include <QSslSocket>
#include <QString>
#include "../Config.h"
#include "../../Server/User.h"
#include "../../Server/Task.h"
#include <QList>
#include <QJsonArray>
class ApiClient:public QSslSocket{
    Q_OBJECT;
private:
    QString m_hostName;
    int m_nPort;
    User user;

public:
    ApiClient(QObject* parent);
    void setHostName  (const QString &hostName);
    void setHostPort  (const uint16_t  port);
    void sendToServer (const QString &data);
    void connectToServer();
    void authenticate(const QString &login, const QString password);
    void getAllTasks();
    void updateTaskStatus(const int taskID, const int newTask);
    void createTask(const Task& task);
    void createUser(const User& user);
    void getAllUsers();
signals:
    void authorizationFailed();
    void authorizationOk();
    void tasksReceived(const QJsonArray &TasksArry);
    void createWorkerWindow(const User& user);
    void usersListReceived(const QList<User>& user);
private slots:
    void slotReadyRead();

};
