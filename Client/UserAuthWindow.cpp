#include "UserAuthWindow.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QSize>

UserAuthWindow::UserAuthWindow(QWidget* parent){

    m_pLogin    = new QLineEdit();
    m_pPassword = new QLineEdit();
    m_pEnter    = new QPushButton(tr("Enter"));
    vBox        = new QVBoxLayout();

    vBox->addWidget(new QLabel(tr("Login")));
    vBox->addWidget(m_pLogin);
    vBox->addWidget(new QLabel(tr("Password")));
    vBox->addWidget(m_pPassword);
    vBox->addWidget(m_pEnter);

    this->setLayout(vBox);
    this->resize(QSize(300,300));
}

void UserAuthWindow::slotButton(){

}
