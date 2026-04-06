#include "UserManagementWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>

UserManagementWindow::UserManagementWindow(QWidget *parent)
    : QDialog(parent)
{
    table = new QTableView(this);
    model = new QStandardItemModel(this);

    model->setColumnCount(5);
    model->setHorizontalHeaderLabels({"ID", "Name", "Surname", "Job Title", "Banned"});

    table->setModel(model);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->horizontalHeader()->setStretchLastSection(true);

    btnBan     = new QPushButton(tr("Block"));
    btnUnban   = new QPushButton(tr("Unblock"));
    btnRefresh = new QPushButton(tr("Update"));

    connect(btnBan,     &QPushButton::clicked, this, &UserManagementWindow::onBanClicked);
    connect(btnUnban,   &QPushButton::clicked, this, &UserManagementWindow::onUnbanClicked);
    connect(btnRefresh, &QPushButton::clicked, this, &UserManagementWindow::onRefreshClicked);

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addWidget(btnBan);
    btnLayout->addWidget(btnUnban);
    btnLayout->addWidget(btnRefresh);

    btnUnban->setObjectName("unbanButton");
    btnRefresh->setObjectName("refreshButton");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(table);
    mainLayout->addLayout(btnLayout);

    setLayout(mainLayout);
    setWindowTitle(tr("List of users"));
    resize(700, 400);

}

void UserManagementWindow::setUsers(const QList<User>& users)
{
    model->removeRows(0, model->rowCount());

    for (const User& u : users) {
        QList<QStandardItem*> row;
        row << new QStandardItem(QString::number(u.id));
        row << new QStandardItem(u.name);
        row << new QStandardItem(u.surname);
        row << new QStandardItem(u.jobTitle);
        row << new QStandardItem(QString::number(u.isBanned));

        model->appendRow(row);
    }

    table->resizeColumnsToContents();
}

int UserManagementWindow::getSelectedUserId() const
{
    QModelIndex index = table->currentIndex();
    if (!index.isValid())
        return -1;

    return model->item(index.row(), 0)->text().toInt();
}

void UserManagementWindow::onBanClicked()
{
    int id = getSelectedUserId();
    if (id > 0)  emit banUserRequested(id);
}

void UserManagementWindow::onUnbanClicked()
{
    int id = getSelectedUserId();
    if (id > 0) emit unbanUserRequested(id);

}

void UserManagementWindow::onRefreshClicked()
{
    emit refreshRequested();
}
