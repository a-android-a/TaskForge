#include "MainWindowWorker.h"
#include "TaskColumn.h"
#include "network/ApiClient.h"
#include "TaskCard.h"
#include "../Server/Task.h"
#include <QWidget>
#include <QList>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>
MainWindowWorker::MainWindowWorker(QWidget *parent) : QWidget(parent)
{

    QVBoxLayout *mainVertical = new QVBoxLayout(this);
    mainVertical->setContentsMargins(0, 0, 0, 0);
    mainVertical->setSpacing(0);


    QWidget *topBar = new QWidget(this);
    topBar->setFixedHeight(50);
    topBar->setStyleSheet("background: #2c3e50;");

    QHBoxLayout *topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(15, 0, 15, 0);
    topLayout->setSpacing(20);


    userLabel = new QLabel(tr("User"), topBar);
    userLabel->setStyleSheet("color: white; font-weight: bold; font-size: 14px;");
    topLayout->addWidget(userLabel);

    topLayout->addStretch();


    QLabel *connectionStatus = new QLabel(tr("Connection..."), topBar);
    connectionStatus->setStyleSheet("color: #f39c12; font-weight: bold; font-size: 13px;");
    topLayout->addWidget(connectionStatus);

    // Кнопка "Выйти"
    m_pLogoutButton = new QPushButton(tr("log out"), topBar);
    m_pLogoutButton->setStyleSheet(
        "background: #e74c3c; color: white; border: none; border-radius: 6px; "
        "padding: 8px 16px; font-weight: bold;"
        );
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

void MainWindowWorker::onLogoutClicked(){

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
        connect(m_apiClient, &ApiClient::tasksReceived,
                this, &MainWindowWorker::onTasksReceived,
                Qt::QueuedConnection);

        m_apiClient->getAllTasks();
    }

}

void MainWindowWorker::onTasksReceived(const QJsonArray &tasksArray){

    qInfo() << "Получено задач:" << tasksArray.size();
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

    for (const QJsonValue &val : tasksArray) {
        QJsonObject obj = val.toObject();

        Task t;
        t.id          = obj["id"].toInt();
        t.status      = obj["status"].toInt();
        t.priority    = obj["priority"].toInt();
        t.taskName    = obj["taskName"].toString();
        t.description = obj["description"].toString();
        t.due_date    = obj["due_date"].toString();
        t.created_by  = obj["created_by"].toString();
        t.assigned_to = obj["assigned_to"].toString();

        // qInfo() << "ID:" << obj["id"].toInt();
        // qInfo() << "Name:" << t.taskName;
        // qInfo() << "Desc:" << t.description;
        // qInfo() << "Due:" << t.due_date;
        // qInfo() << "Priority:" << t.priority;
        // qInfo() << "Status:" << t.status;


        TaskCard *card = new TaskCard(t, this);
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

