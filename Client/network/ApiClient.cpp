#include "ApiClient.h"
#include "../Config.h"
#include "../../Server/User.h"
#include "../MainWindowWorker.h"

#include <QSslConfiguration>
#include <QList>
#include <QAbstractSocket>
#include <QMessageBox>
#include <QCoreApplication>
#include <QByteArray>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QJsonArray>
#include <QDateTime>
ApiClient::ApiClient(QObject* parent): QSslSocket(parent)
{
    connect(this, &QSslSocket::readyRead, this, &ApiClient::slotReadyRead);
    connect(this, &QSslSocket::errorOccurred,this, [](QAbstractSocket::SocketError err){ qWarning() << "Error:" << err; });

    User user;

}
void ApiClient::authenticate(const QString &login, const QString password){
    QJsonObject messageObj;

    messageObj["type"    ] = "authorization";
    messageObj["time"    ] =  QTime::currentTime().toString("hh:mm:ss");;
    messageObj["login"   ] = login;
    messageObj["password"] = password;

    QJsonDocument doc(messageObj);


    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
    jsonData += '\n';
    this->write(jsonData);
}
void ApiClient::connectToServer()
{


    if (this->state() != QAbstractSocket::UnconnectedState) {
        this->abort();
    }
    Config::instance().setSettingsFile("config.json");

    qInfo()<< Config::instance().getServerHost();
    qInfo()<< Config::instance().getServerPort();
    qInfo()<< Config::instance().getCertificate();

    QSslConfiguration sslConfig = this->sslConfiguration();
    QList<QSslCertificate> caCertificates = sslConfig.caCertificates();
    QList<QSslCertificate> newCerts = QSslCertificate::fromPath(Config::instance().getCertificate());

    if (newCerts.isEmpty()) {

        QMessageBox::warning(nullptr,
                             tr("Certificate Error"),
                             tr("Failed to load server certificate (server.crt).\n"
                                "Please ensure the file is in the application directory."));


    } else {

        caCertificates.append(newCerts.constFirst());

    }
    sslConfig.setCaCertificates(caCertificates);
    this->setSslConfiguration(sslConfig);


    connect(this, &QAbstractSocket::errorOccurred, this, [this](QAbstractSocket::SocketError err) {
        QString errMsg = tr("Failed to connect to server:\n%1").arg(this->errorString());
        QMessageBox::warning(nullptr, tr("Connection Error"), errMsg);
        emit ConnectionError();
    });


    this->connectToHostEncrypted(Config::instance().getServerHost(), 4433);
    emit ConnectionOk();

}
void ApiClient::sendToServer(const QString &data)
{
    QJsonObject messageObj;
    messageObj["type"] = data;
    messageObj["time"] =  QTime::currentTime().toString("hh:mm:ss");;
    QJsonDocument doc(messageObj);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
    jsonData += '\n';
    this->write(jsonData);
}
void ApiClient::slotReadyRead(){

    QByteArray rawLine = this->readAll();
    QString message = QString::fromUtf8(rawLine).trimmed();
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(message .toUtf8(), &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        qDebug() << "JSON error:" << parseError.errorString();
        // continue;
    }


    if (!doc.isObject()) {

        //  continue;
    }

    QJsonObject obj = doc.object();

    QString type = obj["type"].toString();

    qInfo()<<type;
    if(type == "authorization_response"){
        QString status = obj["status"].toString();
        if(status == "ok"){
            qInfo()<<"ok";
            user.id        = obj["id"].toInt();
            //user.createdAt = obj["created_at"].toString();
            user.name      = obj["name"].toString();
            user.surname   = obj["surname"].toString();
            user.jobTitle  = obj["job_title"].toString();
            qInfo()<< user.jobTitle;
            emit authorizationOk();
            if(user.jobTitle == "worker"){
                emit createWorkerWindow(user);
            }else if(user.jobTitle == "manager"){
                emit createManagerWindow(user);
            }
            else if(user.jobTitle == "admin"){
                emit createAdminWindow(user);
            }


        }else  {

            qInfo()<<"status:error";
            emit authorizationFailed();
        }
    }
    if(type == "allTasks_response"){

        QString status = obj["status"].toString();
        if (status == "ok") {
            QJsonArray tasksArray = obj["tasks"].toArray();

            //qInfo() << "tasks size:" << tasksArray.size();


            emit tasksReceived(tasksArray);
        } else {
            qWarning() << "error" << obj["message"].toString();
        }
    }
    if(type == "updateTasks_response"){
        //qInfo() << "tasks size:" << tasksArray.size();
        getAllTasks();
    }
    if (type == "getAllUsers_response") {

        QList<User> users;

        QJsonArray arr = obj["users"].toArray();
        for (const QJsonValue &val : arr) {
            QJsonObject obj = val.toObject();

            User u;
            u.id        = obj["userId"].toInt();
            u.name      = obj["name"].toString();
            u.surname   = obj["surname"].toString();
            u.jobTitle  = obj["job_title"].toString();
            u.isBanned  = obj["is_Banned"].toInt();

            users.append(u);
        }

        emit usersListReceived(users);
    }
    if (type == "description_response") {
        QString description = obj["description"].toString();
        emit descriptionListReceived(description);
    }



}

void ApiClient::createTask(const Task& task,const QString des){
    QJsonObject messageObj;
    messageObj["type"]        = "createTask";
    messageObj["id"]          = QString::number(task.id);
    messageObj["status"]      = QString::number(task.status);
    messageObj["priority"]    = QString::number(task.priority);
    messageObj["taskName"]    = task.taskName;
    messageObj["description"] = des;
    messageObj["due_date"]    = task.due_date;
    messageObj["created_by"]  = task.created_by;
    messageObj["assigned_to"] = task.assigned_to;
    QJsonDocument doc(messageObj);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
    jsonData += '\n';
    this->write(jsonData);
}
void ApiClient::getAllTasks(){

    QJsonObject messageObj;
    messageObj["type"] = "getAllTasks";
    messageObj["time"] =  QTime::currentTime().toString("hh:mm:ss");
    QJsonDocument doc(messageObj);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
    jsonData += '\n';
    this->write(jsonData);
}

void ApiClient::updateTaskStatus(const int taskID, const int newStatus){
    QJsonObject messageObj;
    messageObj["type"] = "updateTasks";
    messageObj["taskID"] = taskID;
    messageObj["status"] = newStatus;
    messageObj["time"] =  QTime::currentTime().toString("hh:mm:ss");
    QJsonDocument doc(messageObj);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
    jsonData += '\n';
    this->write(jsonData);
}

void ApiClient::createUser(const User& user){
    QJsonObject messageObj;
    QDateTime   createdAt;
    messageObj["type"] = "createUser";
    messageObj["id"]         = user.id;
    messageObj["createdAt"]  = createdAt.toString(Qt::ISODate);
    messageObj["name"]       = user.name;
    messageObj["surname"]    = user.surname;
    messageObj["jobTitle"]   = user.jobTitle;
    messageObj["password"]   = user.passwordHash;
    messageObj["login"]      = user.loginHash;
    messageObj["isBanned"]   = user.isBanned;
    QJsonDocument doc(messageObj);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
    jsonData += '\n';
    this->write(jsonData);
}
void ApiClient::banUser(const int userID){
    QJsonObject messageObj;
    messageObj["type"] = "banUser";
    messageObj["id"] = userID;
    messageObj["time"] =  QTime::currentTime().toString("hh:mm:ss");
    QJsonDocument doc(messageObj);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
    jsonData += '\n';
    this->write(jsonData);
}
void ApiClient::unBanUser(const int userID){
    QJsonObject messageObj;
    messageObj["type"] = "unBanUser";
    messageObj["id"] = userID;
    messageObj["time"] =  QTime::currentTime().toString("hh:mm:ss");
    QJsonDocument doc(messageObj);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
    jsonData += '\n';
    this->write(jsonData);
}
void ApiClient::getAllUsers(){
    QJsonObject messageObj;
    messageObj["type"] = "getAllUsers";
    messageObj["time"] =  QTime::currentTime().toString("hh:mm:ss");
    QJsonDocument doc(messageObj);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
    jsonData += '\n';
    this->write(jsonData);
}
void ApiClient::getDescription(const qint64 TaskID){
    QJsonObject messageObj;
    messageObj["type"] = "getDescription";
    messageObj["id"] = TaskID;
    messageObj["time"] =  QTime::currentTime().toString("hh:mm:ss");
    QJsonDocument doc(messageObj);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
    jsonData += '\n';
    this->write(jsonData);
}
void ApiClient::updateTaskDescription(const qint64 id, const QString& descriptionJson ){
    QJsonObject messageObj;
    messageObj["type"] = "updateTaskDescription";
    messageObj["id"] = id;
    messageObj["description"] = descriptionJson;
    messageObj["time"] =  QTime::currentTime().toString("hh:mm:ss");
    QJsonDocument doc(messageObj);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
    jsonData += '\n';
    this->write(jsonData);
}
void ApiClient::saveTask(const qint64 id,const QString& json ){
    QJsonObject messageObj;
    messageObj["type"] = "saveTask";
    messageObj["id"] = id;
    messageObj["description"] = json;
    messageObj["time"] =  QTime::currentTime().toString("hh:mm:ss");
    QJsonDocument doc(messageObj);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
    jsonData += '\n';
    this->write(jsonData);
}

