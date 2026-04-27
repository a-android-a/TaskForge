#pragma once
#include <QWidget>
#include <QString>
#include <QVector>
#include "../Server/Task.h"
#include "network/ApiClient.h"
class QLabel;
class QCheckBox;
class QVBoxLayout;
class QTextEdit;
class QProgressBar;
class QScrollArea;
class QLineEdit;
class QPushButton;
class TaskEditWindow:public QWidget{
    Q_OBJECT;
private:
    QVBoxLayout* VBox;
    Task m_task;
    QString description;
    qint64 m_taskId;
    ApiClient* m_pApiClient;
    QLabel       *titleLabelValue;
    QTextEdit    *descriptionEdit;
    QLabel       *dueDateLabel;
    QLabel       *priorityValue;
    QCheckBox    *statusNotStarted;
    QCheckBox    *statusInProgress;
    QCheckBox    *statusDone;
    QProgressBar *progressBar;
    QVBoxLayout  *mainLayout;
    QVBoxLayout  *LayoutCheckBox;
    QScrollArea *scrollArea;
    QWidget *scrollWidget;
    QVector<Task> tasks;
    QVector<QCheckBox*> subtasksCheckBoxes;

    QPushButton *saveBtn;
    QPushButton *closeBtn;

    QScrollArea *tasksArea;
    QVBoxLayout *tasksListLayout;

    // подзадачи
    QScrollArea *subtasksArea;
    QVBoxLayout *subtasksLayout;

    QLineEdit* m_pTaskName;
    QLineEdit* m_pDueDate;
    QLineEdit* m_priority;


    void updateProgress();
private slots:
    void onButtonSave();
    void loadTask(const Task &t);

signals:
    void taskUpdated(const Task &task, const QString &descriptionJson);
    void getDescrition(const qint64 id);
    void saveTask(const qint64 id, const QString& json);

public:
    explicit TaskEditWindow(QWidget *parent = nullptr, const Task &task=  Task());

    void setDescription(const QString &desc);
    void setTasks(const QVector<Task>& tasks);
    bool setStyle(const QString &styleFileName);


};
