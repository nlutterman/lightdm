#include "themesmodel.h"

#include <QString>
#include <QPixmap>
#include <QList>
#include <QDebug>
#include <QDir>
#include <QSettings>

#include <KStandardDirs>
#include <KGlobal>


class ThemeItem {
public:
    QString id;
    QString name;
    QString description;
    QString author;
    QString version;
    QPixmap preview;
};

ThemesModel::ThemesModel(QObject *parent) :
    QAbstractListModel(parent)
{
    //FUTURE FIXME: do the single shot trick so we can start displaying the UI before bothering to do the loading.
    this->load();
}

int ThemesModel::rowCount(const QModelIndex &parent) const
{
    return m_themes.size();
}

QVariant ThemesModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    switch(role) {
    case Qt::DisplayRole:
        return m_themes[row]->name;
    case Qt::DecorationRole:
        if (m_themes[row]->preview.isNull()) {
            return QVariant();
        }
        //FIXME shouldn't really be scaling here, it's a bit slow - in the delegate is better.
        return m_themes[row]->preview.scaled(QSize(100,100), Qt::KeepAspectRatio);
    case ThemesModel::PreviewRole:
        return m_themes[row]->preview;
    case ThemesModel::IdRole:
        return m_themes[row]->id;
    case ThemesModel::DescriptionRole:
        return m_themes[row]->description;
    case ThemesModel::VersionRole:
        return m_themes[row]->version;
    case ThemesModel::AuthorRole:
        return m_themes[row]->author;
    }

    return QVariant();
}

void ThemesModel::load()
{
    qDebug() << "loading themes";
    QStringList themeDirPaths = KGlobal::dirs()->findDirs("data", "lightdm-kde-greeter/themes");
    qDebug() << themeDirPaths;

    //get a list of possible theme directories, loop through each of these finding themes.
    //FIXME I think this can be simplified to return all possible themes directly

    foreach(const QString themeDirPath, themeDirPaths)
    {
        QDir themeDir(themeDirPath);
        foreach(const QString dirPath, themeDir.entryList(QDir::NoDotAndDotDot | QDir::Dirs)) {
            qDebug() << themeDir.filePath(dirPath + "/theme.rc");
            if (QFile::exists(themeDir.filePath(dirPath + "/theme.rc"))) {
                loadTheme(QDir(themeDir.filePath(dirPath)));
            }
        }
    }
}

void ThemesModel::loadTheme(const QDir &themePath) {
    QSettings themeInfo(themePath.filePath("theme.rc"), QSettings::IniFormat);

    ThemeItem *theme = new ThemeItem;
    theme->id = themePath.dirName();
    theme->name = themeInfo.value("theme/Name").toString();
    theme->description = themeInfo.value("theme/Description").toString();
    theme->author = themeInfo.value("theme/Author").toString();
    theme->version = themeInfo.value("theme/Version").toString();

    theme->preview = QPixmap(themePath.absoluteFilePath("preview.png"));

    qDebug() << QString("adding theme") << theme->name;

    beginInsertRows(QModelIndex(), m_themes.size(), m_themes.size()+1);
    m_themes.append(theme);
    endInsertRows();
}
