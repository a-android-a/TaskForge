#pragma once

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include "../Server/Task.h"
#include "../Server/User.h"
#include "../Server/Task.h"
#include "TaskEditWindow.h"
#include "network/ApiClient.h"
class MainWindowAdmin : public QWidget
{
    Q_OBJECT

private:
    User m_admin;  // текущий админ

    // UI элементы
    QWidget* m_centralWidget;
    QVBoxLayout* m_mainLayout;

    ApiClient*   m_apiClient;
    // Блоки
    TaskEditWindow* w;
    // Кнопки пользователей
    QVBoxLayout* layoutUsers;
    QPushButton* m_btnCreateUser;
    QPushButton* m_btnBanUserUnbanUser;


    // Кнопки задач
    QVBoxLayout* layoutTasks;
    QPushButton* m_btnCreateTask;
    QPushButton* m_btnDeleteTask;
    QPushButton* m_btnEditTask;
    QPushButton* m_btnTaskList;

    QPushButton* m_pLogoutButton;
    QVector<Task> tasks;
    QString des;
private slots:
    void onBtnCreateUser();
    void onBtnBanUnbanUser();
    // void onBtnUnbanUser();
    // void onBtnUserList();
    void onTasksReceived(const QJsonArray &tasksArray);
    void onBanClicked  (int userId);
    void onUnbanClicked(int userId);
    void onRefreshClicked();
    void onBtnCreateTask();
    // void onBtnDeleteTask();
    void onBtnEditTask();
    void onBtnTaskList();
    void onUserCreated(const User& user);
    void onLogoutClicked();
    void onGetDescrition(const qint64 id);
    void onDescriptionListReceived(const QString& str);
    void onSaveTask(const qint64 id, const QString& json);
    void onButtonDeletedTask();
    void onDeleteTask(const qint64 id);
signals:
    void ButtonLogOut();
public:

    explicit MainWindowAdmin( QWidget* parent = nullptr);
    void setupUI();
    void setupUserControls();
    void setupTaskControls();
    bool setStyle(const QString &styleFileName);
    void setApiClient(ApiClient* apiClient);
};

