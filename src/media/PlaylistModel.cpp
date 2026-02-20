/**
 * @file PlaylistModel.cpp
 * @brief Stub - empty playlist
 * @author Ahn Hyunjun
 * @date 2026-02-20
 */

#include "PlaylistModel.h"

PlaylistModel::PlaylistModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int PlaylistModel::rowCount(const QModelIndex &) const
{
    return m_urls.size();
}

QVariant PlaylistModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_urls.size()) return QVariant();

    switch (role) {
    case UrlRole: return m_urls[index.row()];
    case TitleRole: return m_titles.value(index.row(), "Unknown");
    case ArtistRole: return m_artists.value(index.row(), "Unknown");
    }
    return QVariant();
}

QHash<int, QByteArray> PlaylistModel::roleNames() const
{
    return { { UrlRole, "url" }, { TitleRole, "title" }, { ArtistRole, "artist" } };
}
