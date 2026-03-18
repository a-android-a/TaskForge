#pragma once
#include "BaseDatabaseManager.h"
#include "Task.h"
#include <QVector>

class TaskDatabaseManager: public BaseDatabaseManager{
public:
    explicit TaskDatabaseManager(const QString& path = "DB/tasks.db"): BaseDatabaseManager(path, "task_conn") {}

    Task getTaskById(qint64 id);
    QVector<Task> getAllTasks();
    bool createTask(const Task& task);
    bool updateTask(const qint64 id, const Task &task);

};
