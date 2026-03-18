#pragma once
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QMouseEvent>
#include"../Server/Task.h"



class TaskCard : public QWidget
{
    Q_OBJECT

public:
    explicit TaskCard(const Task &task, QWidget *parent = nullptr);

     int taskId() const { return m_task.id; }
    // QString status() const { return m_task.status; }

signals:
    void cardClicked(int taskId);
    void cardDropped(int taskId, const QString &newStatus);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    Task m_task;
    QLabel *m_titleLabel;
    QLabel *m_priorityLabel;
    QLabel *m_dueLabel;
    QPoint m_dragStartPosition;
    QWidget* m_pMainWidgetCard;
};
