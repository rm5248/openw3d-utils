#include <QRegularExpression>
#include <iostream>

#include "mixfile.h"
#include "mixfilemodel.h"

MIXFileModel::MIXFileModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

void MIXFileModel::setMixFile(openw3d::MIXFile* mix){
    m_mix = mix;

    beginResetModel();
    m_mix_filenames = m_mix->filenames();
    endResetModel();
}

QVariant MIXFileModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal && role == Qt::DisplayRole){
        switch(section){
        case 0:
            return QVariant("Name");
        case 1:
            return QVariant("Size");
        }
    }

    return QVariant();
}

int MIXFileModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    if(m_mix == nullptr){
        return 0;
    }

    return m_mix_filenames.size();
}

int MIXFileModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return 2;
}

QVariant MIXFileModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if(role != Qt::DisplayRole){
        return QVariant();
    }

    if(index.column() == 0){
        QString str = QString::fromStdString(m_mix_filenames[index.row()]);
        return QVariant(str);
    }

    if(index.column() == 1){
        return QVariant(m_mix->get_file_size(m_mix_filenames[index.row()]));
    }

    return QVariant();
}

void MIXFileModel::displayFilesOfTypes(QVector<QString> extensions){
    m_current_extensions = extensions;

    filterFiles();
}

QSet<QString> MIXFileModel::validExtensions() const{
    QSet<QString> ret;

    for(const std::string& str : m_mix_filenames){
        const std::filesystem::path path = str;
        ret.insert(QString::fromStdString(path.extension()));
    }

    return ret;
}

void MIXFileModel::displayFilesLike(QString name){
    QRegularExpression regex(name);

    m_current_regex = regex;

    filterFiles();
}

void MIXFileModel::filterFiles(){
    beginResetModel();
    std::vector<std::string> names = m_mix->filenames();
    m_mix_filenames.clear();
    for(const std::string& name : names){
        const std::filesystem::path path = name;
        QString filename_str = QString::fromStdString(name);
        QString extension_str = QString::fromStdString(path.extension());

        bool valid_extension = true;

        if(!m_current_extensions.empty()){
            valid_extension = m_current_extensions.contains(extension_str);
        }

        if(valid_extension && m_current_regex.isValid()){
            // Check to see if this also passes the regex
            if(m_current_regex.match(filename_str).hasMatch()){
                m_mix_filenames.push_back(name);
            }
        }else if(valid_extension){
            // The regex is not valid - assume that means it is empty, so we only check extension
            m_mix_filenames.push_back(name);
        }
    }
    endResetModel();
}

QString MIXFileModel::fileAtIndex(int index){
    return QString::fromStdString(m_mix_filenames[index]);
}
