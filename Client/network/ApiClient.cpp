#include "ApiClient.h"
#include "../Config.h"

ApiClient::ApiClient(QObject* parent): QSslSocket(parent)
{

}
void ApiClient::authenticate(const QString &login, const QString password){

}
