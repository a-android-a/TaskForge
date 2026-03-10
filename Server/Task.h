#pragma once
#include<QString>
struct Task
{
    qint64  id;
    qint64  status;
    qint64  priority;
    QString taskName;
    QString description;
    QString due_date;
    QString created_by;
    QString assigned_to;
};
