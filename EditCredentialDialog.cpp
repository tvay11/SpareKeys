// EditCredentialDialog.cpp

#include "EditCredentialDialog.h"

EditCredentialDialog::EditCredentialDialog(QWidget *parent, const QString &email, const QString &password)
        : QDialog(parent)
{
    emailLineEdit = new QLineEdit(email);
    passwordLineEdit = new QLineEdit(password);

    saveButton = new QPushButton("Save");
    cancelButton = new QPushButton("Cancel");

    QFormLayout *layout = new QFormLayout;
    layout->addRow("Email:", emailLineEdit);
    layout->addRow("Password:", passwordLineEdit);
    layout->addWidget(saveButton);
    layout->addWidget(cancelButton);

    setLayout(layout);

    connect(saveButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

QString EditCredentialDialog::getEmail()
{
    return emailLineEdit->text();
}

QString EditCredentialDialog::getPassword()
{
    return passwordLineEdit->text();
}
