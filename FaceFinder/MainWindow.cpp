#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QFileDialog>
#include <QSettings>
#include <QtDebug>
#include <QDir>
#include <QMessageBox>

#include "IntegralImage.h"
#include "Trainer.h"

const QString CONFIG_LAST_POS_DIR = "lastPositiveDir";
const QString CONFIG_LAST_NEG_DIR = "lastNegativeDir";

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

    Trainer trainer(0.4,
                    0.9,
                    0.1,
                    positives,
                    negatives);
    QSharedPointer<ClassifierChain> chain = trainer.train();
}
