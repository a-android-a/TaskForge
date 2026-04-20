#pragma once
#include <QWidget>
#include "network/ApiClient.h"
#include "../Server/Task.h"
#include "../Server/User.h"
#include "TaskInfoWindow.h"
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
    TaskInfoWindow* m_infoWindow;
    ApiClient*   m_apiClient = nullptr;
    QVBoxLayout* mainVertical;
    QLabel*      userLabel;
    QHBoxLayout* topLayout;
    QWidget*     topBar;
    TaskColumn*  todoColumn;
    TaskColumn*  inProgress;
    TaskColumn*  doneColumn;
    QPushButton* m_pLogoutButton;
    QLabel*      connectionStatus;
    QVector<Task>tasks;
    User user;
    QString des;
    qint64 m_currentTaskId;
private slots:
    void onTaskDropped(TaskCard* card, TaskColumn* targetColumn);
    void onTasksReceived(const QJsonArray &tasksArray);
    void onLogoutClicked();
    void onCardClicked(const int taskId);
    void onDescriptionList(const QString& des);
public:
    explicit MainWindowWorker(QWidget *parent = nullptr);
    void setApiClient(ApiClient* apiClient);
    void setTasks(QVector<Task> tasks);
    bool setStyle(const QString& fileStyle);
    void setUser(const User& user);
};
