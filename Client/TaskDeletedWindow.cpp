#include "TaskDeletedWindow.h"
#include <QVBoxLayout>
#include <QScrollArea>
#include <QPushButton>
#include <QLabel>
#include <QFile>
#include <QString>
TaskDeletedWindow::TaskDeletedWindow(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("Delete Task");
    setObjectName("TaskDeletedWindow");

    resize(400, 500);

    mainLayout = new QVBoxLayout(this);


    QLabel *label = new QLabel(tr("Select the task to delete:"), this);
    mainLayout->addWidget(label);

    tasksArea = new QScrollArea(this);
    tasksArea->setWidgetResizable(true);
    mainLayout->addWidget(tasksArea);

    QWidget *tasksWidget = new QWidget(this);
    tasksListLayout = new QVBoxLayout(tasksWidget);
    tasksWidget->setLayout(tasksListLayout);

    tasksArea->setWidget(tasksWidget);


    deleteBtn = new QPushButton(tr("Delete a selected task"), this);
    deleteBtn->setStyleSheet("font-size: 18px; padding: 10px;");
    mainLayout->addWidget(deleteBtn);
    deleteBtn->setObjectName("DeleteButton");



    connect(deleteBtn, &QPushButton::clicked, this, [this]() {
        if (selectedTaskId != -1)
            emit deleteTask(selectedTaskId);
    });
}

void TaskDeletedWindow::setTasks(const QVector<Task> &tasks)
{

    QLayoutItem *child;
    while ((child = tasksListLayout->takeAt(0)) != nullptr) {
        if (child->widget()) child->widget()->deleteLater();
        delete child;
    }

    for (const Task &t : tasks) {
        QPushButton *btn = new QPushButton(t.taskName, this);
        btn->setStyleSheet(
            "text-align: left; padding: 10px; font-size: 16px;"
            "border: 1px solid #ccc; border-radius: 6px;"
            );

        tasksListLayout->addWidget(btn);

        connect(btn, &QPushButton::clicked, this, [this, t, btn]() {
            selectedTaskId = t.id;


            for (auto *w : this->findChildren<QPushButton*>()) {
                w->setStyleSheet(
                    "text-align: left; padding: 10px; font-size: 16px;"
                    "border: 1px solid #ccc; border-radius: 6px;"
                    );
            }

            btn->setStyleSheet(
                "text-align: left; padding: 10px; font-size: 16px;"
                "border: 2px solid red; border-radius: 6px;"
                "background: #ffe5e5;"
                );
        });
    }

    tasksListLayout->addStretch();
}
bool TaskDeletedWindow::setStyle(const QString &styleFileName)
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
