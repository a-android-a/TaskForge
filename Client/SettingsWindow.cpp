#include "SettingsWindow.h"
#include "Config.h"

#include <QVBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QCoreApplication>
#include <QMessageBox>

SettingsWindow::SettingsWindow(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle(tr("Settings"));
    setObjectName("SettingsWindow");

    resize(400, 300);

    auto *mainLayout = new QVBoxLayout(this);
    auto *form = new QFormLayout();

    hostEdit = new QLineEdit(this);
    portEdit = new QLineEdit(this);
    certEdit = new QLineEdit(this);

    styleEdit = new QLineEdit(this);
    styleEdit->setObjectName("SettingsStylePath");

    chooseStyleBtn = new QPushButton("Выбрать файл стиля", this);
    chooseStyleBtn->setObjectName("SettingsChooseStyleButton");

    themeBox = new QComboBox(this);
    themeBox->addItems({"light", "dark"});

    langBox = new QComboBox(this);
    langBox->addItems({"en", "ru"});

    userEdit = new QLineEdit(this);

    form->addRow("Server host:", hostEdit);
    form->addRow("Server port:", portEdit);
    form->addRow("Certificate:", certEdit);
    form->addRow("Theme:", themeBox);
    form->addRow("Language:", langBox);
    form->addRow("Default user:", userEdit);
    form->addRow("Style file:", styleEdit);
    form->addRow("", chooseStyleBtn);


    hostEdit->setObjectName("SettingsHost");
    portEdit->setObjectName("SettingsPort");
    themeBox->setObjectName("SettingsTheme");
    langBox->setObjectName("SettingsLanguage");
    userEdit->setObjectName("SettingsUser");


    connect(chooseStyleBtn, &QPushButton::clicked, this, [this]() {
        QString file = QFileDialog::getOpenFileName(
            this,
            tr("Выберите файл стилей"),
            QCoreApplication::applicationDirPath(),
            tr("QSS Files (*)")
            );

        if (file.isEmpty())
            return;

        // Проверяем существование файла
        if (!QFile::exists(file)) {
            QMessageBox::warning(this, tr("Ошибка"), tr("Файл не существует."));
            return;
        }

        // Проверяем возможность чтения
        QFile f(file);
        if (!f.open(QFile::ReadOnly)) {
            QMessageBox::warning(this, tr("Ошибка"), tr("Не удалось открыть файл стилей."));
            return;
        }
        f.close();

        // Проверяем расширение
        if (!file.endsWith(".qss", Qt::CaseInsensitive)) {
            QMessageBox::warning(this, tr("Ошибка"), tr("Выберите файл с расширением .qss"));
            return;
        }

        // Если всё ок — записываем путь
        styleEdit->setText(file);
    });




    mainLayout->addLayout(form);

    saveBtn = new QPushButton("Сохранить", this);
    mainLayout->addWidget(saveBtn);
    saveBtn->setObjectName("SettingsSaveButton");

    connect(saveBtn, &QPushButton::clicked, this, &SettingsWindow::onSaveClicked);

    loadFromConfig();
}

void SettingsWindow::loadFromConfig()
{
    auto &cfg = Config::instance();

    hostEdit->setText(cfg.getServerHost());
    portEdit->setText(QString::number(cfg.getServerPort()));
    certEdit->setText(cfg.getCertificate());

    themeBox->setCurrentText(cfg.getTheme());
    langBox->setCurrentText(cfg.getLanguage());
    userEdit->setText(cfg.getDefaultUserName());
    styleEdit->setText(cfg.getStylePath());
}

void SettingsWindow::onSaveClicked()
{
    auto &cfg = Config::instance();

    cfg.setServerHost(hostEdit->text());
    cfg.setServerPort(portEdit->text().toInt());
    cfg.setCertificate(certEdit->text());
    cfg.setTheme(themeBox->currentText());
    cfg.setLanguage(langBox->currentText());
    cfg.setDefaultUserName(userEdit->text());
    cfg.setStylePath(styleEdit->text());

    cfg.save();
    QMessageBox::information(this, tr("Information"),tr("For the settings to take effect, you need to restart the application."));
    qInfo() << "Settings saved!";
}

bool SettingsWindow::setStyle(const QString &styleFileName){

    QFile file(styleFileName);
    if(file.open(QFile::ReadOnly)){
        QString strCSS = QLatin1String(file.readAll());
        this->setStyleSheet(strCSS);
        file.close();
        return true;
    }
    return false;
}
