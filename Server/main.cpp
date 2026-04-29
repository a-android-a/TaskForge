#include <QApplication>
#include "SslServer.h"
#include "../Client/Config.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SslServer server;
    Config::instance().load();
    const int port = Config::instance().getServerPort();

    if (!server.listen(QHostAddress::Any, port)) {
        qInfo() << "Error Start Server " << server.errorString();
        return 1;
    }
    qInfo() << "Server Start" ;
   return a.exec();
}
