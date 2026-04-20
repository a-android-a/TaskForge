#include "TaskCard.h"
#include <QDrag>
#include <QMimeData>
#include <QApplication>
#include <QGraphicsDropShadowEffect>
#include <QPushButton>
#include <QWidget>
#include <QMouseEvent>

TaskCard::TaskCard(const Task &task, QWidget *parent)
    : QWidget(parent), m_task(task)
{
    setObjectName("taskCard"); // внешний контейнер (прозрачный)

    QVBoxLayout *outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->setSpacing(0);

    // ВНУТРЕННИЙ КОНТЕЙНЕР — ОСНОВНАЯ КАРТОЧКА
    m_pMainWidgetCard = new QWidget(this);
    m_pMainWidgetCard->setObjectName("taskCardMain");

    // ТЕНЬ — на основном контейнере
    // QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(m_pMainWidgetCard);
    // shadow->setBlurRadius(12);
    // shadow->setColor(QColor(0, 0, 0, 80));
    // shadow->setOffset(3, 4);
    // m_pMainWidgetCard->setGraphicsEffect(shadow);

    QVBoxLayout *innerLayout = new QVBoxLayout(m_pMainWidgetCard);
    innerLayout->setContentsMargins(16, 16, 16, 16);
    innerLayout->setSpacing(8);

    QLabel *title = new QLabel(task.taskName, m_pMainWidgetCard);
    title->setObjectName("taskCardTitle");
    title->setWordWrap(true);

    QLabel *prio = new QLabel(QString("Priority: %1").arg(task.priority), m_pMainWidgetCard);
    prio->setObjectName("taskCardPriority");
    prio->setProperty("priority", task.priority);

    QLabel *due = new QLabel("before: " + task.due_date, m_pMainWidgetCard);
    due->setObjectName("taskCardDue");

    // QLabel *desc = new QLabel(task.description, m_pMainWidgetCard);
    // desc->setObjectName("taskCardDescription");
    // desc->setWordWrap(true);

    QPushButton *btnDetails = new QPushButton("More detailed", m_pMainWidgetCard);
    btnDetails->setObjectName("taskCardButton");

    connect(btnDetails, &QPushButton::clicked, this, [this]() {
        emit cardClicked(m_task.id);
    });

    innerLayout->addWidget(title);
    innerLayout->addWidget(prio);
    innerLayout->addWidget(due);
   // innerLayout->addWidget(desc);
   // innerLayout->addStretch();
    innerLayout->addWidget(btnDetails);

    outerLayout->addWidget(m_pMainWidgetCard);
}

void TaskCard::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragStartPosition = event->pos();
        QWidget::mousePressEvent(event);
        //emit cardClicked(m_task.id);
    }
}

void TaskCard::mouseMoveEvent(QMouseEvent *event)
{
    if (!(event->buttons() & Qt::LeftButton))
        return;

    // Check distance from start click position
    if ((event->pos() - m_dragStartPosition).manhattanLength() < QApplication::startDragDistance())
        return;

    // Start dragging
    QDrag *drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData;

    //  transmitting task ID
    mimeData->setProperty("cardPtr", QVariant::fromValue<void*>(this));
    mimeData->setData("application/x-task-id", QByteArray::number(m_task.id));

    drag->setMimeData(mimeData);

    //  Shows the card in drag-and-drop mode
    QPixmap pixmap = this->grab();
    drag->setPixmap(pixmap.scaled(200, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    drag->setHotSpot(event->pos());

    // Start drag
    Qt::DropAction action = drag->exec(Qt::MoveAction);

    if (action == Qt::MoveAction) {
        //this->hide();
    }
}
