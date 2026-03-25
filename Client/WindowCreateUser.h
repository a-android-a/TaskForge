#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QDateTime>

#include "../Server/User.h"

class WindowCreateUser : public QDialog
{
    Q_OBJECT

public:
    explicit WindowCreateUser(QWidget *parent = nullptr);


signals:
    void userCreated(const User& user);

private slots:
    void onCreateClicked();
    void onGenerateCredentialsClicked();
private:
    QLineEdit *m_editName;
    QLineEdit *m_editSurname;
    QLineEdit *m_editJobTitle;
    QLineEdit *m_editLogin;
    QLineEdit *m_editPassword;

    QPushButton *m_btnCreate;
    QPushButton *m_btnCancel;

    QPushButton *m_pGenerateLoginPassButton;

    void setupUI();
};


