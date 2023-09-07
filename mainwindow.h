#ifndef MAINWINDOW_H  // Include guard to prevent multiple inclusions
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListView>
#include <QLabel>
#include <QStringListModel>
#include <QMap>
#include <QPushButton>
#include <QVector>
#include <QStandardItemModel>
#include <QVBoxLayout>


struct Credential {
    QString email;
    QString password;
};

class MainWindow : public QMainWindow
{
Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void addWebAddress();
    void addInfo();
    void onWebAddressSelectionChanged(const QModelIndex &current, const QModelIndex &previous);

private:
    QListView *webAddressListView;
    QListView *infoListView;
    QLabel *infoLabel;
    QPushButton *addWebAddressButton;
    QPushButton *deleteAddressButton;
    QPushButton *addInfoButton;

    QStringListModel *webAddressModel;
    QStandardItemModel *infoModel;

    // Data storage
    QMap<QString, QVector<Credential>> webInfoMap;

    //functions
    void readFromJsonFile();
    void writeToJsonFile();
    void refreshInfoList(const QString &webAddress);
    void deleteWebAddress();
};

#endif // MAINWINDOW_H
