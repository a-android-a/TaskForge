#include "TaskColumn.h"
#include "TaskCard.h"
#include <QMimeData>
#include <QDebug>
#include <QScrollArea>
#include <QLayoutItem>
TaskColumn::TaskColumn(const QString &title, QWidget *parent)
    : QWidget(parent)
     // todo, in_progress, done
{
    setAcceptDrops(true);
    setMinimumWidth(280);

    // Main column style (background, border)
    setStyleSheet("background: #f5f5f5; border: 1px solid #d0d0d0; border-radius: 8px;");

    if (title == "Not started")      m_status = 0;
    else if (title == "At work")     m_status = 1;
    else if (title == "Completed")   m_status = 2;
    else                             m_status = -1;
    // Main vertical layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(8, 8, 8, 8);
    mainLayout->setSpacing(0);

    // Column title
    m_titleLabel = new QLabel(title, this);
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_titleLabel->setStyleSheet(
        "font-weight: bold; "
        "font-size: 16px; "
        "color: white; "
        "background: #424242; "
        "padding: 12px; "
        "border-radius: 8px 8px 0 0;"
        );
    mainLayout->addWidget(m_titleLabel);

    // Scroll area
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setStyleSheet("QScrollArea { border: none; background: transparent; }");

    // Container for cards
    QWidget *cardsContainer = new QWidget(scrollArea);
    m_layout = new QVBoxLayout(cardsContainer);
    m_layout->setSpacing(12);
    m_layout->setContentsMargins(8, 8, 8, 8);
    m_layout->addStretch();

    scrollArea->setWidget(cardsContainer);

    // add scroll to main layout
    mainLayout->addWidget(scrollArea);
}

void TaskColumn::addTaskCard(TaskCard *card)
{
    m_layout->insertWidget(m_layout->count() - 1, card);
}

void TaskColumn::removeTaskCard(TaskCard *card)
{
    m_layout->removeWidget(card);
}

void TaskColumn::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-task-id")) {
        event->acceptProposedAction();
        // Highlight on hover
        setStyleSheet("background: #e8f5e9; border: 2px dashed #4caf50; border-radius: 8px;");
    }
}

void TaskColumn::dragLeaveEvent(QDragLeaveEvent *event)
{
    // Return to normal style
    setStyleSheet("background: #f5f5f5; border: 1px solid #d0d0d0; border-radius: 8px;");
}

void TaskColumn::dropEvent(QDropEvent *event)
{
    if (!event->mimeData()->hasFormat("application/x-task-id"))
        return;

    TaskCard* card = reinterpret_cast<TaskCard*>(
        event->mimeData()->property("cardPtr").value<void*>()
        );

    if (!card)
        return;

    emit taskDropped(card, this);

    setStyleSheet("background: #f5f5f5; border: 1px solid #d0d0d0; border-radius: 8px;");
    event->acceptProposedAction();
}

void TaskColumn::clearCards()
{
    // Удаляем ВСЕ элементы, кроме stretch в конце
    while (m_layout->count() > 1) {
        QLayoutItem* item = m_layout->takeAt(0);

        if (QWidget* w = item->widget()) {
            w->deleteLater();   // корректно удаляет TaskCard
        }

        delete item;
    }
}
int TaskColumn::status()  { return m_status; }


