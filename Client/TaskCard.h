#pragma once
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QMouseEvent>
#include"../Server/Task.h"
#include"TaskColumn.h"


class TaskCard : public QWidget
{
    Q_OBJECT

public:
    explicit TaskCard(const Task &task, QWidget *parent = nullptr);

     int taskId() const { return m_task.id; }
    // QString status() const { return m_task.status; }


 public:
     void setColumn(TaskColumn* col) { m_column = col; }
     TaskColumn* column() const { return m_column; }


signals:
    void cardClicked(int taskId);
    void cardDropped(int taskId, const QString &newStatus);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    TaskColumn* m_column = nullptr;
    Task m_task;
    QLabel *m_titleLabel;
    QLabel *m_priorityLabel;
    QLabel *m_dueLabel;
    QPoint m_dragStartPosition;
    QWidget* m_pMainWidgetCard;
};
