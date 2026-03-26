#pragma once

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>

#include "../Server/User.h"        // структура User
#include "../Server/Task.h"        // структура Task
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
private slots:
    void onBtnCreateUser();
    void onBtnBanUnbanUser();
    // void onBtnUnbanUser();
    // void onBtnUserList();

    // void onBtnCreateTask();
    // void onBtnDeleteTask();
    // void onBtnEditTask();
    // void onBtnTaskList();
    void onUserCreated(const User& user);
public:

    explicit MainWindowAdmin( QWidget* parent = nullptr);
    void setupUI();
    void setupUserControls();
    void setupTaskControls();
    bool setStyle(const QString &styleFileName);
    void setApiClient(ApiClient* apiClient);
};

