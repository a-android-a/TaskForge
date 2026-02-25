#include <QApplication>
#include "SslServer.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SslServer server;
    const int port = 4433;

    if (!server.listen(QHostAddress::Any, port)) {
        qInfo() << "Error Start Server " << server.errorString();
        return 1;
    }
    qInfo() << "Server Start" ;
   return a.exec();
}
