#include "mainwindow.h"
#include "UserAuthWindow.h"
#include "network/ApiClient.h"
#include "Config.h"
#include <QApplication>
#include <QString>
#include <QTranslator>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ApiClient* apiClient = new ApiClient(&a);
    Config::instance().load();
    QString lang = Config::instance().getLanguage(); // "ru" или "en"

    static QTranslator translator;

    QString path = QCoreApplication::applicationDirPath()
                   + "/translations/app_" + lang + ".qm";


    if (translator.load(path)) {
        a.installTranslator(&translator);
    } else {
        qWarning() << "Не удалось загрузить перевод:" << path;
    }



    UserAuthWindow auth(apiClient, nullptr);
    auth.show();
    auth.setStyle(Config::instance().getStylePath());
    return a.exec();
}
