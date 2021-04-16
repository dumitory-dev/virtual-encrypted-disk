#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "createnew.h"
#include "copy.h"
#include "mount.h"

#include "source/classes/ved_manager/ved_manager.h"

std::unique_ptr<ved::i_ved_manager> MainWindow::uptr_ved_manager_ = ved::i_ved_manager::create<ved::ved_manager>();


MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow)

{
  ui->setupUi(this);
  connect(this,&MainWindow::signalToTable,this,&MainWindow::CreateTable);
  this->smodel = new QStandardItemModel();
  this->ui->tableView->setModel(this->smodel);
  this->update_table();
  message_.setFixedSize(500,200);


}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::on_RunD_clicked()
{

  try {

    this->uptr_ved_manager_->run_driver();
    this->message_.setText("Success!");
    this->message_.exec();

  }
  catch (const ved::c_win_api_exception & error) {

    this->message_.critical(
          nullptr,
          "Error"
          ,QString::fromWCharArray(error.GetMessageW().c_str()));


  }
  catch(const std::exception & error){

    this->message_.critical(
          nullptr,
          "Error"
          ,QString::fromUtf8(error.what()));

  }

}

void MainWindow::on_StopD_clicked()
{

  try {

    this->uptr_ved_manager_->stop_driver();
    this->message_.setText("Success!");
    this->message_.exec();

  }
  catch (const ved::c_win_api_exception & error) {

    this->message_.critical(
          nullptr,
          "Error"
          ,QString::fromWCharArray(error.GetMessageW().c_str()));


  }
  catch(const std::exception & error){

    this->message_.critical(
          nullptr,
          "Error"
          ,QString::fromUtf8(error.what()));

  }

}

void MainWindow::on_Create_clicked()
{

  CreateNew *NewDrive = new CreateNew(this);
  NewDrive->setFixedSize(500,220);
  NewDrive->setModal(true);
  NewDrive->exec();


}

void MainWindow::on_Copy_clicked()
{


  Copy *CopyFile = new Copy(this);
  CopyFile->setFixedSize(400,125);
  CopyFile->setModal(true);
  CopyFile->exec();


}

void MainWindow::on_Mount_clicked()
{


  Mount *MountDrive = new Mount(this);
  connect(MountDrive,SIGNAL(signalToTable()),this,SLOT(CreateTable()));
  MountDrive->setFixedSize(420,200);
  MountDrive->setModal(true);
  MountDrive->exec();


}

void MainWindow::on_Unmount_clicked()
{
  try {

    auto row = ui->tableView->selectionModel()->currentIndex().row();
    if  (row == -1){

        this->message_.critical(
              nullptr,
              "Error"
              ,"Device not selected!");

        return;

      }
    auto str = this->smodel->index(row,0).data().toString();

    MainWindow::uptr_ved_manager_->un_mount(str.toStdWString()[0]);

    this->message_.setText("Success!");
    this->message_.exec();

    emit signalToTable();

  }
  catch (const ved::c_win_api_exception & error) {

    this->message_.critical(
          nullptr,
          "Error"
          ,QString::fromWCharArray(error.GetMessageW().c_str()));


  }
  catch(const std::exception & error){

    this->message_.critical(
          nullptr,
          "Error"
          ,QString::fromUtf8(error.what()));

  }

}

void MainWindow::on_Refresh_clicked()
{
  emit signalToTable();
}

void MainWindow::CreateTable()
{

  try {

    auto mounted_data = MainWindow::uptr_ved_manager_->get_mounted_disks();

    this->smodel = new QStandardItemModel;
    QStandardItem *Item;
    update_table();

    for (size_t i{0};i < mounted_data.size();++i) {

        int j = 0;


        Item = new QStandardItem(QString(mounted_data[i].DriveLetter));

        smodel->setItem(static_cast<int>(i),j++,Item);

        Item = new QStandardItem(QString::number(mounted_data[i].FileSize.QuadPart / 1024 / 1024));
        smodel->setItem(static_cast<int>(i),j++,Item);

        Item = new QStandardItem(QString::fromWCharArray(mounted_data[i].FileName));
        smodel->setItem(static_cast<int>(i),j,Item);

      }

    this->ui->tableView->setModel(smodel);
    ui->tableView->resizeColumnsToContents();

  }
  catch (const ved::c_win_api_exception & error) {

    this->message_.critical(
          nullptr,
          "Error"
          ,QString::fromWCharArray(error.GetMessageW().c_str()));


  }
  catch(const std::exception & error){

    this->message_.critical(
          nullptr,
          "Error"
          ,QString::fromUtf8(error.what()));

  }



}



void MainWindow::on_pushButton_clicked()
{
  this->close();
}

void MainWindow::update_table()
{
  smodel->setHorizontalHeaderItem(0,new QStandardItem("Drive letter"));
  smodel->setHorizontalHeaderItem(1,new QStandardItem("File size(mb)"));
  smodel->setHorizontalHeaderItem(2,new QStandardItem("File path"));
  this->ui->tableView->setColumnWidth(0,100);
  this->ui->tableView->setColumnWidth(1,150);
  this->ui->tableView->setColumnWidth(2,198);
}

void MainWindow::on_pushButton_2_clicked()
{
  try {


    MainWindow::uptr_ved_manager_->un_mount_all();
    emit signalToTable();

  }
  catch (const ved::c_win_api_exception & error) {

    this->message_.critical(
          nullptr,
          "Error"
          ,QString::fromWCharArray(error.GetMessageW().c_str()));


  }
  catch(const std::exception & error){

    this->message_.critical(
          nullptr,
          "Error"
          ,QString::fromUtf8(error.what()));

  }

}
