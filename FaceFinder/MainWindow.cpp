#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QFileDialog>
#include <QSettings>
#include <QtDebug>
#include <QDir>
#include <QMessageBox>
#include <QPainter>
#include <QLabel>


const QString CONFIG_LAST_POS_DIR = "lastPositiveDir";
const QString CONFIG_LAST_NEG_DIR = "lastNegativeDir";
const QString CONFIG_LAST_TEST_DIR = "lastTestDir";
const QString CONFIG_LAST_SERIALIZE_DIR = "lastSerializeDir";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//private slot
void MainWindow::on_positiveBrowseButton_clicked()
{
    QSettings settings;
    QString lastPositive;
    if (settings.contains(CONFIG_LAST_POS_DIR))
        lastPositive = settings.value(CONFIG_LAST_POS_DIR).toString();

    QString dir = QFileDialog::getExistingDirectory(this,
                                      "Select folder with images of faces",
                                                    lastPositive);

    if (dir.isEmpty() || dir.isNull())
        return;

    settings.setValue(CONFIG_LAST_POS_DIR, dir);

    this->ui->positiveExamplesPathEdit->setText(dir);
}

//private slot
void MainWindow::on_negativeBrowseButton_clicked()
{
    QSettings settings;
    QString lastNegative;
    if (settings.contains(CONFIG_LAST_NEG_DIR))
        lastNegative = settings.value(CONFIG_LAST_NEG_DIR).toString();

    QString dir = QFileDialog::getExistingDirectory(this,
                                      "Select folder with images of not faces",
                                                    lastNegative);

    if (dir.isEmpty() || dir.isNull())
        return;

    settings.setValue(CONFIG_LAST_NEG_DIR, dir);

    this->ui->negativeExamplesPathEdit->setText(dir);
}

//private slot
void MainWindow::on_trainButton_clicked()
{
    QDir positiveDir(this->ui->positiveExamplesPathEdit->text());
    QDir negativeDir(this->ui->negativeExamplesPathEdit->text());

    if (this->ui->positiveExamplesPathEdit->text().isEmpty()
            || this->ui->negativeExamplesPathEdit->text().isEmpty())
        return;
    else if (!positiveDir.exists() || !positiveDir.isReadable())
    {
        const QString error = "Could not read positive dir. Aborting.";
        qWarning() << error;
        QMessageBox::warning(this, "Warning", error);
        return;
    }
    else if (!negativeDir.exists() || !negativeDir.isReadable())
    {
        const QString error = "Could not read negative dir. Aborting.";
        qWarning() << error;
        QMessageBox::warning(this, "Warning", error);
        return;
    }
    else if (positiveDir == negativeDir)
    {
        const QString error = "Positive and negative example cannot be in same folder.";
        qWarning() << error;
        QMessageBox::warning(this, "Warning", error);
        return;
    }

    QList<IntegralImage> positives;
    QList<IntegralImage> negatives;

    QFileInfoList positiveList = positiveDir.entryInfoList(QDir::Files);
    foreach(const QFileInfo& info, positiveList)
    {
        IntegralImage img(info.absoluteFilePath());
        if (img.isNull())
            continue;
        positives.append(img);
    }

    QFileInfoList negativeList = negativeDir.entryInfoList(QDir::Files);
    foreach(const QFileInfo& info, negativeList)
    {
        IntegralImage img(info.absoluteFilePath());
        if (img.isNull())
            continue;
        negatives.append(img);
    }


    qDebug() << positives.size() << "positive examples." << negatives.size() << "negative examples";

    Trainer trainer(this->ui->maxFalsePositivePerLayer->value(),
                    this->ui->minTruePositivePerLayer->value(),
                    this->ui->maxOverallFalsePositive->value(),
                    positives,
                    negatives,
                    this->ui->validationSetPortion->value());
    _chain = trainer.train();
}

//private slot
void MainWindow::on_testFileBrowseButton_clicked()
{
    QSettings settings;
    QString lastTest;
    if (settings.contains(CONFIG_LAST_TEST_DIR))
        lastTest = settings.value(CONFIG_LAST_TEST_DIR).toString();

    QString file = QFileDialog::getOpenFileName(this, "Select file to test", lastTest);

    if (file.isEmpty() || file.isNull())
        return;

    settings.setValue(CONFIG_LAST_TEST_DIR, file);

    this->ui->testFilePathEdit->setText(file);
}

//private slot
void MainWindow::on_testButton_clicked()
{
    QImage originalImage(this->ui->testFilePathEdit->text());
    IntegralImage imgToTest(this->ui->testFilePathEdit->text());

    if (_chain.isNull())
        return;
    else if (imgToTest.isNull())
    {
        QString error = "Failed to open image to test.";
        qWarning() << error;
        QMessageBox::warning(this, "Error", error);
        return;
    }

    QPainter painter(&originalImage);

    //Move over all viable origins of the test image
    bool foundAtleastOne = false;
    for (int y = 0; y < imgToTest.width() - 23; y+=8)
    {
        for (int x = 0; x < imgToTest.height() - 23; x+=8)
        {
            //Try every scale that will work here
            qreal scale = 1.0;
            while (x + 24*scale <= imgToTest.width()
                   && y + 24*scale <= imgToTest.height())
            {
                if (_chain->classify(imgToTest, QPoint(x,y), scale))
                {
                    //painter.drawRect(x, y, 24*scale, 24*scale);
                    painter.fillRect(x, y, 24*scale, 24*scale, QColor(255,0,0,30));
                    qDebug() << "Found at" << x << y << "-" << 24*scale << "x" << 24*scale;
                    foundAtleastOne = true;
                }

                scale *= 1.25;
            }
        }
    }

    if (!foundAtleastOne)
        qDebug() << "Found no faces in this test";

    QLabel * displayer = new QLabel();
    displayer->setAttribute(Qt::WA_DeleteOnClose);
    displayer->setPixmap(QPixmap::fromImage(originalImage));
    displayer->show();
}

//private slot
void MainWindow::on_actionLoad_classifier_triggered()
{
    QSettings settings;
    QString lastSerialize;
    if (settings.contains(CONFIG_LAST_SERIALIZE_DIR))
        lastSerialize = settings.value(CONFIG_LAST_SERIALIZE_DIR).toString();

    QString loadFilename = QFileDialog::getOpenFileName(this,
                                                        "Select classifier file",
                                                        lastSerialize);
    if (loadFilename.isEmpty())
        return;

    settings.setValue(CONFIG_LAST_SERIALIZE_DIR, loadFilename);

    QFile fp(loadFilename);
    fp.open(QFile::ReadOnly);
    QByteArray data = fp.readAll();
    fp.close();

    _chain = QSharedPointer<ClassifierChain>(ClassifierChain::fromBytes(data));
}

//private slot
void MainWindow::on_actionSave_classifier_triggered()
{
    if (_chain.isNull())
        return;
    QByteArray bytes = _chain->toBytes();

    QSettings settings;
    QString lastSerialize;
    if (settings.contains(CONFIG_LAST_SERIALIZE_DIR))
        lastSerialize = settings.value(CONFIG_LAST_SERIALIZE_DIR).toString();

    QString destFilename = QFileDialog::getSaveFileName(this,
                                                        "Select save destination",
                                                        lastSerialize);
    if (destFilename.isEmpty())
        return;

    settings.setValue(CONFIG_LAST_SERIALIZE_DIR, destFilename);

    QFile fp(destFilename);
    fp.open(QFile::ReadWrite);
    fp.write(bytes);
    fp.close();
}
