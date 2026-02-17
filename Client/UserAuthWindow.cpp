#include "UserAuthWindow.h"
#include "network/ApiClient.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QSize>
#include <QFile>
#include <QString>
#include <QCryptographicHash>
#include <QByteArray>
#include <QMessageBox>
UserAuthWindow::UserAuthWindow(QWidget* parent){

    m_pLogin    = new QLineEdit();
    m_pPassword = new QLineEdit();
    m_pEnter    = new QPushButton(tr("Enter"));
    vBox        = new QVBoxLayout();
    m_apiClient = new ApiClient(this);

    vBox->addWidget(new QLabel(tr("Login")));
    vBox->addWidget(m_pLogin);
    vBox->addWidget(new QLabel(tr("Password")));
    vBox->addWidget(m_pPassword);
    vBox->addWidget(m_pEnter);

    this->setLayout(vBox);
    this->resize(QSize(300,300));
    connect(m_pEnter,&QPushButton::clicked, this, &UserAuthWindow::slotButton);


}

void UserAuthWindow::slotButton(){

    QString login = m_pLogin->text().trimmed();
    QString password = m_pPassword->text();

    if (login.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, tr("Missing information"),tr("Both login and password fields are required"));
        return;
    }

    m_apiClient->authenticate(login, password);
}

bool UserAuthWindow::setStyle(const QString &styleFileName){
    QFile file(styleFileName);
    if(file.open(QFile::ReadOnly)){
        QString strCSS = QLatin1String(file.readAll());
        this->setStyleSheet(strCSS);
        file.close();
        return true;
    }
    return false;

}
QString UserAuthWindow::hashString(const QString &str){
    QByteArray hash = QCryptographicHash::hash(str.toUtf8(), QCryptographicHash::Sha256);
    QString hashHex = hash.toHex();
    return hashHex;
}

