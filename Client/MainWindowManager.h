#pragma once
#include <QWidget>
#include <QString>
#include "network/ApiClient.h"
#include <QScrollArea>
class QLineEdit;
class QPushButton;
class QVBoxLayout;

class MainWindowManager:public QWidget{
    Q_OBJECT;
private:
    QVBoxLayout* m_pVbox;
    QPushButton* m_AddTask;
    QPushButton* m_AddChekBox;
    QLineEdit*   m_pTaskName;
    QLineEdit*   m_pDueDate;
    QLineEdit*   m_pCreatedBy;
    QLineEdit*   m_pAssignedTo;
    QLineEdit*   m_pDescription;
    QLineEdit*   m_pPriority;
    ApiClient*   m_apiClient;
    QScrollArea* m_scrollArea;
    QWidget*     m_scrollWidget;
    QVBoxLayout* m_checkBoxLayout;
private slots:
    void onButtonAddTask();
    void onButtonAddChekBox();
public:
    explicit MainWindowManager(QWidget *parent = nullptr);
    void setApiClient(ApiClient* apiClient);
    bool setStyle(const QString& fileStyle);
};
