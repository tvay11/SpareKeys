#include "mainwindow.h"
#include <QVBoxLayout>
#include <QTextStream>
#include <QStandardItemModel>
#include <QDialog>
#include <QLineEdit>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QFile>
#include <QJsonObject>
#include <QtCore/qjsonarray.h>
#include <QtCore/qjsondocument.h>
#include <iostream>
#include <QDir>
#include "EditCredentialDialog.h"
#include <QApplication>
#include <QClipboard>
#include <QSpinbox>
#include <QRandomGenerator>
#include <QCheckBox>

using namespace std;

// Initializes the main application window with various UI elements such as buttons, list views,
// and labels. It also sets up the initial connections for signals and slots.

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent),
          webInfoMap(),
          webAddressListView(new QListView(this)),
          infoListView(new QListView(this)),
          infoLabel(new QLabel("         Spare Keys", this)),
          addWebAddressButton(new QPushButton("Add Website", this)),
          deleteAddressButton(new QPushButton("Delete Website", this)),
          addInfoButton(new QPushButton("Add Credential", this)),
          webAddressModel(new QStringListModel(this)),
          infoModel(new QStandardItemModel(this))
{
    readFromJsonFile();

    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    centralWidget->setStyleSheet("QWidget {"
                                 "border: 2px solid #555;"
                                 "border-radius: 10px;"
                                 "}");

    this->resize(1200, 900);

    QFont buttonFont = addWebAddressButton->font();
    buttonFont.setPointSize(24);
    addWebAddressButton->setFont(buttonFont);
    addInfoButton->setFont(buttonFont);
    deleteAddressButton->setFont(buttonFont);

    addWebAddressButton->setMaximumWidth(400);
    addInfoButton->setMaximumWidth(400);
    infoLabel->setStyleSheet("border: none;");

    QFont font = webAddressListView->font();
    font.setPointSize(24);
    webAddressListView->setFont(font);

    font = infoListView->font();
    font.setPointSize(24);
    infoListView->setFont(font);

    font = infoLabel->font();
    font.setPointSize(24);
    infoLabel->setFont(font);

    webAddressListView->setModel(webAddressModel);
    infoListView->setModel(infoModel);

    connect(addWebAddressButton, &QPushButton::clicked, this, &MainWindow::addWebAddress);
    connect(addInfoButton, &QPushButton::clicked, this, &MainWindow::addInfo);
    connect(webAddressListView->selectionModel(), &QItemSelectionModel::currentChanged, this, &MainWindow::onWebAddressSelectionChanged);
    connect(deleteAddressButton, &QPushButton::clicked, this, &MainWindow::deleteWebAddress);

    QHBoxLayout *leftButtonLayout = new QHBoxLayout();
    leftButtonLayout->addWidget(addWebAddressButton);
    leftButtonLayout->addWidget(deleteAddressButton);
    leftButtonLayout->addStretch(1);
    leftButtonLayout->addWidget(addWebAddressButton);
    leftButtonLayout->addStretch(1);

    QVBoxLayout *leftLayout = new QVBoxLayout();
    leftLayout->addLayout(leftButtonLayout);
    leftLayout->addWidget(webAddressListView, 1);

    QHBoxLayout *rightButtonLayout = new QHBoxLayout();
    rightButtonLayout->addWidget(infoLabel);
    rightButtonLayout->addStretch(1);
    rightButtonLayout->addWidget(addInfoButton);

    QVBoxLayout *rightLayout = new QVBoxLayout();
    rightLayout->addLayout(rightButtonLayout);
    rightLayout->addWidget(infoListView, 1);

    QHBoxLayout *listLayout = new QHBoxLayout();
    listLayout->addLayout(leftLayout, 35);
    listLayout->addLayout(rightLayout, 65);
    mainLayout->addLayout(listLayout, 1);

    setCentralWidget(centralWidget);
}

// The destructor ensures that all the saved data is written to a JSON file before the MainWindow object is destroyed.
MainWindow::~MainWindow()
{
    writeToJsonFile();
}

//*This function is triggered when the user wants to delete a web address.
// It removes the selected web address from the map and updates the model.

void MainWindow::deleteWebAddress()
{
    QModelIndex index = webAddressListView->currentIndex();
    if(!index.isValid()) {
        return;
    }

    QString webAddress = index.data(Qt::DisplayRole).toString();
    webInfoMap.remove(webAddress);
    webAddressModel->removeRow(index.row());
    writeToJsonFile();
}

// This function opens a dialog for the user to input a new web address.
// If the dialog is accepted and the input is valid, the web address is added to the map and the model is updated.

void MainWindow::addWebAddress()
{
    QDialog customDialog(this);
    customDialog.setWindowTitle("Add Web Address");
    customDialog.resize(600, 200);

    QVBoxLayout *mainLayout = new QVBoxLayout;

    QLabel *label = new QLabel("Web Address", &customDialog);
    QFont labelFont = label->font();
    labelFont.setPointSize(24);
    label->setFont(labelFont);

    QLineEdit *lineEdit = new QLineEdit(&customDialog);
    QFont lineEditFont = lineEdit->font();
    lineEditFont.setPointSize(24);
    lineEdit->setFont(lineEditFont);
    lineEdit->setStyleSheet("QLineEdit { background-color: #555; color: #FFF; border: 2px solid #FFF; border-radius: 10px; }");

    QPushButton *cancelButton = new QPushButton("Cancel", &customDialog);
    QPushButton *okButton = new QPushButton("Add", &customDialog);
    cancelButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    okButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(okButton);

    mainLayout->addWidget(label);
    mainLayout->addWidget(lineEdit);
    mainLayout->addLayout(buttonLayout);
    customDialog.setLayout(mainLayout);
    customDialog.setStyleSheet("QDialog { background-color: #333; }" "QLabel { color: #FFF; }");

    QRect parentRect = this->geometry();
    QSize dialogSize = customDialog.sizeHint();
    int xPos = parentRect.left() + (parentRect.width() - dialogSize.width()) / 2;
    int yPos = parentRect.top() + (parentRect.height() - dialogSize.height()) / 2;
    customDialog.move(xPos, yPos);

    connect(okButton, &QPushButton::clicked, &customDialog, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, &customDialog, &QDialog::reject);

    bool ok = (customDialog.exec() == QDialog::Accepted);
    if (ok) {
        QString webAddress = lineEdit->text();
        if (!webAddress.isEmpty()) {
            webInfoMap[webAddress] = QList<Credential>();
            webAddressModel->setStringList(webInfoMap.keys());
        }
    }
}

// This function creates a random password using uppercase letters, lowercase letters, digits, and special characters.
//
// @param length The length of the password to be generated.
// @return Returns the generated random password as a QString.


QString generateRandomPassword(int length)
{
    const QString possibleCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#$%^&*()-=_+");
    QString randomString;

    for(int i = 0; i < length; ++i) {
        int index = QRandomGenerator::global()->bounded(possibleCharacters.length());
        QChar nextChar = possibleCharacters.at(index);
        randomString.append(nextChar);
    }

    return randomString;
}

/*
* This function creates a random password using only uppercase letters, lowercase letters, and digits.
*
* @param length The length of the password to be generated.
* @return Returns the generated random password as a QString.
*/

QString generateSimpleRandomPassword(int length)
{
    const QString possibleCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
    QString randomString;
    for (int i = 0; i < length; ++i) {
        int index = QRandomGenerator::global()->bounded(possibleCharacters.length());
        QChar nextChar = possibleCharacters.at(index);
        randomString.append(nextChar);
    }
    return randomString;
}

// This function opens a QDialog that allows the user to input an email and password for a selected website
// from the webAddressListView. Users also have the option to generate a random password.
//
// @note This function should only be called when a valid web address is selected.

void MainWindow::addInfo()
{
    QModelIndex index = webAddressListView->currentIndex();
    if (!index.isValid()) return;

    QString webAddress = index.data().toString();

    QDialog dialog(this);
    dialog.setWindowTitle("Add Email and Password");
    dialog.resize(400, 300);

    QFont dialogFont = dialog.font();
    dialogFont.setPointSize(24);
    dialog.setFont(dialogFont);

    QVBoxLayout *mainLayout = new QVBoxLayout;

    QFormLayout form;
    QLineEdit *emailLineEdit = new QLineEdit(&dialog);
    QLineEdit *passwordLineEdit = new QLineEdit(&dialog);
    passwordLineEdit->setEchoMode(QLineEdit::Password);

    form.addRow("Email:", emailLineEdit);
    form.addRow("Password:", passwordLineEdit);

    emailLineEdit->setFixedSize(300, 50);
    passwordLineEdit->setFixedSize(300, 50);

    QSpinBox *passwordLengthSpinBox = new QSpinBox(&dialog);
    passwordLengthSpinBox->setRange(8, 128);
    passwordLengthSpinBox->setValue(12);
    form.addRow("Password Length:", passwordLengthSpinBox);

    QPushButton *generatePasswordButton = new QPushButton("Generate Password", &dialog);
    form.addRow(generatePasswordButton);
    QCheckBox *simplePasswordCheckBox = new QCheckBox("Simple Password", &dialog);
    form.addRow(simplePasswordCheckBox);


    connect(generatePasswordButton, &QPushButton::clicked, [passwordLineEdit, passwordLengthSpinBox, simplePasswordCheckBox]() {
        QString randomPassword;
        if (simplePasswordCheckBox->isChecked()) {
            randomPassword = generateSimpleRandomPassword(passwordLengthSpinBox->value());
        } else {
            randomPassword = generateRandomPassword(passwordLengthSpinBox->value());
        }
        passwordLineEdit->setText(randomPassword);
    });
    mainLayout->addLayout(&form);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    QPushButton *okButton = buttonBox.button(QDialogButtonBox::Ok);
    QPushButton *cancelButton = buttonBox.button(QDialogButtonBox::Cancel);

    QFont buttonFont = okButton->font();
    buttonFont.setPointSize(18);
    okButton->setFont(buttonFont);
    cancelButton->setFont(buttonFont);

    mainLayout->addWidget(&buttonBox);

    dialog.setLayout(mainLayout);

    connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        QString email = emailLineEdit->text();
        QString password = passwordLineEdit->text();

        if (!email.isEmpty() && !password.isEmpty()) {
            Credential info = {email, password};
            webInfoMap[webAddress].append(info);
            refreshInfoList(webAddress);
        }
    }
}

void MainWindow::onWebAddressSelectionChanged(const QModelIndex &current, const QModelIndex &previous)
{
    if (current.isValid()) {
        QString webAddress = current.data().toString();
        refreshInfoList(webAddress);
    }
}

void MainWindow::refreshInfoList(const QString &webAddress)
{
    infoModel->clear();
    QVector<Credential> &infos = webInfoMap[webAddress];

    for (int i = 0; i < infos.size(); ++i) {
        Credential &info = infos[i];

        QStandardItem *paddingItem = new QStandardItem("");
        paddingItem->setSizeHint(QSize(0, 25));
        infoModel->appendRow(paddingItem);

        QString labelText = QString::asprintf("<b>Email:</b> %s<br><b>Password:</b> ****", info.email.toUtf8().constData());
        QLabel *textLabel = new QLabel(labelText);
        QFont textFont = textLabel->font();
        textFont.setPointSize(20);
        textLabel->setFont(textFont);

        QStandardItem *textItem = new QStandardItem();
        textItem->setSizeHint(QSize(200, 70));
        infoModel->appendRow(textItem);
        QModelIndex textIndex = infoModel->indexFromItem(textItem);
        infoListView->setIndexWidget(textIndex, textLabel);

        QPushButton *editButton = new QPushButton("Edit");
        QPushButton *deleteButton = new QPushButton("Delete");
        QPushButton *copyEmailButton = new QPushButton("Copy Email");
        QPushButton *copyPasswordButton = new QPushButton("Copy Password");

        editButton->setMinimumHeight(40);
        editButton->setMaximumHeight(40);

        deleteButton->setMinimumHeight(40);
        deleteButton->setMaximumHeight(40);

        copyEmailButton->setMinimumHeight(40);
        copyEmailButton->setMaximumHeight(40);

        copyPasswordButton->setMinimumHeight(40);
        copyPasswordButton->setMaximumHeight(40);

        QFont buttonFont = editButton->font();
        buttonFont.setPointSize(24);

        editButton->setFont(buttonFont);
        deleteButton->setFont(buttonFont);
        copyEmailButton->setFont(buttonFont);
        copyPasswordButton->setFont(buttonFont);


        connect(copyEmailButton, &QPushButton::clicked, [info] {
            QClipboard *clipboard = QApplication::clipboard();
            clipboard->setText(info.email);
        });

        connect(copyPasswordButton, &QPushButton::clicked, [info] {
            QClipboard *clipboard = QApplication::clipboard();
            clipboard->setText(info.password);
        });

        QHBoxLayout *buttonLayout = new QHBoxLayout();
        buttonLayout->addWidget(editButton);
        buttonLayout->addWidget(deleteButton);
        buttonLayout->addWidget(copyEmailButton);
        buttonLayout->addWidget(copyPasswordButton);

        editButton->setStyleSheet("margin-top: -20px; background: transparent;");
        deleteButton->setStyleSheet("margin-top: -20px; background: transparent;");
        copyEmailButton->setStyleSheet("margin-top: -20px; background: transparent;");
        copyPasswordButton->setStyleSheet("margin-top: -20px; background: transparent;");

        connect(editButton, &QPushButton::clicked, [this, info, webAddress, i] {
            EditCredentialDialog dialog(this, info.email, info.password);
            if (dialog.exec() == QDialog::Accepted) {
                Credential &originalInfo = webInfoMap[webAddress][i];
                originalInfo.email = dialog.getEmail();
                originalInfo.password = dialog.getPassword();
                writeToJsonFile();
                refreshInfoList(webAddress);
            }
        });

        connect(deleteButton, &QPushButton::clicked, [this, webAddress, i] {
            webInfoMap[webAddress].removeAt(i);
            writeToJsonFile();
            refreshInfoList(webAddress);
        });


        QWidget *buttonWidget = new QWidget();
        buttonWidget->setLayout(buttonLayout);

        QStandardItem *buttonItem = new QStandardItem();
        buttonItem->setSizeHint(QSize(200, 40));
        infoModel->appendRow(buttonItem);
        QModelIndex buttonIndex = infoModel->indexFromItem(buttonItem);
        infoListView->setIndexWidget(buttonIndex, buttonWidget);
    }
}



QString encrypt(const QString& data, const QString& key)
{
    QByteArray ba = data.toUtf8();
    int keyLength = key.length();
    for (int i = 0; i < ba.size(); ++i) {
        ba[i] = ba[i] ^ key[i % keyLength].toLatin1();
    }
    return QString(ba);
}

QString decrypt(const QString& data, const QString& key)
{
    QByteArray ba = data.toUtf8();
    int keyLength = key.length();
    for (int i = 0; i < ba.size(); ++i) {
        ba[i] = ba[i] ^ key[i % keyLength].toLatin1();
    }
    return QString(ba);
}



void MainWindow::readFromJsonFile()
{
    QFile file("data.json");
    if (!file.exists()) {
        qDebug() << "File does not exist, creating a new one.";
        if (file.open(QIODevice::WriteOnly)) {
            file.close();
        } else {
            qDebug() << "Failed to create new file. Error:" << file.errorString();
            return;
        }
    }

    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open file for reading. Error:" << file.errorString();
        return;
    }

    QByteArray saveData = file.readAll();
    file.close();

    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));

    QJsonObject json = loadDoc.object();
    QJsonArray websiteArray = json["websites"].toArray();

    webInfoMap.clear();

    for (int websiteIndex = 0; websiteIndex < websiteArray.size(); ++websiteIndex) {
        QJsonObject websiteObject = websiteArray[websiteIndex].toObject();
        QString webAddress = websiteObject["webAddress"].toString();

        QVector<Credential> credentials;
        QJsonArray credentialArray = websiteObject["credentials"].toArray();

        QString key = "9sD9G2GkQ7bYxMvZu3D8vKZ2R6fT5P";
        for (int credentialIndex = 0; credentialIndex < credentialArray.size(); ++credentialIndex) {
            QJsonObject credentialObject = credentialArray[credentialIndex].toObject();
            Credential credential;
            credential.email = decrypt(credentialObject["email"].toString(), key);
            credential.password = decrypt(credentialObject["password"].toString(), key);
            credentials.append(credential);
        }

        webInfoMap[webAddress] = credentials;
    }
    webAddressModel->setStringList(webInfoMap.keys());
}


void MainWindow::writeToJsonFile()
{
    QJsonArray websiteArray;
    for (const QString& webAddress : webInfoMap.keys())
    {
        qDebug() << "Processing webAddress: " << webAddress;
        QJsonObject websiteObject;
        websiteObject["webAddress"] = webAddress;

        QJsonArray credentialArray;
        const QVector<Credential>& credentials = webInfoMap[webAddress];
        QString key = "9sD9G2GkQ7bYxMvZu3D8vKZ2R6fT5P";

        for (const Credential& credential : credentials)
        {
            QJsonObject credentialObject;
            credentialObject["email"] = encrypt(credential.email, key);
            credentialObject["password"] = encrypt(credential.password, key);
            credentialArray.append(credentialObject);
        }


        websiteObject["credentials"] = credentialArray;
        websiteArray.append(websiteObject);
    }

    QJsonObject rootObject;
    rootObject["websites"] = websiteArray;

    QFile jsonFile("data.json");

    if (!jsonFile.open(QIODevice::WriteOnly))
    {
        qWarning("Couldn't open save file.");
        return;
    }
    QJsonDocument jsonDoc(rootObject);
    jsonFile.write(jsonDoc.toJson());
    jsonFile.close();
}



