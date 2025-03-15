#ifndef MIXFILEMODEL_H
#define MIXFILEMODEL_H

#include <QAbstractTableModel>
#include <QRegularExpression>

namespace openw3d {
class MIXFile;
}

class MIXFileModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit MIXFileModel(QObject *parent = nullptr);

    void setMixFile(openw3d::MIXFile* mix);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void displayFilesOfTypes(QVector<QString> extensions);
    QSet<QString> validExtensions() const;

    /**
     * Display files named like 'name'.  Used as a regex.
     * @param name
     */
    void displayFilesLike(QString name);

private:
    void filterFiles();

private:
    openw3d::MIXFile* m_mix = nullptr;
    std::vector<std::string> m_mix_filenames;

    // Filtering variables
    QVector<QString> m_current_extensions;
    QRegularExpression m_current_regex;
};

#endif // MIXFILEMODEL_H
