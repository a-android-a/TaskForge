#include "mainwindow.h"
#include "UserAuthWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    UserAuthWindow auth(nullptr);
    auth.show();
    return a.exec();
}
