#pragma once
#include <QWidget>
#include <QString>
#include <QVector>
#include "../Server/Task.h"
class QLabel;
class QCheckBox;
class QVBoxLayout;
class QTextEdit;
class QProgressBar;
class QScrollArea;

class TaskInfoWindow:public QWidget{
    Q_OBJECT;
private:
    QVBoxLayout* VBox;
    Task m_task;
    QString description;
    qint64 m_taskId;

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
    QVector<QCheckBox*> subtaskCheckboxes;

    void updateProgress();


public:
    explicit TaskInfoWindow(QWidget *parent = nullptr, const Task &task=  Task());
    void setDescription(const QString &desc);
    bool setStyle(const QString &styleFileName);

};
