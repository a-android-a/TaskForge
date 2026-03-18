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
    void clearCards();
    int status();

signals:
    void taskDropped(TaskCard* card, TaskColumn* column);
protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;
    void dropEvent(QDropEvent *event)           override;

private:
    int         m_status;
    QLabel      *m_titleLabel;
    QVBoxLayout *m_layout;
};
