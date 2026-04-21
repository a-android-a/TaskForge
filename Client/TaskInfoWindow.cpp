#include "TaskInfoWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <QCheckBox>
#include <QProgressBar>
#include <QPushButton>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QScrollArea>
#include <QFile>
TaskInfoWindow::TaskInfoWindow(QWidget *parent,const Task &task)
    : QWidget(parent),m_task(task)
{
    setWindowTitle(tr("Task Information"));
    resize(520, 620);

    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);


    QLabel *titleLabel = new QLabel(tr("Task name: ") + task.taskName, this);
    mainLayout->addWidget(titleLabel);


    QLabel *descLabel = new QLabel(tr("Description:" ) , this);
    mainLayout->addWidget(descLabel);

    descriptionEdit = new QTextEdit(this);
    descriptionEdit->setMinimumHeight(120);
    descriptionEdit->setReadOnly(true);
    mainLayout->addWidget(descriptionEdit);


    QLabel *dueLabel = new QLabel(tr("Due date: ") +  task.due_date, this);
    mainLayout->addWidget(dueLabel);


    QLabel *priorityLabel = new QLabel(tr("Priority:") + QString::number(task.priority), this);
    mainLayout->addWidget(priorityLabel);


    QLabel *statusLabel = new QLabel(tr("Status: ") + QString::number(task.status), this);
    mainLayout->addWidget(statusLabel);

    QLabel *progressLabel = new QLabel(tr("Progress:"), this);
    mainLayout->addWidget(progressLabel);

    progressBar = new QProgressBar(this);
    progressBar->setRange(0, 100);
    progressBar->setValue(0);
    mainLayout->addWidget(progressBar);


    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    mainLayout->addWidget(scrollArea);


    QWidget *scrollWidget = new QWidget(this);
    LayoutCheckBox = new QVBoxLayout(scrollWidget);
    scrollWidget->setLayout(LayoutCheckBox);

    scrollArea->setWidget(scrollWidget);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    QPushButton *saveBtn   = new QPushButton(tr("Save"), this);
    QPushButton *closeBtn  = new QPushButton(tr("Close"), this);
    saveBtn->setObjectName("saveButton");
    closeBtn->setObjectName("closeButton");

    btnLayout->addWidget(saveBtn);
    btnLayout->addWidget(closeBtn);
    mainLayout->addLayout(btnLayout);
    connect(saveBtn,   &QPushButton::clicked, this, &TaskInfoWindow::onButtonSave );
}

void TaskInfoWindow::onButtonSave()
{
    QJsonArray tasksArray;
    for (QCheckBox *cb : subtaskCheckboxes) {
        QJsonObject t;
        t["text"] = cb->text();
        t["done"] = cb->isChecked();
        tasksArray.append(t);
    }

    QJsonObject descriptionObj;
    descriptionObj["main"] = descriptionEdit->toPlainText();

    descriptionObj["tasks"] = tasksArray;

    QString descriptionJson = QString::fromUtf8(
        QJsonDocument(descriptionObj).toJson(QJsonDocument::Compact)
        );

    emit taskUpdated(m_task, descriptionJson);
}


void TaskInfoWindow::setDescription(const QString &desc)
{
    description = desc;

    // Парсим JSON
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(desc.toUtf8(), &err);

    if (err.error != QJsonParseError::NoError) {
        descriptionEdit->setText("Ошибка парсинга JSON: " + err.errorString());
        return;
    }

    if (!doc.isObject()) {
        descriptionEdit->setText("Некорректный JSON формат");
        return;
    }

    QJsonObject obj = doc.object();

    // 1. Основной текст
    QString mainText = obj.value("main").toString();

    // 2. Подзадачи
    QString subtasksText;
    QJsonArray tasksArray = obj.value("tasks").toArray();

    for (const QJsonValue &v : tasksArray) {
        QJsonObject t = v.toObject();

        bool done = t.value("done").toBool();
        //QString text = t.value("text").toString();

        QCheckBox* checkBox = new QCheckBox(this);
        checkBox->setText(t.value("text").toString());
        checkBox->setChecked(t.value("done").toBool());
        LayoutCheckBox->addWidget(checkBox);
        subtaskCheckboxes.append(checkBox);
        connect(checkBox, &QCheckBox::stateChanged, this, [this](){
            updateProgress();
        });
    }
    updateProgress();
    descriptionEdit->setText(mainText);
}
void TaskInfoWindow::updateProgress()
{
    int total = subtaskCheckboxes.size();
    if (total == 0) {
        progressBar->setValue(0);
        return;
    }

    int doneCount = 0;
    for (QCheckBox *cb : subtaskCheckboxes) {
        if (cb->isChecked())
            doneCount++;
    }

    int percent = (doneCount * 100) / total;
    progressBar->setValue(percent);
}
bool TaskInfoWindow::setStyle(const QString &styleFileName){

    QFile file(styleFileName);
    if(file.open(QFile::ReadOnly)){
        QString strCSS = QLatin1String(file.readAll());
        this->setStyleSheet(strCSS);
        file.close();
        return true;
    }
    return false;
}

