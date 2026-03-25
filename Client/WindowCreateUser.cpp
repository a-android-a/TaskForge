#include "WindowCreateUser.h"

#include <QHBoxLayout>
#include <QMessageBox>
#include <QRandomGenerator>
#include <QString>

WindowCreateUser::WindowCreateUser(QWidget *parent)
    : QDialog(parent)
{
    setObjectName("createUserWindow");
    setupUI();
}

void WindowCreateUser::setupUI()
{
    setWindowTitle(tr("Creating a user"));
    setFixedSize(400, 400);

    QVBoxLayout *layout = new QVBoxLayout(this);

    QLabel *title = new QLabel(tr("Creating a new user"), this);
    title->setObjectName("createUserTitle");
    title->setAlignment(Qt::AlignCenter);
    layout->addWidget(title);

    m_editName     = new QLineEdit(this);
    m_editSurname  = new QLineEdit(this);
    m_editJobTitle = new QLineEdit(this);
    m_editLogin    = new QLineEdit(this);
    m_editPassword = new QLineEdit(this);

    //m_editPassword->setEchoMode(QLineEdit::Password);

    m_editName->setPlaceholderText(tr("name"));
    m_editSurname->setPlaceholderText(tr("surname"));
    m_editJobTitle->setPlaceholderText(tr("job title"));
    m_editLogin->setPlaceholderText(tr("login"));
    m_editPassword->setPlaceholderText(tr("passwor"));

    layout->addWidget(m_editName);
    layout->addWidget(m_editSurname);
    layout->addWidget(m_editJobTitle);
    layout->addWidget(m_editLogin);
    layout->addWidget(m_editPassword);

    // Кнопки
    m_btnCreate = new QPushButton(tr("create"), this);
    m_btnCreate->setObjectName("btnCreateUser");

    m_btnCancel = new QPushButton(tr("Cancel"), this);
    m_btnCancel->setObjectName("btnCancelUser");

    m_pGenerateLoginPassButton = new QPushButton(tr("generate login password"), this);
    m_pGenerateLoginPassButton->setObjectName("btnGenerateLoginPass");

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addWidget(m_btnCreate);
    btnLayout->addWidget(m_btnCancel);
    layout->addWidget(m_pGenerateLoginPassButton);
    layout->addLayout(btnLayout);

    connect(m_btnCreate,                &QPushButton::clicked, this, &WindowCreateUser::onCreateClicked             );
    connect(m_pGenerateLoginPassButton, &QPushButton::clicked, this, &WindowCreateUser::onGenerateCredentialsClicked);
    connect(m_btnCancel,                &QPushButton::clicked, this, &WindowCreateUser::reject                      );
}

void WindowCreateUser::onCreateClicked()
{
    if (m_editName->text().isEmpty() ||
        m_editSurname->text().isEmpty() ||
        m_editLogin->text().isEmpty() ||
        m_editPassword->text().isEmpty())
    {
        QMessageBox::warning(this, tr("Error"), tr("Fill in all required fields!"));
        return;
    }

    User user;
    user.id = -1; // сервер назначит
    user.createdAt = QDateTime::currentDateTime();
    user.name = m_editName->text();
    user.surname = m_editSurname->text();
    user.jobTitle = m_editJobTitle->text();
    user.loginHash = m_editLogin->text();     // хешируем позже
    user.passwordHash = m_editPassword->text();
    user.password_salt = "";                  // соль добавит сервер
    user.isBanned = false;

    emit userCreated(user);


}
void WindowCreateUser::onGenerateCredentialsClicked(){
    const QString chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789"
        "!@#$%&*";

    QString pass,login;
    int length = 16;
    pass.reserve(length); // 16 длина символов
    login.reserve(length);
    for (int i = 0; i < length; i++)
    {
        int index = QRandomGenerator::global()->bounded(chars.size());
        pass.append(chars[index]);
    }
    for (int i = 0; i < length; i++)
    {
        int index = QRandomGenerator::global()->bounded(chars.size());
        login.append(chars[index]);
    }
    m_editPassword->setText(pass);
    m_editLogin->setText(login);
}
