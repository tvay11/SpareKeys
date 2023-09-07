// EditCredentialDialog.h

#ifndef EDITCREDENTIALDIALOG_H
#define EDITCREDENTIALDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QFormLayout>

class EditCredentialDialog : public QDialog
{
Q_OBJECT
public:
    explicit EditCredentialDialog(QWidget *parent = nullptr, const QString &email = "", const QString &password = "");

    QString getEmail();
    QString getPassword();

private:
    QLineEdit *emailLineEdit;
    QLineEdit *passwordLineEdit;
    QPushButton *saveButton;
    QPushButton *cancelButton;
};

#endif // EDITCREDENTIALDIALOG_H
