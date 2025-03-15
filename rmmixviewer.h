#ifndef RMMIXVIEWER_H
#define RMMIXVIEWER_H

#include <QMainWindow>
#include <QTimer>

#include "mixfile.h"
#include "mixfilemodel.h"

class QLabel;

QT_BEGIN_NAMESPACE
namespace Ui { class RMMixViewer; }
QT_END_NAMESPACE

class RMMixViewer : public QMainWindow
{
    Q_OBJECT

public:
    RMMixViewer(QWidget *parent = nullptr);
    ~RMMixViewer();

private slots:
    void on_actionExit_triggered();

    void on_actionOpen_triggered();

    void viewAllExtensions();
    void fileTypeSelected();

    void on_lineEdit_textChanged(const QString &arg1);
    void lineedit_done();
    void tableClicked(const QModelIndex &index);

private:
    Ui::RMMixViewer *ui;
    openw3d::MIXFile m_mix;
    MIXFileModel m_mix_table_model;
    QLabel* m_mix_file_label;
    QMenu* m_file_type_menu;
    QAction* m_view_all_types;
    QVector<QAction*> m_all_file_types;
    QTimer m_regex_debounce;
};
#endif // RMMIXVIEWER_H
