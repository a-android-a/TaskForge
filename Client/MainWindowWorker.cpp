#include "MainWindowWorker.h"
#include "TaskColumn.h"
#include "network/ApiClient.h"
#include "TaskCard.h"
#include "../Server/Task.h"
#include "TaskInfoWindow.h"
#include <QWidget>
#include <QList>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>
#include <QFile>


MainWindowWorker::MainWindowWorker(QWidget *parent) : QWidget(parent)
{

    QVBoxLayout *mainVertical = new QVBoxLayout(this);
    mainVertical->setContentsMargins(0, 0, 0, 0);
    mainVertical->setSpacing(0);
    this->resize(QSize(800,800));

    QWidget *topBar = new QWidget(this);
    topBar->setFixedHeight(50);
    topBar->setObjectName("topBar");

    QHBoxLayout *topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(15, 0, 15, 0);
    topLayout->setSpacing(20);

    // User Name
    userLabel = new QLabel(tr("User"), topBar);
    userLabel->setObjectName("userLabel");
    topLayout->addWidget(userLabel);

    topLayout->addStretch();


    connectionStatus = new QLabel(tr("Connection..."), topBar);
    //connectionStatus->setObjectName("connectionStatus");
    connectionStatus->setStyleSheet(" color: #f39c12;  font-weight: bold; font-size: 13px;");
    topLayout->addWidget(connectionStatus);


    m_pLogoutButton = new QPushButton(tr("log out"), topBar);
    m_pLogoutButton->setObjectName("logoutButton");
    m_pLogoutButton->setCursor(Qt::PointingHandCursor);
    connect(m_pLogoutButton, &QPushButton::clicked, this, &MainWindowWorker::onLogoutClicked);
    topLayout->addWidget(m_pLogoutButton);

    mainVertical->addWidget(topBar);


    QHBoxLayout *kanbanLayout = new QHBoxLayout();
    kanbanLayout->setSpacing(10);
    kanbanLayout->setContentsMargins(10, 10, 10, 10);

    todoColumn   = new TaskColumn(tr("Not started"), this);
    inProgress   = new TaskColumn(tr("At work"), this);
    doneColumn   = new TaskColumn(tr("Completed"), this);

    kanbanLayout->addWidget(todoColumn);
    kanbanLayout->addWidget(inProgress);
    kanbanLayout->addWidget(doneColumn);

    mainVertical->addLayout(kanbanLayout);

    connect(todoColumn,   &TaskColumn::taskDropped, this, &MainWindowWorker::onTaskDropped);
    connect(inProgress,   &TaskColumn::taskDropped, this, &MainWindowWorker::onTaskDropped);
    connect(doneColumn,   &TaskColumn::taskDropped, this, &MainWindowWorker::onTaskDropped);





}
void MainWindowWorker::onTaskUpdated(const Task &task, const QString &descriptionJson){
    m_apiClient->updateTaskDescription(task.id, descriptionJson);

    // Обновляем локальные данные

}

void MainWindowWorker::onLogoutClicked(){

}
void MainWindowWorker::onCardClicked(int taskId)
{
    m_currentTaskId= taskId;
    Task taskShow;
    for(const Task &t : tasks){
        if(t.id == m_currentTaskId){
            taskShow = t;
            break;
        }
    }
    qInfo()<<des;
    m_infoWindow = new TaskInfoWindow(nullptr,taskShow);
    m_infoWindow->resize(600, 400);
    m_infoWindow->setWindowTitle(tr("Task info"));
    m_infoWindow->setStyle("style/stylesMainWindowWorkerLight.qss");
    m_infoWindow->show();
    m_infoWindow->raise();
    m_infoWindow->activateWindow();
    m_apiClient->getDescription(taskShow.id);
    connect(m_infoWindow, &TaskInfoWindow::taskUpdated, this, &MainWindowWorker::onTaskUpdated);
}

void MainWindowWorker::setApiClient(ApiClient* apiClient){
    if (m_apiClient == apiClient) {
        return;
    }


    if (m_apiClient) {
        disconnect(m_apiClient, nullptr, this, nullptr);

    }

    m_apiClient = apiClient;

    if (m_apiClient) {




        //connect(m_apiClient, &ApiClient::tasksReceived,this,        &MainWindowWorker::onTasksReceived);
        connect(m_apiClient, &ApiClient::tasksReceived          ,this, &MainWindowWorker::onTasksReceived,Qt::QueuedConnection);
        connect(m_apiClient, &ApiClient::descriptionListReceived,this, &MainWindowWorker::onDescriptionList);

        m_apiClient->getAllTasks();
        connectionStatus->setText(tr("Connected"));
        connectionStatus->setStyleSheet(" color: #3CF527;  font-weight: bold; font-size: 13px;");
    }

}
void MainWindowWorker::onDescriptionList(const QString& des){
    qInfo()<<des;
    m_infoWindow->setDescription(des);
}
void MainWindowWorker::onTasksReceived(const QJsonArray &tasksArray){

    qInfo() << ":" << tasksArray.size();
    if (!todoColumn) {
        qCritical() << "todoColumn == nullptr!";
        return;
    }
    if (!inProgress) {
        qCritical() << "inProgress == nullptr!";
        return;
    }
    if (!doneColumn) {
        qCritical() << "doneColumn == nullptr!";
        return;
    }

    todoColumn->clearCards();
    inProgress->clearCards();
    doneColumn->clearCards();
    tasks.clear();
    for (const QJsonValue &val : tasksArray) {
        QJsonObject obj = val.toObject();

        Task t;
        t.id          = obj["id"].toInt();
        t.status      = obj["status"].toInt();
        t.priority    = obj["priority"].toInt();
        t.taskName    = obj["taskName"].toString();
        //t.description = obj["description"].toString();
        t.due_date    = obj["due_date"].toString();
        t.created_by  = obj["created_by"].toString();
        t.assigned_to = obj["assigned_to"].toString();

        // qInfo() << "ID:" << obj["id"].toInt();
        // qInfo() << "Name:" << t.taskName;
        // qInfo() << "Desc:" << t.description;
        // qInfo() << "Due:" << t.due_date;
        // qInfo() << "Priority:" << t.priority;
        // qInfo() << "Status:" << t.status;
        tasks.append(t);

        TaskCard *card = new TaskCard(t, this);
        connect(card,         &TaskCard::cardClicked,   this, &MainWindowWorker::onCardClicked);
        if (t.status == 0) {

            todoColumn->addTaskCard(card);
        }
        else if (t.status == 1) {

            inProgress->addTaskCard(card);
        }
        else if (t.status == 2) {

            doneColumn->addTaskCard(card);
        }
    }


}
bool MainWindowWorker::setStyle(const QString &styleFileName){

    QFile file(styleFileName);
    if(file.open(QFile::ReadOnly)){
        QString strCSS = QLatin1String(file.readAll());
        this->setStyleSheet(strCSS);
        file.close();
        return true;
    }
    return false;
}

void MainWindowWorker::onTaskDropped(TaskCard* card, TaskColumn* targetColumn)
{

    if (auto oldColumn = qobject_cast<TaskColumn*>(card->parentWidget()))
        oldColumn->removeTaskCard(card);

    card->setParent(targetColumn);
    targetColumn->addTaskCard(card);

    card->show();
    int newStatus = targetColumn->status();
    m_apiClient->updateTaskStatus(card->taskId(), newStatus);
}

void MainWindowWorker::setUser(const User& user){
    this->user = user;
    if(!user.name.isEmpty() && !user.name.isEmpty()){
        userLabel->setText(user.name + ": " + user.jobTitle);
    }

}
