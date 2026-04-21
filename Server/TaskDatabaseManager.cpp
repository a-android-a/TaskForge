#include "TaskDatabaseManager.h"
#include "Task.h"
#include <QDebug>

Task TaskDatabaseManager::getTaskById(qint64 id)
{
    auto row = getFirstRow(
        "SELECT id, status, priority, taskName, description, due_date, created_by, assigned_to "
        "FROM \"Task\" WHERE id = :id",
        {{":id", id}}
        );

    if (row.isEmpty()) {
        qWarning() << "Task with id" << id << "not found";
        return Task{};
    }

    Task t;
    t.id          = row.value("id").toLongLong();
    t.status      = row.value("status").toLongLong();
    t.priority    = row.value("priority").toLongLong();
    t.taskName    = row.value("taskName").toString();
    //t.description = row.value("description").toString();
    t.due_date    = row.value("due_date").toString();
    t.created_by  = row.value("created_by").toString();
    t.assigned_to = row.value("assigned_to").toString();

    return t;
}

QVector<Task> TaskDatabaseManager::getAllTasks()
{
    QVector<Task> tasks;

    auto rows = getRows(
        "SELECT id, status, priority, taskName, description, due_date, created_by, assigned_to "
        "FROM \"Task\" ORDER BY priority ASC, id DESC"
        );

    for (const auto& row : rows) {
        Task t;
        t.id          = row.value("id").toLongLong();
        t.status      = row.value("status").toLongLong();
        t.priority    = row.value("priority").toLongLong();
        t.taskName    = row.value("taskName").toString();
        //t.description = row.value("description").toString();
        t.due_date    = row.value("due_date").toString();
        t.created_by  = row.value("created_by").toString();
        t.assigned_to = row.value("assigned_to").toString();

        tasks.append(t);
    }

    return tasks;
}

bool TaskDatabaseManager::createTask(const Task& task,const QString des)
{
    qint64 newId = -1;

    bool ok = execInsertGetId(
        "INSERT INTO \"Task\" (status, priority, taskName, description, due_date, created_by, assigned_to) "
        "VALUES (:status, :priority, :taskName, :description, :due_date, :created_by, :assigned_to)",
        {
            {":status",      task.status},
            {":priority",    task.priority},
            {":taskName",    task.taskName},
            {":description", des},
            {":due_date",    task.due_date},
            {":created_by",  task.created_by},
            {":assigned_to", task.assigned_to}
        },
        newId
        );

    if (ok) {
        qInfo() << "Task created with ID:" << newId;
    } else {
        qWarning() << "Failed to create task";
    }

    return ok && newId > 0;
}

bool TaskDatabaseManager::updateTask(qint64 id, const Task& task)
{
    bool ok = exec(
        "UPDATE \"Task\" SET "
        "status = :status, "
        "priority = :priority, "
        "taskName = :taskName, "
        "description = :description, "
        "due_date = :due_date, "
        "created_by = :created_by, "
        "assigned_to = :assigned_to "
        "WHERE id = :id",
        {
            {":id",          id},
            {":status",      task.status},
            {":priority",    task.priority},
            {":taskName",    task.taskName},
            //{":description", task.description},
            {":due_date",    task.due_date},
            {":created_by",  task.created_by},
            {":assigned_to", task.assigned_to}
        }
        );

    if (ok) {
        qInfo() << "Task" << id << "updated";
    } else {
        qWarning() << "Failed to update task" << id;
    }

    return ok;
}
bool TaskDatabaseManager::updateStatus(const long long id, const long long newStatus)
{
    QSqlQuery query(m_db);

    query.prepare("UPDATE Task SET status = :status WHERE id = :id");
    query.bindValue(":status", newStatus);
    query.bindValue(":id", id);

    if (!query.exec()) {
        qInfo() << "Ошибка обновления статуса:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}
bool TaskDatabaseManager::updateTaskDescription(qint64 id, const QString &des)
{
    QSqlQuery query(m_db);

    query.prepare("UPDATE Task SET description = :description WHERE id = :id");
    query.bindValue(":description", des);
    query.bindValue(":id", id);

    if (!query.exec()) {
        qWarning() << "Failed to update task description:" << query.lastError().text();
        return false;
    }

    return true;
}



QString TaskDatabaseManager::getDescription(const qint64 id){
    QSqlQuery query(m_db);
    query.prepare("SELECT description FROM Task WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qWarning() << "SQL error:" << query.lastError().text();
        return QString(); // пустая строка
    }

    if (query.next()) {
        return query.value(0).toString();
    }

    return QString(); // если нет записи
}
