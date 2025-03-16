#include <QFileDialog>
#include <QMessageBox>
#include <QLabel>

#include "rmmixviewer.h"
#include "./ui_rmmixviewer.h"

RMMixViewer::RMMixViewer(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::RMMixViewer)
{
    ui->setupUi(this);
    m_mix_file_label = new QLabel(this);
    ui->mixFileContents->setModel(&m_mix_table_model);
    ui->statusbar->addPermanentWidget(m_mix_file_label);
    m_mix_file_label->setText("No MIX file open");

    m_file_type_menu = new QMenu(this);
    ui->fileTypeButton->setMenu(m_file_type_menu);
    ui->fileTypeButton->setPopupMode(QToolButton::InstantPopup);

    m_view_all_types = new QAction("All file types", this);
    m_file_type_menu->addAction(m_view_all_types);

    m_regex_debounce.setInterval(250);
    m_regex_debounce.setSingleShot(true);

    connect(m_view_all_types, &QAction::triggered,
            this, &RMMixViewer::viewAllExtensions);
    connect(&m_regex_debounce, &QTimer::timeout,
            this, &RMMixViewer::lineedit_done);
    connect(ui->mixFileContents, &QTableView::clicked,
            this, &RMMixViewer::tableClicked);
}

RMMixViewer::~RMMixViewer()
{
    delete ui;
}


void RMMixViewer::on_actionExit_triggered()
{
    QApplication::exit();
}


void RMMixViewer::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open MIX File"),
                                                    QString(),
                                                    tr("MIX Files (*.mix *.MIX *.dat *.DAT);;All Files (*)"));

    if(fileName.isEmpty()){
        return;
    }

    const std::filesystem::path path = fileName.toStdString();
    if(!m_mix.open(path)){
        QMessageBox::critical(this, "Unable to open MIX", "Unable to open MIX file");
        return;
    }

    m_mix_table_model.setMixFile(&m_mix);
    m_mix_file_label->setText(QString::fromStdString(path.string()));
    ui->mixFileContents->resizeColumnToContents(0);
    ui->mixFileContents->resizeColumnToContents(1);

    m_all_file_types.clear();
    m_file_type_menu->clear();
    m_file_type_menu->addAction(m_view_all_types);
    for(QString extension : m_mix_table_model.validExtensions()){
        QAction* action = new QAction(extension, m_file_type_menu);
        action->setCheckable(true);
        m_file_type_menu->addAction(action);
        m_all_file_types.push_back(action);

        connect(action, &QAction::triggered,
                this, &RMMixViewer::fileTypeSelected);
    }
}

void RMMixViewer::viewAllExtensions(){
    for(QAction* action : m_all_file_types){
        action->setChecked(false);
    }

    fileTypeSelected();
}

void RMMixViewer::fileTypeSelected(){
    QVector<QString> typesToView;
    for(QAction* action : m_all_file_types){
        if(action->isChecked()){
            typesToView.push_back(action->text());
        }
    }

    m_mix_table_model.displayFilesOfTypes(typesToView);
}

void RMMixViewer::on_lineEdit_textChanged(const QString &arg1)
{
    if(!m_regex_debounce.isActive()){
        m_regex_debounce.start();
    }
}

void RMMixViewer::lineedit_done(){
    m_mix_table_model.displayFilesLike(ui->lineEdit->text());
}

void RMMixViewer::tableClicked(const QModelIndex &index){
    QString filename = m_mix_table_model.fileAtIndex(index.row());
    std::string filename_std = filename.toStdString();

    auto fileinfo = m_mix.file_info(filename_std);

    if(fileinfo){
        ui->fileDisplay->displayFileFromMIX(filename,
                                            fileinfo.value(),
                                            m_mix.get_file(filename_std));
    }
}
