#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->sobel_gb->hide();
    ui->binary_gb->hide();

    capture = cv::VideoCapture(0);
    if(!capture.isOpened())  // check if we succeeded
            return ;

    capture_timer = new QTimer(this);

    capture_timer->setInterval(100);
    capture_timer->start();

    connect(capture_timer,SIGNAL(timeout()),this,SLOT(get_new_frame()));
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::get_new_frame(){
     capture.read(origial_frame);
     if (!origial_frame.empty()){
         cv::cvtColor(origial_frame,origial_frame,CV_BGR2RGB);
         QImage q_img_original ((uchar*) origial_frame.data,origial_frame.cols,origial_frame.rows,origial_frame.step,QImage::Format_RGB888); // for color images
         ui->origianl_img_lbl->setPixmap(QPixmap::fromImage(q_img_original).scaled(ui->origianl_img_lbl->size(), Qt::KeepAspectRatio));
     }

     if(type==output_type::gray_scale){
        ui->output_img_lbl->setPixmap(QPixmap::fromImage(get_gray_scale(origial_frame)).scaled(ui->output_img_lbl->size(), Qt::KeepAspectRatio));
     }else if(type == output_type::sobel_operator){
        ui->output_img_lbl->setPixmap(QPixmap::fromImage(get_sobel_operator(origial_frame)).scaled(ui->output_img_lbl->size(), Qt::KeepAspectRatio));
     }else if(type == output_type::binary_black_white){
         ui->output_img_lbl->setPixmap(QPixmap::fromImage(binary(origial_frame)).scaled(ui->output_img_lbl->size(), Qt::KeepAspectRatio));
      }
}

QImage MainWindow::get_gray_scale(cv::Mat original){
    cv::cvtColor(origial_frame,output_frame,CV_RGB2GRAY);
    QImage q_img_output ((uchar*) output_frame.data,output_frame.cols,output_frame.rows,output_frame.step,QImage::Format_Grayscale8); // for color images
    return q_img_output;

}

QImage MainWindow::get_sobel_operator(cv::Mat original){
    /// Generate grad_x and grad_y
    cv::Mat grad_x, grad_y;
    cv::Mat abs_grad_x, abs_grad_y;
    int scale = ui->scale_hsb->value();
    int delta =ui->delta_hsb->value();
    int ddepth = CV_16S;


    cv::cvtColor(origial_frame,output_frame,CV_RGB2GRAY);
    /// Gradient X
    Sobel( output_frame, grad_x, ddepth, 1, 0, 3, scale, delta, cv::BORDER_DEFAULT );
    convertScaleAbs( grad_x, abs_grad_x );

    /// Gradient Y
    Sobel( output_frame, grad_y, ddepth, 0, 1, 3, scale, delta, cv::BORDER_DEFAULT );
    convertScaleAbs( grad_y, abs_grad_y );

    /// Total Gradient (approximate)
    addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, output_frame );

    QImage q_img_output ((uchar*) output_frame.data,output_frame.cols,output_frame.rows,output_frame.step,QImage::Format_Indexed8); // for color images
    return q_img_output;
}

QImage MainWindow::binary(cv::Mat input){
    cv::cvtColor(origial_frame,output_frame,CV_RGB2GRAY);
    cv::threshold( output_frame,output_frame, ui->threshold_hsb->value(),255,cv::THRESH_BINARY );
    QImage q_img_output ((uchar*) output_frame.data,output_frame.cols,output_frame.rows,output_frame.step,QImage::Format_Indexed8); // for color images
    return q_img_output;

}

void MainWindow::on_groupBox_clicked(bool checked)
{
    std::cout<<"clicked"<<std::endl;
}


void MainWindow::on_gray_scale_rb_clicked(bool checked)
{
     type = output_type::gray_scale;
     ui->sobel_gb->hide();
     ui->binary_gb->hide();

}

void MainWindow::on_sobel_rb_clicked(bool checked)
{
    type = output_type::sobel_operator;
    ui->sobel_gb->show();
    ui->binary_gb->hide();
}

void MainWindow::on_scale_hsb_rangeChanged(int min, int max)
{

}

void MainWindow::on_delta_hsb_valueChanged(int value)
{
    ui->delta_value_lbl->setText(QString::fromStdString(std::to_string(value)));
}

void MainWindow::on_scale_hsb_valueChanged(int value)
{
    ui->scale_value_lbl->setText(QString::fromStdString(std::to_string(value)));
}

void MainWindow::on_binary_rb_clicked(bool checked)
{
    type = output_type::binary_black_white;
    ui->sobel_gb->hide();
    ui->binary_gb->show();
}

void MainWindow::on_threshold_hsb_valueChanged(int value)
{
    ui->threshold_value_lbl->setText(QString::fromStdString(std::to_string(value)));
}
