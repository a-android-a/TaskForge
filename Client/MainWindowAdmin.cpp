#include "MainWindowAdmin.h"
#include "WindowCreateUser.h"
#include "UserManagementWindow.h"
#include "MainWindowManager.h"
#include "MainWindowWorker.h"
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

    layoutUsers = new QVBoxLayout(this);

    m_btnCreateUser = new QPushButton(tr("Create a user"), this);
    m_btnBanUserUnbanUser  = new QPushButton(tr("Unbanking a user/Ban a user"), this);


    m_btnCreateUser->setObjectName("btnCreateUser");

    m_btnBanUserUnbanUser->setObjectName("btnUnbanUser");



    layoutUsers->addWidget(m_btnCreateUser);
    layoutUsers->addWidget(m_btnBanUserUnbanUser);



    m_mainLayout->addLayout(layoutUsers);

    // Заглушки

    connect(m_btnCreateUser,       &QPushButton::clicked, this, &MainWindowAdmin::onBtnCreateUser  );
    connect(m_btnBanUserUnbanUser, &QPushButton::clicked, this, &MainWindowAdmin::onBtnBanUnbanUser);



    connect(m_btnBanUserUnbanUser, &QPushButton::clicked, [](){
        qInfo() << "Unban user clicked";
    });


}
void MainWindowAdmin::onBtnBanUnbanUser(){
    UserManagementWindow* w = new UserManagementWindow(this);
    w->show();

    m_apiClient->getAllUsers();

    connect(m_apiClient, &ApiClient::usersListReceived, w, &UserManagementWindow::setUsers);
    connect(w, &UserManagementWindow::banUserRequested,this,      &MainWindowAdmin::onBanClicked);
    connect(w, &UserManagementWindow::unbanUserRequested,this,    &MainWindowAdmin::onUnbanClicked);
    connect(w, &UserManagementWindow::refreshRequested,this,      &MainWindowAdmin::onRefreshClicked);




}
void MainWindowAdmin::onBanClicked(int userId){
    qInfo()<<"onBanClicked "<<userId;
    m_apiClient->banUser(userId);
}

void MainWindowAdmin::onUnbanClicked(int userId){
     qInfo()<<"onUnbanClicked "<<userId;
    m_apiClient->unBanUser(userId);
}
void MainWindowAdmin::onRefreshClicked(){

}
void MainWindowAdmin::onBtnCreateTask(){
    MainWindowManager* w = new MainWindowManager(nullptr);
    w->setStyle("style/stylesMainWindowWorkerLight.qss");
    w->setApiClient(m_apiClient);
    w->show();
}
void MainWindowAdmin::onBtnTaskList(){
    MainWindowWorker* w = new MainWindowWorker(nullptr);
    w->setStyle("style/stylesMainWindowWorkerLight.qss");
    w->setApiClient(m_apiClient);
    w->show();
}
void MainWindowAdmin::setupTaskControls()
{

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


    m_mainLayout->addLayout(layoutTasks);


    connect(m_btnCreateTask,   &QPushButton::clicked, this, &MainWindowAdmin::onBtnCreateTask );

    connect(m_btnDeleteTask, &QPushButton::clicked, [](){
        qInfo() << "Delete task clicked";
    });

    connect(m_btnEditTask, &QPushButton::clicked, [](){
        qInfo() << "Edit task clicked";
    });

    connect(m_btnTaskList,   &QPushButton::clicked, this, &MainWindowAdmin::onBtnTaskList );
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
