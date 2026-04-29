#include "UserAuthWindow.h"
#include "network/ApiClient.h"
#include "MainWindowWorker.h"
#include "MainWindowManager.h"
#include "MainWindowAdmin.h"
#include "SettingsWindow.h"
#include "Config.h"
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
#include <QTimer>
#include <QString>
UserAuthWindow::UserAuthWindow(ApiClient* apiClient, QWidget *parent)
    : QWidget(parent)
    , m_apiClient(apiClient){

    m_pLogin    = new QLineEdit();
    m_pPassword = new QLineEdit();
    m_pEnter    = new QPushButton(tr("Enter"));
    m_pReconnect= new QPushButton(tr("Reconnect"));
    vBox        = new QVBoxLayout();

    QPushButton* settingsBtn = new QPushButton("⚙️", this);
    settingsBtn->setObjectName("settingsButton");
    settingsBtn->setFixedSize(28, 28);
    settingsBtn->setStyleSheet(
        "#settingsButton { border: none; font-size: 18px; }"
        "#settingsButton:hover { color: #4a90e2; }"
        );

    QHBoxLayout* topBar = new QHBoxLayout();
    topBar->addStretch();
    topBar->addWidget(settingsBtn);

    vBox->addLayout(topBar);




    vBox->addWidget(new QLabel(tr("Login")));
    vBox->addWidget(m_pLogin);
    vBox->addWidget(new QLabel(tr("Password")));
    vBox->addWidget(m_pPassword);
    vBox->addWidget(m_pEnter);
    vBox->addWidget(m_pReconnect);

    this->setLayout(vBox);
    this->resize(QSize(300,300));
    connect(m_pEnter,    &QPushButton::clicked,             this,&UserAuthWindow::slotButton             );
    connect(m_apiClient, &ApiClient::authorizationFailed,   this,&UserAuthWindow::slotAuthorizationFailed);
    connect(m_apiClient, &ApiClient::authorizationOk,       this,&UserAuthWindow::slotAuthorizationOk    );
    connect(m_apiClient, &ApiClient::createWorkerWindow,    this,&UserAuthWindow::slotCreateWorkerWindow );
    connect(m_apiClient, &ApiClient::createAdminWindow,     this,&UserAuthWindow::slotCreateAdminWindow  );
    connect(m_apiClient, &ApiClient::createManagerWindow,   this,&UserAuthWindow::slotCreateManagerWindow);
    connect(m_pReconnect,&QPushButton::clicked,             this,&UserAuthWindow::slotButtonReconnect    );
    connect(settingsBtn, &QPushButton::clicked, this, [](){
        SettingsWindow* w = new SettingsWindow();
        w->setStyle(Config::instance().getStylePath());
        w->show();
    });

    m_apiClient->connectToServer();
    m_pLogin->setObjectName("authLogin");
    m_pPassword->setObjectName("authPassword");
    m_pEnter->setObjectName("authButton");
    m_pReconnect->setObjectName("reconnectButton");
    this->setObjectName("authWindow");
    m_pPassword->setEchoMode(QLineEdit::Password);

    Config::instance().load();


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
void UserAuthWindow::slotButtonReconnect(){
    m_apiClient->connectToServer();
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

void UserAuthWindow::slotAuthorizationFailed(){
    QMessageBox::warning(this, tr("Missing information"),tr("login or password incorrect"));
}


QString UserAuthWindow::hashString(const QString &str){
    QByteArray hash = QCryptographicHash::hash(str.toUtf8(), QCryptographicHash::Sha256);
    QString hashHex = hash.toHex();
    return hashHex;
}
void UserAuthWindow::ShowWindow(){
    this->show();
}
void UserAuthWindow::slotAuthorizationOk(){

}
void UserAuthWindow::slotCreateAdminWindow(const User& user){
    MainWindowAdmin* w = new MainWindowAdmin(nullptr);
    w->setApiClient(m_apiClient);
    // w->setStyle("style/stylesMainWindowWorkerLight.qss");
    //w->setUser(user);
    //QString path = Config::instance().getStylePath();
    w->setStyle(Config::instance().getStylePath());
    this->hide();
    w->show();
    connect(w, &MainWindowAdmin::ButtonLogOut,   this,&UserAuthWindow::ShowWindow);
}
void UserAuthWindow::slotCreateManagerWindow(const User& user){
    MainWindowManager* w = new MainWindowManager(nullptr);
    w->setApiClient(m_apiClient);
    w->setStyle(Config::instance().getStylePath());
    this->hide();
    w->show();
    connect(w, &MainWindowManager::ButtonLogOut,   this,&UserAuthWindow::ShowWindow);
}
void UserAuthWindow::slotCreateWorkerWindow(const User& user){
    MainWindowWorker* w = new MainWindowWorker(nullptr);
    w->setApiClient(m_apiClient);
    w->setStyle(Config::instance().getStylePath());
    w->setUser(user);
    this->hide();
    w->show();
    connect(w, &MainWindowWorker::ButtonLogOut,   this,&UserAuthWindow::ShowWindow);
}

