#include "mainwindow.h"
#include "UserAuthWindow.h"
#include "network/ApiClient.h"
#include "Config.h"
#include <QApplication>
#include <QString>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ApiClient* apiClient = new ApiClient(&a);



    UserAuthWindow auth(apiClient, nullptr);
    auth.show();
    auth.setStyle(Config::instance().getStylePath());
    return a.exec();
}
