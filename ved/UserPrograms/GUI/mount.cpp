#include "mount.h"
#include "ui_mount.h"
#include <QFileDialog>

Mount::Mount(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::Mount)
{
  ui->setupUi(this);
  ui->label->setVisible(false);
  this->ui->Letter->setValidator(new QRegExpValidator(QRegExp("[A-Z]+"), this));
}

Mount::~Mount()
{
  delete ui;
}

void Mount::on_PathButton_clicked()
{
  ui->PathToFile->setText(QFileDialog::getOpenFileName(this, "Get Any File","C://"));
}

void Mount::on_Ok_clicked()
{

  try {

    if(ui->PathToFile->text().isEmpty()||ui->Password->text().isEmpty()||this->ui->Letter->text().isEmpty()){
        ui->label->setVisible(true);
        return;
      }

    if(!iswalpha(this->ui->Letter->text().toStdWString()[0])){

        this->message_.critical(
              nullptr,
              "Error"
              ,"Drive letter cannot be that value");
        return;

      }

    MainWindow::uptr_ved_manager_->mount(
          this->ui->PathToFile->text().replace('/','\\').toStdWString(),
          this->ui->Password->text().toStdString(),
          this->ui->Letter->text().toStdWString()[0]
        );

    this->message_.setText("Success!");
    this->message_.exec();
    emit signalToTable();
    close();

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

void Mount::on_Cancel_clicked()
{
  close();
}
