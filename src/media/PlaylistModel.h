/**
 * @file PlaylistModel.h
 * @brief QAbstractListModel for playlist - TODO: implement
 * @author Ahn Hyunjun
 * @date 2026-02-20
 */

#ifndef PLAYLISTMODEL_H
#define PLAYLISTMODEL_H

#include <QAbstractListModel>
#include <QUrl>

class PlaylistModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Role { UrlRole = Qt::UserRole + 1, TitleRole, ArtistRole };

    explicit PlaylistModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

private:
    QVector<QString> m_titles;
    QVector<QString> m_artists;
    QVector<QUrl> m_urls;
};

#endif // PLAYLISTMODEL_H
