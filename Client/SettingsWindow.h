#pragma once
#include <QWidget>
#include <QString>
class QLineEdit;
class QComboBox;
class QPushButton;

class SettingsWindow : public QWidget {
    Q_OBJECT

private:
    QLineEdit *hostEdit;
    QLineEdit *portEdit;
    QLineEdit *certEdit;
    QComboBox *themeBox;
    QComboBox *langBox;
    QLineEdit *userEdit;

    QPushButton *saveBtn;

    void loadFromConfig();
public:
    explicit SettingsWindow(QWidget *parent = nullptr);
    bool setStyle(const QString &styleFileName);

private slots:
    void onSaveClicked();

};
