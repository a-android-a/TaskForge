#include "MainWindowAdmin.h"
#include "WindowCreateUser.h"
#include "../Server/User.h"
#include "../Server/Task.h"

#include <QList>
#include <QString>
#include <QFile>
#include <QLabel>
MainWindowAdmin::MainWindowAdmin(QWidget* parent)
{
    setupUI();
    this->setObjectName("adminWindow");


}

// ---------------------------------------------------------
// UI SETUP
// ---------------------------------------------------------

void MainWindowAdmin::setupUI()
{
    m_centralWidget = new QWidget(this);
    m_mainLayout = new QVBoxLayout(this);

    QWidget *topBar = new QWidget(this);
    topBar->setFixedHeight(50);
    topBar->setObjectName("topBar");

    QHBoxLayout *topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(15, 0, 15, 0);
    topLayout->setSpacing(20);

    QLabel* userLabel = new QLabel(tr("Admin"), topBar);
    userLabel->setObjectName("userLabel");
    topLayout->addWidget(userLabel);

    topLayout->addStretch(); // ← перемещено сюда

    m_pLogoutButton = new QPushButton(tr("log out"), topBar);
    m_pLogoutButton->setObjectName("logoutButton");
    m_pLogoutButton->setCursor(Qt::PointingHandCursor);
    topLayout->addWidget(m_pLogoutButton);


    m_mainLayout->addWidget(topBar);

    // Заголовок
    QLabel* title = new QLabel(tr("Admin Panel"), this);
    title->setAlignment(Qt::AlignCenter);
    m_mainLayout->addWidget(title);

    setupUserControls();
    setupTaskControls();

    m_mainLayout->addStretch();
}


void MainWindowAdmin::setupUserControls()
{
    // Создаём layout для блока пользователей
    layoutUsers = new QVBoxLayout(this);

    m_btnCreateUser = new QPushButton(tr("Create a user"), this);
    m_btnBanUser    = new QPushButton(tr("Ban a user"), this);
    m_btnUnbanUser  = new QPushButton(tr("Unbanking a user"), this);
    m_btnUserList   = new QPushButton(tr("List of users"), this);

    m_btnCreateUser->setObjectName("btnCreateUser");
    m_btnBanUser->setObjectName("btnBanUser");
    m_btnUnbanUser->setObjectName("btnUnbanUser");
    m_btnUserList->setObjectName("btnUserList");


    layoutUsers->addWidget(m_btnCreateUser);
    layoutUsers->addWidget(m_btnBanUser);
    layoutUsers->addWidget(m_btnUnbanUser);
    layoutUsers->addWidget(m_btnUserList);

    // Добавляем в главный layout
    m_mainLayout->addLayout(layoutUsers);

    // Заглушки

    connect(m_btnCreateUser, &QPushButton::clicked, this, &MainWindowAdmin::onBtnCreateUser);

    connect(m_btnBanUser, &QPushButton::clicked, [](){
        qInfo() << "Ban user clicked";
    });

    connect(m_btnUnbanUser, &QPushButton::clicked, [](){
        qInfo() << "Unban user clicked";
    });

    connect(m_btnUserList, &QPushButton::clicked, [](){
        qInfo() << "User list clicked";
    });
}

void MainWindowAdmin::setupTaskControls()
{
    // Layout для блока задач
    layoutTasks = new QVBoxLayout(this);

    m_btnCreateTask = new QPushButton(tr("Create a task"), this);
    m_btnDeleteTask = new QPushButton(tr("Delete an issue"), this);
    m_btnEditTask   = new QPushButton(tr("Edit a task"), this);
    m_btnTaskList   = new QPushButton(tr("Task list"), this);

    m_btnCreateTask->setObjectName("btnCreateTask");
    m_btnDeleteTask->setObjectName("btnDeleteTask");
    m_btnEditTask->setObjectName("btnEditTask");
    m_btnTaskList->setObjectName("btnTaskList");


    layoutTasks->addWidget(m_btnCreateTask);
    layoutTasks->addWidget(m_btnDeleteTask);
    layoutTasks->addWidget(m_btnEditTask);
    layoutTasks->addWidget(m_btnTaskList);

    // Добавляем в главный layout
    m_mainLayout->addLayout(layoutTasks);

    // Заглушки
    connect(m_btnCreateTask, &QPushButton::clicked, [](){
        qInfo() << "Create task clicked";
    });

    connect(m_btnDeleteTask, &QPushButton::clicked, [](){
        qInfo() << "Delete task clicked";
    });

    connect(m_btnEditTask, &QPushButton::clicked, [](){
        qInfo() << "Edit task clicked";
    });

    connect(m_btnTaskList, &QPushButton::clicked, [](){
        qInfo() << "Task list clicked";
    });
}

bool MainWindowAdmin::setStyle(const QString &styleFileName){

    QFile file(styleFileName);
    if(file.open(QFile::ReadOnly)){
        QString strCSS = QLatin1String(file.readAll());
        this->setStyleSheet(strCSS);
        file.close();
        return true;
    }
    return false;
}
void MainWindowAdmin::onBtnCreateUser(){
    WindowCreateUser* w = new WindowCreateUser(this);
    w->show();
    connect(w,&WindowCreateUser::userCreated, this, &MainWindowAdmin::onUserCreated);
}

void MainWindowAdmin::setApiClient(ApiClient* apiClient)
{
    m_apiClient = apiClient;
}
void MainWindowAdmin::onUserCreated(const User& user){
    qInfo() << "onUserCreated";
    m_apiClient->createUser(user);
}
