#include "MainWindowManager.h"
#include "network/ApiClient.h"
#include "../Server/Task.h"
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QFile>
#include <QCheckBox>
#include <QMessageBox>
MainWindowManager::MainWindowManager(QWidget *parent) : QWidget(parent){

    m_pVbox        = new QVBoxLayout(this);
    m_AddTask      = new QPushButton(tr("SaveTask"),this);
    m_AddChekBox   = new QPushButton(tr("addTask"),this);
    m_pTaskName    = new QLineEdit(this);
    m_pDueDate     = new QLineEdit(this);
    m_pCreatedBy   = new QLineEdit(this);
    m_pAssignedTo  = new QLineEdit(this);
    m_pDescription = new QLineEdit(this);
    m_pPriority    = new QLineEdit(this);

    m_pTaskName->setObjectName("taskInput");
    m_pDueDate->setObjectName("taskInput");
    m_pCreatedBy->setObjectName("taskInput");
    m_pAssignedTo->setObjectName("taskInput");
    m_pDescription->setObjectName("taskDescription");
    m_pPriority->setObjectName("taskPriority");
    m_AddTask->setObjectName("addTaskButton");
    m_AddChekBox->setObjectName("addCheckboxButton");

    m_pTaskName->setPlaceholderText("Task name");
    m_pDueDate->setPlaceholderText("Due date (YYYY-MM-DD)");
    m_pCreatedBy->setPlaceholderText("Created by");
    m_pAssignedTo->setPlaceholderText("Assigned to");
    m_pDescription->setPlaceholderText("Description");
    m_pPriority->setPlaceholderText("Task priority");

    QWidget* card = new QWidget(this);
    card->setObjectName("taskFormCard");

    QVBoxLayout* cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(20, 20, 20, 20);
    cardLayout->setSpacing(12);
    cardLayout->addWidget(m_pTaskName);
    cardLayout->addWidget(m_pDueDate);
    cardLayout->addWidget(m_pCreatedBy);
    cardLayout->addWidget(m_pAssignedTo);
    cardLayout->addWidget(m_pDescription);
    cardLayout->addWidget(m_pPriority);
    cardLayout->addWidget(m_AddTask);
    cardLayout->addWidget(m_AddChekBox);


    m_pVbox->addWidget(card);
    connect(m_AddChekBox, &QPushButton::clicked, this, &MainWindowManager::onButtonAddChekBox);
    connect(m_AddTask,    &QPushButton::clicked, this, &MainWindowManager::onButtonAddTask   );
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setObjectName("taskScrollArea");

    // Internal widget for scrolling
    m_scrollWidget = new QWidget(this);
    m_scrollWidget->setObjectName("taskScrollWidget");

    // Layout for checkboxes
    m_checkBoxLayout = new QVBoxLayout(m_scrollWidget);
    m_checkBoxLayout->setContentsMargins(0, 0, 0, 0);
    m_checkBoxLayout->setSpacing(6);

    // Attach widget to scroll area
    m_scrollArea->setWidget(m_scrollWidget);

    // Add scroll area to the card
    cardLayout->addWidget(m_scrollArea);
}

void MainWindowManager::onButtonAddChekBox(){
    QLineEdit* box = new QLineEdit(tr("New subtask"), this);
    box->setObjectName("taskCheckbox");
    m_checkBoxLayout->addWidget(box);
}

void MainWindowManager::onButtonAddTask()
{
    if(!m_pTaskName->text().isEmpty() && !m_pDueDate->text().isEmpty() && !m_pCreatedBy->text().isEmpty() && !m_pAssignedTo->text().isEmpty() && !m_pPriority->text().isEmpty()){
        Task task;

        task.id = 0;
        task.status = 0;
        task.priority = task.priority = m_pPriority->text().toInt();

        task.taskName    = m_pTaskName->text();

        task.due_date    = m_pDueDate->text();
        task.created_by  = m_pCreatedBy->text();
        task.assigned_to = m_pAssignedTo->text();

        QJsonArray tasksArray;

        for (int i = 0; i < m_checkBoxLayout->count(); ++i) {
            QWidget* w = m_checkBoxLayout->itemAt(i)->widget();

            if (auto line = qobject_cast<QLineEdit*>(w)) {
                QString text = line->text().trimmed();

                if (!text.isEmpty()) {
                    QJsonObject subtaskObj;
                    subtaskObj["text"] = text;
                    qInfo()<<text;
                    subtaskObj["done"] = false;

                    tasksArray.append(subtaskObj);
                }
            }
        }
        QJsonObject descObj;
        descObj["main"] = m_pDescription->text();
        descObj["tasks"] = tasksArray;
        QJsonDocument descDoc(descObj);
        QString descriptionJsonString = descDoc.toJson(QJsonDocument::Compact);
        task.description = descriptionJsonString;
        qInfo()<<task.description;

        if (m_apiClient)
            m_apiClient->createTask(task);
    }else {
        QMessageBox::warning(this, tr("Error"), tr("all fields must be filled in"));

    }
}

bool MainWindowManager::setStyle(const QString &styleFileName){

    QFile file(styleFileName);
    if(file.open(QFile::ReadOnly)){
        QString strCSS = QLatin1String(file.readAll());
        this->setStyleSheet(strCSS);
        file.close();
        return true;
    }
    return false;
}

void MainWindowManager::setApiClient(ApiClient* apiClient)
{
    m_apiClient = apiClient;
}
