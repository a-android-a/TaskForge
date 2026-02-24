#include <QApplication>
#include "SslServer.h"
int main(int argc, char *argv[])
{
    SslServer server;
    const int port = 4433;

    if (!server.listen(QHostAddress::Any, port)) {
        qInfo() << "Error Start Server " << server.errorString();
        return 1;
    }

    return 0;
}
