#ifndef EXTRAROWPROXYMODEL_H
#define EXTRAROWPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QHash>
#include <QVector>

/**
 * A proxy model which makes it possible to append extra rows at the end
 */
class ExtraRowProxyModel : public QAbstractListModel
{
public:
    ExtraRowProxyModel(QObject *parent = 0);

    /**
     * Append a row, returns a row id
     */
    int appendRow();

    void setRowData(int id, int column, const QVariant &value, int role);
    void setRowText(int id, int column, const QVariant &value);

    int rowCount(const QModelIndex &parent = QModelIndex()) const; // reimp
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const; // reimp

    void setSourceModel(QAbstractItemModel* model);

private:
    typedef QHash<int, QVariant> Item;
    typedef QVector<Item> Row;
    typedef QVector<Row> Rows;

    QAbstractItemModel* m_model;
    Rows m_rows;
};

#endif /* EXTRAROWPROXYMODEL_H */
