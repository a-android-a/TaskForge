#include "SslServer.h"
#include "UsersDatabaseManager.h"
#include "TaskDatabaseManager.h"
#include "Task.h"

#include <QVector>
#include <QFile>
#include <QSslKey>
#include <QSslCertificate>
#include <QSslSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>
#include <QSqlQuery>
#include <QCryptographicHash>
#include <QRandomGenerator>
#include <QByteArray>
#include <QList>

SslServer::SslServer(QObject *parent):QTcpServer(parent)
{

    usersDB = UsersDatabaseManager("DB/Users.db");
    tasksDB = TaskDatabaseManager("DB/tasks.db");
    usersDB.open();
    tasksDB.open();
    if(!usersDB.isOpen()){
        qWarning() << "Failed to open Users database " << usersDB.lastErrorText();
    }
    if(!tasksDB.isOpen()){
        qWarning() << "Failed to open Tasks database " << tasksDB.lastErrorText();
    }



}

SslServer::~SslServer()
{
    for (auto *client : m_clients) {
        client->disconnectFromHost();
    }
}

void SslServer::incomingConnection(qintptr socketDescriptor)
{
    QSslSocket *sslSocket = new QSslSocket(this);


    QFile keyFile("server.key");
    if (!keyFile.open(QIODevice::ReadOnly)) {
        qWarning() << "Mistake: the server.key was not found in the current folder!";
        delete sslSocket;
        return;
    }
    QSslKey key(keyFile.readAll(), QSsl::Rsa, QSsl::Pem, QSsl::PrivateKey);
    if (key.isNull()) {
        qWarning() << "Error: invalid or encrypted private key!";
        delete sslSocket;
        return;
    }
    sslSocket->setPrivateKey(key);

    QFile certFile("server.crt");
    if (!certFile.open(QIODevice::ReadOnly)) {
        qWarning() << "Mistake: the server.crt was not found in the current folder!";
        delete sslSocket;
        return;
    }
    QSslCertificate cert(certFile.readAll());
    if (cert.isNull()) {
        qWarning() << "Error: Invalid certificate!";
        delete sslSocket;
        return;
    }
    sslSocket->setLocalCertificate(cert);


    if (!sslSocket->setSocketDescriptor(socketDescriptor)) {
        qWarning() << "setSocketDescriptor failed:" << sslSocket->errorString();
        delete sslSocket;
        return;
    }


    connect(sslSocket, &QSslSocket::encrypted, this, &SslServer::onEncrypted);
    connect(sslSocket, &QSslSocket::readyRead, this, &SslServer::onReadyRead);
    connect(sslSocket, &QSslSocket::disconnected, this, &SslServer::onDisconnected);
    connect(sslSocket, QOverload<const QList<QSslError>&>::of(&QSslSocket::sslErrors),
            this, &SslServer::onSslErrors);


    sslSocket->startServerEncryption();

    qDebug() << "New connection: waiting for SSL handshake...";
}

void SslServer::onEncrypted()
{
    QSslSocket *socket = qobject_cast<QSslSocket*>(sender());
    if (!socket) return;

    qDebug().nospace() << "SSL handshake is successful! The client is connected:"
                       << socket->peerAddress().toString() << ":" << socket->peerPort();

    m_clients.append(socket);
    socket->write("Hi");
}

void SslServer::onReadyRead()
{
    QSslSocket *socket = qobject_cast<QSslSocket*>(sender());
    if (!socket) return;

    while (socket->canReadLine()) {
        QByteArray data = socket->readLine();
        if (data.isEmpty()) continue;

        QString message = QString::fromUtf8(data).trimmed();
        qInfo()<< "Message from" << socket->peerAddress().toString() << ":" << message;

        QJsonParseError parseError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(message.toUtf8(), &parseError);

        if (parseError.error != QJsonParseError::NoError) {
            qWarning() << "JSON parse error:" << parseError.errorString();
            return;
        }


        if (!jsonDoc.isObject()) {
            qWarning() << "JSON is not an object";
            return;
        }

        QJsonObject jsonObj = jsonDoc.object();


        QString type = jsonObj.value("type").toString();

        QByteArray result;

        if (type == "authorization") {

            int statusCode = -1;

            QString login    = jsonObj.value("login").toString();
            QString password = jsonObj.value("password").toString();

            qInfo()<<"authorization";
            QJsonObject response;
            response["type"] = "authorization_response";
            User u = authenticate(password,login,statusCode);

            if(statusCode == 1 && u.isBanned == 0){

                qInfo()<<"status:ok";
                response["status"]     = "ok";
                response["userId"]     = u.id;
                response["name"]       = u.name;
                response["surname"]    = u.surname;
                response["job_title"]  = u.jobTitle;
                response["timestamp"]  = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
            }else  {
                qInfo()<<"status:error";
                response["status"]  = "error";
                response["message"] = "Authentication failed";
            }
            QJsonDocument doc(response);
            QByteArray result = doc.toJson(QJsonDocument::Compact);
            result.append('\n');

            socket->write(result);
            qInfo()<<result;
        } else if (type == "getAllTasks") {
            QVector<Task> tasks = tasksDB.getAllTasks();

            QJsonArray tasksArray;
            for (const Task &t : tasks) {
                QJsonObject taskObj;
                taskObj["id"]          = t.id;
                taskObj["status"]      = t.status;
                taskObj["priority"]    = t.priority;
                taskObj["taskName"]    = t.taskName;
                taskObj["description"] = t.description;
                taskObj["due_date"]    = t.due_date;
                taskObj["created_by"]  = t.created_by;
                taskObj["assigned_to"] = t.assigned_to;

                tasksArray.append(taskObj);
            }

            QJsonObject response;
            response["type"]   = "allTasks_response";
            response["status"] = "ok";
            response["tasks"]  = tasksArray;

            QJsonDocument doc(response);
            QByteArray result = doc.toJson(QJsonDocument::Compact);
            result.append('\n');

            socket->write(result);
            socket->flush();

            qInfo() << "Tasks sent:" << tasks.size();
        }
        else if(type == "updateTasks"){
            long long  id     = jsonObj.value("taskID").toInt();
            long long status = jsonObj.value("status").toInt();
            tasksDB.updateStatus(id,status);
            qInfo() << "ID: " <<id <<" status: "<< status;
            Task t = tasksDB.getTaskById(id);
            QJsonObject taskObj;
            taskObj["id"]          = t.id;
            taskObj["status"]      = t.status;
            taskObj["priority"]    = t.priority;
            taskObj["taskName"]    = t.taskName;
            taskObj["description"] = t.description;
            taskObj["due_date"]    = t.due_date;
            taskObj["created_by"]  = t.created_by;
            taskObj["assigned_to"] = t.assigned_to;
            taskObj["type"] = "updateTasks_response";
            QJsonDocument doc(taskObj);
            QByteArray result = doc.toJson(QJsonDocument::Compact);
            result.append('\n');
            // socket->write(result);
            for (QTcpSocket* client : m_clients) {
                client->write(result);
            }
        }
        else if(type == "createTask"){
            Task t;

            t.id          = jsonObj["id"].toString().toLongLong();
            t.status      = jsonObj["status"].toString().toLongLong();
            t.priority    = jsonObj["priority"].toString().toLongLong();
            t.taskName    = jsonObj["taskName"].toString();
            t.description = jsonObj["description"].toString();
            t.due_date    = jsonObj["due_date"].toString();
            t.created_by  = jsonObj["created_by"].toString();
            t.assigned_to = jsonObj["assigned_to"].toString();

            tasksDB.createTask(t);

        } else if(type == "createUser"){
            qInfo() << "createUser";

            // jsonObj.value("type").toString()
            QString name      = jsonObj["name"].toString();
            QString surname   = jsonObj["surname"].toString();
            QString jobTitle  = jsonObj["jobTitle"].toString();
            QString login     = jsonObj["login"].toString();
            QString password  = jsonObj["password"].toString();
            qInfo() << "Login"<< login<<'\t'<<"pass "<<password;
            bool ok = usersDB.createUser(name, surname, jobTitle, login, password);


            QJsonObject messageObj;
            messageObj["type"] = "createUser_response";

            if (ok){

                messageObj["status"] = "ok";
            }
            else{
                qInfo() << "Failed to create user";
                messageObj["status"] = "error";
            }
            QJsonDocument doc(messageObj);
            QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
            jsonData += '\n';
            socket->write(jsonData);
        }
        else if(type == "getAllUsers"){
            qInfo() << "getAllUsers";
            QList<User> users = usersDB.getAllUsers();

            QJsonArray usersArray;
            for (const User &u : users) {
                QJsonObject userObj;
                userObj["userId"]     = u.id;
                userObj["name"]       = u.name;
                userObj["surname"]    = u.surname;
                userObj["job_title"]  = u.jobTitle;
                userObj["is_Banned"]   = u.isBanned;

                usersArray.append(userObj);
            }

            QJsonObject response;
            response["type"]   = "getAllUsers_response";
            response["status"] = "ok";
            response["users"]  = usersArray;

            QJsonDocument doc(response);
            QByteArray result = doc.toJson(QJsonDocument::Compact);
            result.append('\n');
            qInfo()<<doc;
            socket->write(result);
        }


    }
}


QByteArray SslServer::generateSalt(int bytes )
{
    QByteArray salt(bytes, Qt::Uninitialized);

    auto *rng = QRandomGenerator::global();

    for (int i = 0; i < bytes; ++i) {
        salt[i] = static_cast<char>(rng->bounded(256));
    }

    return salt;
}


QString SslServer::hashPasswordPBKDF2(const QString &password, const QByteArray &salt)
{
    QCryptographicHash hash(QCryptographicHash::Sha256);
    QByteArray data = password.toUtf8() + salt;

    for (int i = 0; i < 600000; ++i) {
        hash.reset();
        hash.addData(data);
        data = hash.result();
    }

    return QString::fromLatin1(data.toHex());
}
QString SslServer::toSha256(const QString &str){
    QString resultStr;
    QByteArray data = str.toUtf8();

    QCryptographicHash hash(QCryptographicHash::Sha256);
    hash.addData(data);
    QByteArray result = hash.result();
    resultStr  =  QString::fromLatin1(result.toHex());
    return resultStr;
}


User SslServer::authenticate(const QString &passwd, const QString &login, int &statusCode)
{
    statusCode = 0;

    // 1. Хешируем логин
    QString loginHash = toSha256(login);

    // 2. Ищем пользователя
    User u = usersDB.getUserByLoginHash(loginHash);


    // 3. Достаём соль
    QByteArray salt = QByteArray::fromBase64(u.password_salt.toUtf8());

    // 4. SHA256 введённого пароля
    QString passwordSha256 = toSha256(passwd);

    // 5. PBKDF2(SHA256(password), salt)
    QString passwordHashInput = hashPasswordPBKDF2(passwordSha256, salt);
    qInfo()<<passwordHashInput;
    qInfo()<< u.passwordHash;
    // 6. Сравниваем с тем, что в базе
    if (passwordHashInput == u.passwordHash) {
        statusCode = 1; // успех
    } else {
        statusCode = 0; // неверный пароль
    }

    return u;
}


void SslServer::onDisconnected()
{
    QSslSocket *socket = qobject_cast<QSslSocket*>(sender());
    if (!socket) return;

    qInfo() << "The client has disconnected:" << socket->peerAddress().toString();
    m_clients.removeOne(socket);
    socket->deleteLater();
}

void SslServer::onSslErrors(const QList<QSslError> &errors)
{
    QSslSocket *socket = qobject_cast<QSslSocket*>(sender());
    if (!socket) return;

    for (const QSslError &error : errors) {
        qWarning() << "SSL error:" << error.errorString();
    }


    //socket->ignoreSslErrors();
}
