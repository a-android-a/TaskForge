#pragma once
#include "BaseDatabaseManager.h"
#include "Task.h"
#include <QVector>

class TaskDatabaseManager: public BaseDatabaseManager{
public:
    explicit TaskDatabaseManager(const QString& path = "DB/tasks.db"): BaseDatabaseManager(path, "task_conn") {}

    Task getTaskById(qint64 id);
    QVector<Task> getAllTasks();
    bool createTask(const Task& task, const QString des);
    bool updateTask(const qint64 id, const Task &task);
    bool updateStatus(const long long id , const long long newStatus);
    bool updateTaskDescription(const qint64 id, const QString& des);
    QString getDescription(const qint64 id);
    bool saveTask(const qint64 id, const QString& des);
    bool deleteTask(const qint64 id);

};
