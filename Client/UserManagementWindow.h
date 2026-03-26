#pragma once
#include <QDialog>
#include <QTableView>
#include <QStandardItemModel>
#include <QPushButton>
#include "../Server/User.h"
class UserManagementWindow : public QDialog
{
    Q_OBJECT
public:
    explicit UserManagementWindow(QWidget *parent = nullptr);

    void setUsers(const QList<User>& users);  // метод для заполнения таблицы

signals:
    void banUserRequested(int userId);
    void unbanUserRequested(int userId);
    void refreshRequested();

private slots:
    void onBanClicked();
    void onUnbanClicked();
    void onRefreshClicked();

private:
    QTableView *table;
    QStandardItemModel *model;

    QPushButton *btnBan;
    QPushButton *btnUnban;
    QPushButton *btnRefresh;

    int getSelectedUserId() const;
};
