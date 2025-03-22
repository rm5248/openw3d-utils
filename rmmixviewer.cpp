#include <QFileDialog>
#include <QMessageBox>
#include <QLabel>
#include <QTableWidgetItem>

#include <fmt/format.h>
#include <log4cxx/logger.h>

#include "rmmixviewer.h"
#include "./ui_rmmixviewer.h"

static auto logger = log4cxx::Logger::getLogger("RMMixViewer");

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

    ui->mixFileContents->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(m_view_all_types, &QAction::triggered,
            this, &RMMixViewer::viewAllExtensions);
    connect(&m_regex_debounce, &QTimer::timeout,
            this, &RMMixViewer::lineedit_done);
    connect(ui->mixFileContents, &QTableView::clicked,
            this, &RMMixViewer::tableClicked);
    connect(ui->mixFileContents, &QTableView::customContextMenuRequested,
            this, &RMMixViewer::rowRightClick);
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

void RMMixViewer::rowRightClick(const QPoint &pos){
    int row = ui->mixFileContents->rowAt(pos.y());
    if (row < 0) {
        return;
    }

    QMenu menu;
    QAction action_extract("Extract...");
    menu.addAction(&action_extract);
    QAction* action_to_do = menu.exec(QCursor::pos());
    if(action_to_do == nullptr){
        return;
    }

    if(action_to_do == &action_extract){
        QString toExtract = m_mix_table_model.fileAtIndex(row);
        QString toSaveAs = QFileDialog::getSaveFileName(this, "Extract file to...", toExtract );
        if(toSaveAs.isNull()){
            return;
        }

        QFile f(toSaveAs);
        if(!f.open(QIODevice::WriteOnly)){
            LOG4CXX_ERROR(logger, "Can't open file to save to");
            return;
        }
        std::optional<std::span<const char>> data = m_mix.get_file(toExtract.toStdString());
        if(!data){
            LOG4CXX_ERROR(logger, "Can't get data from MIX??");
            return;
        }
        f.write(data->data(), data->size());
    }
}
