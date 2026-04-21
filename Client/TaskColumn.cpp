#include "TaskColumn.h"
#include "TaskCard.h"
#include <QMimeData>
#include <QDebug>
#include <QScrollArea>
#include <QLayoutItem>
#include <QStyle>
TaskColumn::TaskColumn(const QString &title, QWidget *parent)
    : QWidget(parent)
{
    setAcceptDrops(true);
    setMinimumWidth(280);

    setObjectName("taskColumn");

    if (title == "Not started")      m_status = 0;
    else if (title == "At work")     m_status = 1;
    else if (title == "Completed")   m_status = 2;
    else                             m_status = -1;

    // ВНЕШНИЙ ЛЕЙАУТ
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // ВНУТРЕННИЙ КОНТЕЙНЕР КОЛОНКИ
    QWidget *columnBody = new QWidget(this);
    columnBody->setObjectName("taskColumnBody");

    QVBoxLayout *bodyLayout = new QVBoxLayout(columnBody);
    bodyLayout->setContentsMargins(8, 8, 8, 8);
    bodyLayout->setSpacing(0);

    // Заголовок
    m_titleLabel = new QLabel(title, columnBody);
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_titleLabel->setObjectName("taskColumnTitle");
    bodyLayout->addWidget(m_titleLabel);

    // Scroll area
    QScrollArea *scrollArea = new QScrollArea(columnBody);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setObjectName("taskColumnScroll");

    QWidget *cardsContainer = new QWidget(scrollArea);
    cardsContainer->setObjectName("taskCardsContainer");

    m_layout = new QVBoxLayout(cardsContainer);
    m_layout->setSpacing(12);
    m_layout->setContentsMargins(8, 8, 8, 8);
    m_layout->addStretch();

    scrollArea->setWidget(cardsContainer);
    bodyLayout->addWidget(scrollArea);

    mainLayout->addWidget(columnBody);
}


void TaskColumn::addTaskCard(TaskCard *card)
{
    m_layout->insertWidget(m_layout->count() - 1, card);
    //m_layout->addWidget(card);

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
        setProperty("hover", true);
        style()->unpolish(this);
        style()->polish(this);
    }
}

void TaskColumn::dragLeaveEvent(QDragLeaveEvent *event)
{
    // Return to normal style
    setProperty("hover", false);
    style()->unpolish(this);
    style()->polish(this);
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


    setProperty("hover", false);
    style()->unpolish(this);
    style()->polish(this);
    event->acceptProposedAction();
}

void TaskColumn::clearCards(){



    while (m_layout->count() > 1) {
        QLayoutItem* item = m_layout->takeAt(0);

        if (QWidget* w = item->widget()) {
            w->deleteLater();   // корректно удаляет TaskCard
        }

        delete item;
    }

}
int TaskColumn::status()  { return m_status; }


