#pragma once
#include <QWidget>
#include "network/ApiClient.h"
#include "../Server/Task.h"
#include "TaskColumn.h"
#include <QJsonArray>
class QTextEdit;
class QLineEdit;
class QPushButton;
class QVBoxLayout;
class QHBoxLayout;

class MainWindowWorker: public QWidget
{
    Q_OBJECT;
private:
    ApiClient*   m_apiClient = nullptr;
    QVBoxLayout* m_pVBoxLayout;
    TaskColumn*  todoColumn;
    TaskColumn*  inProgress;
    TaskColumn*  doneColumn;
    QPushButton* m_pLogoutButton;
private slots:
    void onTaskDropped(TaskCard* card, TaskColumn* targetColumn);
    void onTasksReceived(const QJsonArray &tasksArray);
public:
    explicit MainWindowWorker(QWidget *parent = nullptr);
    void setApiClient(ApiClient* apiClient);
    void setTasks(QVector<Task> tasks);
};
