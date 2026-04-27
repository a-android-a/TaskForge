#pragma once
#include <QWidget>
#include <QVector>
#include"../Server/Task.h"
class QVBoxLayout;
class QScrollArea;
class QPushButton;



class TaskDeletedWindow : public QWidget {
    Q_OBJECT

private:
    QVBoxLayout *mainLayout;
    QScrollArea *tasksArea;
    QVBoxLayout *tasksListLayout;
    QPushButton *deleteBtn;

    qint64 selectedTaskId = -1;

public:
    explicit TaskDeletedWindow(QWidget *parent = nullptr);

    void setTasks(const QVector<Task> &tasks);
    bool setStyle(const QString &styleFileName);

signals:
    void deleteTask(qint64 id);

};
