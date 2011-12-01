#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "themesmodel.h"

#include <QUiLoader>
#include <QFile>
#include <QHBoxLayout>
#include <QPixmap>


MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QHBoxLayout *layout = new QHBoxLayout(this);
    ui->optionsWidget->setLayout(layout);

    ThemesModel *model = new ThemesModel(this);
    ui->themesList->setModel(model);
    connect(ui->themesList, SIGNAL(activated(QModelIndex)), SLOT(onThemeSelected(QModelIndex)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onThemeSelected(const QModelIndex &index)
{
    ui->nameLabel->setText(index.data().toString());
    ui->descriptionLabel->setText(index.data(ThemesModel::DescriptionRole).toString());
    ui->authorLabel->setText(index.data(ThemesModel::AuthorRole).toString());

    //FUTURE, simply load the actual QML, specify an arbitrary screen size and scale the contents.
    //we can add fake components for the greeter so QML thinks it's working.

    //can't do this easily now as we need our private kdeclarative library and my widgets from the greeter
    //could make a private lib for all this - but that seems overkill when we won't need any of that in the 4.8 only versions.
    QPixmap preview = index.data(ThemesModel::PreviewRole).value<QPixmap>();
    if (! preview.isNull()) {
        ui->preview->setPixmap(preview.scaled(QSize(250,250), Qt::KeepAspectRatio));
    } else
    {
        ui->preview->setPixmap(QPixmap());
    }


//    //this is just test code for loading config options.
//    QUiLoader loader;
//    QFile uiFile("/home/david/lightdm/lightdm-kde/kcm/sessionconfig.ui");
//    QWidget* widget = loader.load(&uiFile, this);

//    if(ui->optionsWidget->layout()->count() > 0) {
//        delete ui->optionsWidget->layout()->itemAt(0);
//    }

    //    ui->optionsWidget->layout()->addWidget(widget);
}

void MainWindow::onApply()
{
    //save to a config
}
