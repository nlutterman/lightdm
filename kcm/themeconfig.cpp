/*
This file is part of LightDM-KDE.

Copyright 2011, 2012 David Edmundson <kde@davidedmundson.co.uk>

LightDM-KDE is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

LightDM-KDE is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with LightDM-KDE.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "themeconfig.h"
#include "ui_themeconfig.h"

#include "themesmodel.h"
#include "themesdelegate.h"

#include <QUiLoader>
#include <QFile>
#include <QHBoxLayout>
#include <QPixmap>
#include <QDir>

#include <KMessageBox> //note only used for temporary warning.
#include <KDebug>
#include <KSharedConfig>
#include <KConfigGroup>
#include <KConfigDialog>
#include <KConfigSkeletonItem>
#include <KStandardDirs>
#include <KFileDialog>

#include <KAuth/Action>

#include "config.h"

ThemeConfig::ThemeConfig(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ThemeConfig),
    m_config(KSharedConfig::openConfig(LIGHTDM_CONFIG_DIR "/lightdm-kde-greeter.conf", KConfig::SimpleConfig))
{
    ui->setupUi(this);
    ui->options->setConfig(m_config);

    ui->messageWidget->hide();

    m_themesModel = new ThemesModel(this);
    ui->themesList->setModel(m_themesModel);
    ui->themesList->setItemDelegate(new ThemesDelegate(this));

    connect(ui->themesList, SIGNAL(activated(QModelIndex)), SLOT(onThemeSelected(QModelIndex)));
    connect(ui->themesList, SIGNAL(clicked(QModelIndex)), SLOT(onThemeSelected(QModelIndex)));
    connect(ui->options, SIGNAL(changed(bool)), SIGNAL(changed(bool)));
    connect(ui->installThemeButton, SIGNAL(clicked()), SLOT(onInstallThemeButtonPressed()));

    QString theme = m_config->group("greeter").readEntry("theme-name", "userbar");

    QModelIndex index = findIndexForTheme(theme);
    if (!index.isValid()) {
        kWarning() << "Could not find" << theme << "in theme list. Falling back to \"userbar\" theme.";
        index = findIndexForTheme("userbar");
        if (!index.isValid()) {
            kWarning() << "Could not find \"userbar\" theme. Something is wrong with this installation. Falling back to first available theme.";
            index = m_themesModel->index(0);
        }
    }
    ui->themesList->setCurrentIndex(index);
    onThemeSelected(index);
}

ThemeConfig::~ThemeConfig()
{
    delete ui;
}

QModelIndex ThemeConfig::findIndexForTheme(const QString& theme) const
{
    QAbstractItemModel* model = ui->themesList->model();
    for (int i=0; i < model->rowCount(); i++) {
        QModelIndex index = model->index(i, 0);
        if (index.data(ThemesModel::IdRole).toString() == theme) {
            return index;
        }
    }
    return QModelIndex();
}

void ThemeConfig::onThemeSelected(const QModelIndex &index)
{
    ui->descriptionLabel->setText(index.data(ThemesModel::DescriptionRole).toString());

    //FUTURE, simply load the actual QML, specify an arbitrary screen size and scale the contents.
    //we can add fake components for the greeter so QML thinks it's working.

    //can't do this easily now as we need our private kdeclarative library and my widgets from the greeter
    //could make a private lib for all this - but that seems overkill when we won't need any of that in the 4.8 only versions.
 
    QPixmap preview = QPixmap(index.data(ThemesModel::PreviewRole).toString());
    if (! preview.isNull()) {
        ui->preview->setPixmap(preview.scaled(QSize(300, 300), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else
    {
        ui->preview->setPixmap(QPixmap());
    }

    ui->options->setTheme(themeDir());

    emit changed(true);
}

void ThemeConfig::onInstallThemeButtonPressed()
{
    //find file to install

    QString systemInstallPath = KGlobal::dirs()->installPath("data");
    systemInstallPath.append("/lightdm-kde-greeter/themes");
    qDebug() << systemInstallPath;

    QUrl packageFilePath = KFileDialog::getOpenUrl();

    if (packageFilePath.isEmpty()) {
        return;
    }

    QVariantMap args;
    args["packageTarget"] = systemInstallPath;
    args["packagePath"] = packageFilePath.toLocalFile();

    KAuth::Action installAction("org.kde.kcontrol.kcmlightdm.installpackage");
    installAction.setHelperID("org.kde.kcontrol.kcmlightdm");
    installAction.setArguments(args);
    KAuth::ActionReply reply = installAction.execute();
    if (reply.failed()) {
        ui->messageWidget->setMessageType(KMessageWidget::Warning);
        ui->messageWidget->setText(i18n("Unable to install theme"));
        ui->messageWidget->animatedShow();
        kWarning() << "save failed:" << reply.errorDescription();

    } else {
        ui->messageWidget->setMessageType(KMessageWidget::Positive);
        ui->messageWidget->setText(i18n("Theme installed"));
        ui->messageWidget->animatedShow();
        m_themesModel->load();
    }
}

QDir ThemeConfig::themeDir() const
{
    return QDir(ui->themesList->currentIndex().data(ThemesModel::PathRole).toString());
}

QVariantMap ThemeConfig::save()
{
    QModelIndex currentIndex = ui->themesList->currentIndex();
    if (!currentIndex.isValid()) {
        return QVariantMap();
    }
    QVariantMap args;
    args["greeter/greeter/theme-name"] = currentIndex.data(ThemesModel::IdRole);

    args.unite(ui->options->save());
    return args;
}

void ThemeConfig::defaults()
{
    ui->options->defaults();
}

#include "moc_themeconfig.cpp"
