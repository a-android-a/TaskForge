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

    QVBoxLayout *outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->setSpacing(0);


    m_pMainWidgetCard = new QWidget(this);


    m_pMainWidgetCard->setStyleSheet(
        "background: #ffffff;"
        "border: 1px solid #e0e0e0;"
        "border-radius: 12px;"
    );


    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(m_pMainWidgetCard);
    shadow->setBlurRadius(12);
    shadow->setColor(QColor(0, 0, 0, 80));
    shadow->setOffset(3, 4);
    m_pMainWidgetCard->setGraphicsEffect(shadow);


    QVBoxLayout *innerLayout = new QVBoxLayout(m_pMainWidgetCard);
    innerLayout->setContentsMargins(16, 16, 16, 16);
    innerLayout->setSpacing(8);


    QLabel *title = new QLabel(task.taskName, m_pMainWidgetCard);
    title->setWordWrap(true);
    title->setStyleSheet("font-weight: bold; font-size: 15px; color: #111111;");



    QLabel *prio = new QLabel(QString(tr("Priority: %1")).arg(task.priority), m_pMainWidgetCard);
    QString prioColor = (task.priority <= 2) ? "#c62828" : (task.priority == 3 ? "#f57c00" : "#2e7d32");
    prio->setStyleSheet(QString("color: %1; font-size: 13px;").arg(prioColor));



    QLabel *due = new QLabel(tr("before: ") + task.due_date, m_pMainWidgetCard);
    due->setStyleSheet("color: #555555; font-size: 13px;");


    QLabel *desc = new QLabel(task.description, m_pMainWidgetCard);
    desc->setWordWrap(true);
    desc->setStyleSheet("color: #666666; font-size: 12px; line-height: 1.4;");


    QPushButton *btnDetails = new QPushButton(tr("More detailed"), m_pMainWidgetCard);
    btnDetails->setStyleSheet(
        "background: #1976d2; color: white; border: none; border-radius: 6px; padding: 8px;"
        "font-weight: bold;"
        );
    connect(btnDetails, &QPushButton::clicked, this, [this]() {
        emit cardClicked(m_task.id);
    });


    innerLayout->addWidget(title);
    innerLayout->addWidget(prio);
    innerLayout->addWidget(due);
    innerLayout->addWidget(desc);
    innerLayout->addStretch();
    innerLayout->addWidget(btnDetails);


    outerLayout->addWidget(m_pMainWidgetCard);
}

void TaskCard::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragStartPosition = event->pos();
        QWidget::mousePressEvent(event);
        emit cardClicked(m_task.id);
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
