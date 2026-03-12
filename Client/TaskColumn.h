#pragma once
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QString>
class TaskCard;

class TaskColumn : public QWidget
{
    Q_OBJECT

public:
    explicit TaskColumn(const QString &title, QWidget *parent = nullptr);

    void addTaskCard(TaskCard *card);
    void removeTaskCard(TaskCard *card);

    QString getStatus() const { return m_status; }

signals:
    void taskDropped(int taskId, const QString &newStatus);
protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;
    void dropEvent(QDropEvent *event)           override;

private:
    QString     m_status;
    QLabel      *m_titleLabel;
    QVBoxLayout *m_layout;
};
