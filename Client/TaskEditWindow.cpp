#include "TaskEditWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <QCheckBox>
#include <QProgressBar>
#include <QPushButton>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QScrollArea>
#include <QFile>
#include <QLineEdit>
#include <QInputDialog>

TaskEditWindow::TaskEditWindow(QWidget *parent, const Task &task)
    : QWidget(parent), m_task(task)
{
    setObjectName("TaskEditWindow");
    setWindowTitle("Edit Task");
    resize(520, 620);

    mainLayout = new QVBoxLayout(this);
   // mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(10);


    mainLayout->addWidget(new QLabel("Task name:", this));
    m_pTaskName = new QLineEdit(this);
    mainLayout->addWidget(m_pTaskName);

    mainLayout->addWidget(new QLabel("Description:", this));
    descriptionEdit = new QTextEdit(this);
    descriptionEdit->setMinimumHeight(100);
    mainLayout->addWidget(descriptionEdit);

    mainLayout->addWidget(new QLabel("Due date:", this));
    m_pDueDate = new QLineEdit(this);
    mainLayout->addWidget(m_pDueDate);

    mainLayout->addWidget(new QLabel("Priority:", this));
    m_priority = new QLineEdit(this);
    mainLayout->addWidget(m_priority);

    mainLayout->addWidget(new QLabel("Status:", this));

    mainLayout->addWidget(new QLabel("Tasks:", this));

    tasksArea = new QScrollArea(this);
    tasksArea->setWidgetResizable(true);
    mainLayout->addWidget(tasksArea);

    QWidget *tasksWidget = new QWidget(this);
    tasksListLayout = new QVBoxLayout(tasksWidget);
    tasksWidget->setLayout(tasksListLayout);
    tasksArea->setWidget(tasksWidget);


    mainLayout->addWidget(new QLabel("Subtasks:", this));

    subtasksArea = new QScrollArea(this);
    subtasksArea->setWidgetResizable(true);
    mainLayout->addWidget(subtasksArea);
    subtasksArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);


    QWidget *subtasksWidget = new QWidget(this);
    subtasksLayout = new QVBoxLayout(subtasksWidget);
    subtasksWidget->setLayout(subtasksLayout);
    subtasksArea->setWidget(subtasksWidget);




    QHBoxLayout *btnLayout = new QHBoxLayout();
    saveBtn = new QPushButton("Save", this);
    QPushButton *closeBtn = new QPushButton("Close", this);

    btnLayout->addWidget(saveBtn);
    btnLayout->addWidget(closeBtn);
    mainLayout->addLayout(btnLayout);
    connect(saveBtn,       &QPushButton::clicked, this, &TaskEditWindow::onButtonSave  );
}

bool TaskEditWindow::setStyle(const QString &styleFileName)
{
    QFile file(styleFileName);
    if (file.open(QFile::ReadOnly)) {
        QString strCSS = QLatin1String(file.readAll());
        this->setStyleSheet(strCSS);
        file.close();
        return true;
    }
    return false;
}

void TaskEditWindow::setTasks(const QVector<Task> &tasks)
{
    this->tasks = tasks;


    QLayoutItem *child;
    while ((child = tasksListLayout->takeAt(0)) != nullptr) {
        if (child->widget()) child->widget()->deleteLater();
        delete child;
    }


    for (const Task &t : tasks) {
        QPushButton *btn = new QPushButton(t.taskName, this);
        btn->setStyleSheet("QPushButton {"
                           "   text-align: left;"
                           "   padding: 10px 12px;"
                           "   font-size: 17px;"
                           "   background: #f7f9fc;"
                           "   border: 1px solid #e1e5eb;"
                           "   border-radius: 10px;"
                           "   color: #1f2d3d;"
                           "}"
                           "QPushButton:hover {"
                           "   background: #eef3ff;"
                           "   border-color: #b5c8ff;"
                           "}");
        tasksListLayout->addWidget(btn);

        connect(btn, &QPushButton::clicked, this, [this, t]() {
            loadTask(t);
            emit getDescrition(t.id);
        });
    }

    tasksListLayout->addStretch();
}


void TaskEditWindow::loadTask(const Task &t)
{
    m_task = t;

    m_pTaskName->setText(t.taskName);
    m_pDueDate->setText(t.due_date);
    m_priority->setText(QString::number(t.priority));
}


void TaskEditWindow::setDescription(const QString &des)
{
    QJsonDocument doc = QJsonDocument::fromJson(des.toUtf8());
    QJsonObject obj = doc.object();


    descriptionEdit->setText(obj["main"].toString());


    QLayoutItem *child;
    while ((child = subtasksLayout->takeAt(0)) != nullptr) {
        if (child->widget()) child->widget()->deleteLater();
        delete child;
    }


    QJsonArray arr = obj["tasks"].toArray();

    for (auto v : arr) {
        QJsonObject o = v.toObject();

        QString text = o["text"].toString();
        bool done = o["done"].toBool();

        QWidget *row = new QWidget(this);
        QHBoxLayout *rowLayout = new QHBoxLayout(row);
        rowLayout->setContentsMargins(0, 0, 0, 0);


        QCheckBox *cb = new QCheckBox(text, row);
        cb->setChecked(done);
        cb->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);


        QPushButton *editBtn = new QPushButton("Edit", row);
       editBtn->setFixedWidth(50);


        QPushButton *delBtn = new QPushButton("X", row);
        delBtn->setFixedWidth(30);
        delBtn->setStyleSheet(
            "QPushButton {"
            "   color: red;"
            "   font-weight: bold;"
            "   padding: 2px;"
            "}"
            );



        rowLayout->addWidget(cb);
        rowLayout->addWidget(editBtn);
        rowLayout->addWidget(delBtn);

        subtasksLayout->addWidget(row);


        subtasksCheckBoxes.append(cb);


        connect(editBtn, &QPushButton::clicked, this, [cb]() {
            bool ok;
            QString newText = QInputDialog::getText(
                nullptr,
                "Edit subtask",
                "New name:",
                QLineEdit::Normal,
                cb->text(),
                &ok
                );

            if (ok && !newText.isEmpty()) {
                cb->setText(newText);
            }
        });


        connect(delBtn, &QPushButton::clicked, this, [this, row, cb]() {
            subtasksCheckBoxes.removeOne(cb);
            row->deleteLater();
        });
    }

    subtasksLayout->addStretch();


    subtasksLayout->addStretch();


    subtasksLayout->addStretch();
}
void TaskEditWindow::onButtonSave()
{

    QString mainText = descriptionEdit->toPlainText();


    QJsonArray subtasksArray;


    for (int i = 0; i < subtasksLayout->count(); ++i) {
        QWidget *row = subtasksLayout->itemAt(i)->widget();
        if (!row) continue;


        QCheckBox *cb = row->findChild<QCheckBox*>();
        if (!cb) continue;

        QJsonObject sub;
        sub["text"] = cb->text();
        sub["done"] = cb->isChecked();

        subtasksArray.append(sub);
    }


    QJsonObject root;
    root["main"] = mainText;
    root["tasks"] = subtasksArray;

    QJsonDocument doc(root);
    QString jsonString = doc.toJson(QJsonDocument::Compact);


    emit saveTask(m_task.id, jsonString);

    qInfo()<<jsonString;
    close();
}

