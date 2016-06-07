#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QObject>
#include <QIcon>

/**
 * @brief MainWindow that displays the interface for the program.
 * @param parent parent widget of mainwindow = 0
 */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("MilaOCRv1.0.0");

    // create all layouts
    QWidget* main_window=new QWidget;//create a central widget
    QVBoxLayout* vlayout1 = new QVBoxLayout;//main layout for application
    QHBoxLayout* hlayout1 = new QHBoxLayout;//create top horizontal layout, for buttons
    QHBoxLayout* hlayout2 = new QHBoxLayout;//middle horiz layout, img + text boxes
    QHBoxLayout* hlayout3 = new QHBoxLayout;//bottom horiz layout, export button
    QVBoxLayout* vlayout2 = new QVBoxLayout;//layout for file and ocr buttons

    QLabel* header = new QLabel("<h3>MilaOCR</h3>");
    QPixmap pic(":/Image/MilaOCR.png");
    //to set buttons as icons with pictures
    //QPixmap import_pic(":/Image/importfile.png");
    //QPixmap export_pic(":/Image/export.png");
    //QIcon export_icon(export_pic);
    //QIcon import_icon(import_pic);
    header->setPixmap(pic);
    header->setAlignment(Qt::AlignCenter);
    QPushButton* openFileButton = new QPushButton("Choose File");//open file button
    openFileButton->setFixedWidth(150);
    //openFileButton->setFont(QFont::);
    //openFileButton->setIcon(import_icon);
    //openFileButton->setIconSize(import_pic.size());
    //set tooltip
    openFileButton->setToolTip("Max file size: 1MB");
    QPushButton* performOCR = new QPushButton("Perform OCR");//perform OCR button
    performOCR->setFixedWidth(150);
    //set tooltip
    performOCR->setToolTip("Click to perform OCR and see results");
    QPushButton* exportToFile = new QPushButton("Export");//export button
    exportToFile->setFixedWidth(100);
    //exportToFile->setIcon(export_icon);
    //exportToFile->setIconSize(export_pic.size());
    //set tooltip
    exportToFile->setToolTip("Save the results to a text file");

    //Qlabels to hold OCR image and OCR'd text
    displayImage->setStyleSheet("border: 1.4px solid black");
    displayImage->setFixedSize(350,400);
    displayImage->setScaledContents(true);
    displayText->setStyleSheet("border: 1.4px solid black");
    displayText->setFixedSize(350,400);
    displayText->setScaledContents(true);

    //set signals and slots for buttons
    QObject::connect(openFileButton,SIGNAL(clicked()),this,SLOT(chooseFile()));
    QObject::connect(performOCR,SIGNAL(clicked()),this,SLOT(recognize()));
    QObject::connect(exportToFile,SIGNAL(clicked()),this,SLOT(saveToFile()));

    //add everything to layouts
    vlayout2->addWidget(openFileButton);
    vlayout2->addWidget(performOCR);
    hlayout2->addWidget(displayImage);
    hlayout2->addLayout(vlayout2);
    hlayout2->addWidget(displayText);
    hlayout3->addWidget(exportToFile);
    hlayout3->setAlignment(Qt::AlignRight);

    //add everything to main layout
    vlayout1->addWidget(header);
    vlayout1->addLayout(hlayout1);
    vlayout1->addLayout(hlayout2);
    vlayout1->addLayout(hlayout3);
    main_window->setLayout(vlayout1);
    //set central widget
    this->setCentralWidget(main_window);
}

/**
 * @brief Destructor
 */
MainWindow::~MainWindow()
{
    delete ui;
}

/**
  * @brief Convert any image uploaded by the user into a greyscaled image
  * @param origin original image uploaded by the user
  * @return QImage pointer that points to the greyscaled image
  */
 QImage* MainWindow::greyScale(QImage* origin)
 {
     QImage* newImage = new QImage(origin->width(), origin->height(), QImage::Format_ARGB32);

      QRgb* line;

      for(int y = 0; y<newImage->height(); y++){
         line = (QRgb *)origin->scanLine(y);

        for(int x = 0; x<newImage->width(); x++){
          int average = (qRed(line[x]) + qGreen(line[x]) + qRed(line[x]))/3;
          newImage->setPixel(x,y, qRgb(average, average, average));
        }

      }
      return newImage;
 }

 /**
 * @brief Function that chooses a file from the user's computer drive and opens it
 */
void MainWindow::chooseFile()
{
    fileName = QFileDialog::getOpenFileName(this,tr("open file"),"C:/",tr("Allfile(*.*)"));//open a dialog
    if(!fileName.isNull())//if a file is selected, do something
    {
           originalImage->load(fileName);
           greyScaleImage=greyScale(originalImage);
           greyScaleImage->scaled(350,400,Qt::KeepAspectRatio);
           displayImage->setPixmap(QPixmap::fromImage(*greyScaleImage));
           //displayImage->setPixmap(QPixmap::fromImage(*greyScaleImage).scaled(displayImage->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
           displayImage->setScaledContents(true);
    }
    else{
        std::cout<<"fail to open file"<<std::endl;
    }
}

/**
 * @brief Function that saves the parsed results of OCR to .txt file
 *        on your computer
 */
void MainWindow::saveToFile()
{
    //saveLocation = QFileDialog::getExistingDirectory(this,tr("Save As"),"C:/",QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks);//open a dialog
    QString saveAsName = QFileDialog::getSaveFileName(this,tr("Saved As"),"C:/untitled.txt",tr("(*.txt)"));
    if(!saveAsName.isNull())//if a file is selected, do something
    {
        QFile outputFile(saveAsName);
        outputFile.open(QFile::WriteOnly);
        QTextStream out(&outputFile);
        //out<<"ęłżś";
        //out<<"a";
        out<<text;
        outputFile.close();
    }
    else{
        std::cout<<"failed to choose file"<<std::endl;
    }
}

/**
 * @brief Function that sets the format of the multipart message we are sending
 * @param key parameter required to send to API
 * @param value
 * @return QHttpPart object to be added to multipart message
 */
QHttpPart part_parameter(QString key, QString value) {
    QHttpPart part;
    part.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\""+ key +"\""));
    part.setBody(value.toLatin1());
    return part;
}

/**
 * @brief Function that sends the multipart message request to the API server.
 *        Shows the image being OCR'd in the left-hand side of the interface
 */
void MainWindow::recognize(){
    QHttpMultiPart* multipart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    //set up http part message to send to api that contains image data
    QHttpPart imagePart;
    imagePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/gif"));
    imagePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"file\"; filename=\"./Polish_test2.gif\""));

    QFile* file = new QFile(fileName);

    //debugging: make sure file was uploaded
    if(!file->open(QIODevice::ReadOnly)){
        qDebug() << "# Could not upload/open file";
    }

    QByteArray fileContent(file->readAll());
    imagePart.setBody(fileContent);

    //append image data, api key, language, and overlay setting to multipart
    multipart->append(imagePart);
    multipart->append(part_parameter("apikey","3653fc62e388957"));
    multipart->append(part_parameter("language","pol"));
    multipart->append(part_parameter("isOverlayRequired","false"));

    //ocr api url
    QUrl api_url("https://apifree2.ocr.space/parse/image");

    //create network request obj that contains the api url
    QNetworkRequest api_request(api_url);
    manager = new QNetworkAccessManager;
    //=(url, multipart encoded message)
    reply = manager->post(api_request, multipart);

    QObject::connect(reply, SIGNAL(finished()), this, SLOT(networkData()));
    //debugging: make sure file was opened; if 0 bytes, it wasn't!
    qDebug() << file->size() << "bytes";

    imagePart.setBodyDevice(file);
    file->setParent(multipart);
    networkData();
}

/**
 * @brief Function that retrieves network reply from the API, formats the
 *        response and inserts it in Qlabel object displayText to show the
 *        results on the interface.
 */
void MainWindow::networkData(){
    //tests for network error
     QNetworkReply::NetworkError err = reply->error();
     if (err != QNetworkReply::NoError) {
         qDebug() << reply->errorString();
         return;
     }

    //store the network's response in a string
    QString response = (QString)reply->readAll();

    //network reply is stored in JSON format; get only the OCR'd text results
    QJsonDocument jsonDoc = QJsonDocument::fromJson(response.toUtf8());
    QJsonObject jsonObj = jsonDoc.object();
    QJsonArray jsonArr = jsonObj["ParsedResults"].toArray();
    foreach(const QJsonValue& value, jsonArr) {
        QJsonObject obj = value.toObject();
        text.append(obj["ParsedText"].toString());
    }
    //display the results on the interface
    displayText->setText(text);
    displayText->setTextInteractionFlags(Qt::TextSelectableByMouse);
    qDebug() << text;
}


