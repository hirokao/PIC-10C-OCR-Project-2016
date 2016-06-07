/**
    OCR Project for Professor Desalvo's PIC 10C Spring 2016 class

    Group members:      Jack Wu, Zane Karl, Bess , Jennifer Okamura
    Researcher:         Mila Oiva
    Objective:          Create a program that accepts .gif images of historical newspapers
                        written in Polish and convert them to .txt files.
    Project Details:    Given the time constraint and magnitude of this project, we decided
                        to use an api that performs optical character recognition on images
                        and implement this into a program designed in Qt/C++. More inform-
                        ation can be found at the following link: https://ocr.space/OCRAPI

                        The resulting program allows a user to upload an image no greater
                        than 1MB and output the results of the character recognition to
                        on the interface. Then a user may export the results to a .txt
                        file and save it to their computer.

                        We would like to implement a cropping tool that will allow the user
                        to extract smaller parts of an image that is large in size and we
                        plan on completing this sometime during the summer of 2016.
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <iostream>
#include <QImage>
#include <QLabel>
#include <QPainter>
#include <QCheckBox>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <iostream>
#include <QEvent>
#include <QDebug>
#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QList>
#include <QString>
#include <QHttpMultiPart>
#include <QFile>
#include <QScriptEngine>
#include <QDir>
#include <QVariantMap>
#include <QByteArray>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QImage* greyScale(QImage* origin);
    //void parseApiResponse();

public slots:
    void chooseFile();
    void recognize();
    void networkData();
    void saveToFile();

private:
    Ui::MainWindow *ui;
    QString fileName;
    QString saveLocation;
    QString text;
    QLabel * gifImage = new QLabel;
    QImage* originalImage = new QImage;//original image
    QImage* greyScaleImage = new QImage;//greyscaled image
    QLabel* displayImage = new QLabel;
    QNetworkReply* reply;
    QLabel* displayText = new QLabel;
    QNetworkAccessManager* manager;
};

#endif // MAINWINDOW_H
