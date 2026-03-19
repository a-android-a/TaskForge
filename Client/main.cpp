#include "mainwindow.h"
#include "UserAuthWindow.h"
#include "network/ApiClient.h"
#include <QApplication>
#include <QString>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ApiClient* apiClient = new ApiClient(&a);
    QString host = "127.0.0.1";


    UserAuthWindow auth(apiClient, nullptr);
    auth.show();
    auth.setStyle("style/stylesAuth.qss");
    return a.exec();
}
