#pragma once
#include<QWidget>
class QLineEdit;
class QPushButton;
class QVBoxLayout;

class UserAuthWindow:public QWidget{
    Q_OBJECT
private:
    QLineEdit*   m_pLogin;
    QLineEdit*   m_pPassword;
    QPushButton* m_pEnter;
    QVBoxLayout* vBox;
public:
    UserAuthWindow(QWidget *parent = nullptr);
    bool setStyle(const QString &styleFileName);

private slots:
    void slotButton();

};
