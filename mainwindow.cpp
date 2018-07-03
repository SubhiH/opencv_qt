#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <vector>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->sobel_gb->hide();
    ui->binary_gb->hide();
    ui->canny_gb->hide();
    ui->hsv_gb->hide();

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
         ui->output_img_lbl->setPixmap(QPixmap::fromImage(get_binary(origial_frame)).scaled(ui->output_img_lbl->size(), Qt::KeepAspectRatio));
      }else if(type == output_type::histogram){
         ui->output_img_lbl->setPixmap(QPixmap::fromImage(get_histogram(origial_frame)).scaled(ui->output_img_lbl->size(), Qt::KeepAspectRatio));
      }else if(type == output_type::normalize_block_filter){
         ui->output_img_lbl->setPixmap(QPixmap::fromImage(get_block_normalize(origial_frame)).scaled(ui->output_img_lbl->size(), Qt::KeepAspectRatio));
      }else if(type == output_type::guassian_filter){
         ui->output_img_lbl->setPixmap(QPixmap::fromImage(get_guassian(origial_frame)).scaled(ui->output_img_lbl->size(), Qt::KeepAspectRatio));
      }else if(type == output_type::median_filter){
         ui->output_img_lbl->setPixmap(QPixmap::fromImage(get_median(origial_frame)).scaled(ui->output_img_lbl->size(), Qt::KeepAspectRatio));
      }else if(type == output_type::canny){
         ui->output_img_lbl->setPixmap(QPixmap::fromImage(get_canny(origial_frame)).scaled(ui->output_img_lbl->size(), Qt::KeepAspectRatio));
      }else if(type == output_type::hsv){
         ui->output_img_lbl->setPixmap(QPixmap::fromImage(get_hsv(origial_frame)).scaled(ui->output_img_lbl->size(), Qt::KeepAspectRatio));
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

    QImage q_img_output ((uchar*) output_frame.data,output_frame.cols,output_frame.rows,output_frame.step,QImage::Format_Indexed8);
    return q_img_output;
}

QImage MainWindow::get_binary(cv::Mat input){
    cv::cvtColor(origial_frame,output_frame,CV_RGB2GRAY);
    cv::threshold( output_frame,output_frame, ui->threshold_hsb->value(),255,cv::THRESH_BINARY );
    QImage q_img_output ((uchar*) output_frame.data,output_frame.cols,output_frame.rows,output_frame.step,QImage::Format_Indexed8);
    return q_img_output;
}

QImage MainWindow::get_histogram(cv::Mat input){
    /// Separate the image in 3 places ( B, G and R )
      std::vector<cv::Mat> bgr_planes;
      cv::split( input, bgr_planes );

      /// Establish the number of bins
      int histSize = 256;

      /// Set the ranges ( for B,G,R) )
      float range[] = { 0, 256 } ;
      const float* histRange = { range };

      bool uniform = true; bool accumulate = false;

      cv::Mat b_hist, g_hist, r_hist;

      /// Compute the histograms:
      cv::calcHist( &bgr_planes[0], 1, 0, cv::Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate );
      cv::calcHist( &bgr_planes[1], 1, 0, cv::Mat(), g_hist, 1, &histSize, &histRange, uniform, accumulate );
      cv::calcHist( &bgr_planes[2], 1, 0, cv::Mat(), r_hist, 1, &histSize, &histRange, uniform, accumulate );

      // Draw the histograms for B, G and R
      int hist_w = 512; int hist_h = 400;
      int bin_w = cvRound( (double) hist_w/histSize );

      cv::Mat histImage( hist_h, hist_w, CV_8UC3, cv::Scalar( 0,0,0) );

      /// Normalize the result to [ 0, histImage.rows ]
      cv::normalize(b_hist, b_hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat() );
      cv::normalize(g_hist, g_hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat() );
      cv::normalize(r_hist, r_hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat() );

      /// Draw for each channel
      for( int i = 1; i < histSize; i++ )
      {
          line( histImage, cv::Point( bin_w*(i-1), hist_h - cvRound(b_hist.at<float>(i-1)) ) ,
                           cv::Point( bin_w*(i), hist_h - cvRound(b_hist.at<float>(i)) ),
                           cv::Scalar( 255, 0, 0), 2, 8, 0  );
          line( histImage, cv::Point( bin_w*(i-1), hist_h - cvRound(g_hist.at<float>(i-1)) ) ,
                           cv::Point( bin_w*(i), hist_h - cvRound(g_hist.at<float>(i)) ),
                           cv::Scalar( 0, 255, 0), 2, 8, 0  );
          line( histImage, cv::Point( bin_w*(i-1), hist_h - cvRound(r_hist.at<float>(i-1)) ) ,
                           cv::Point( bin_w*(i), hist_h - cvRound(r_hist.at<float>(i)) ),
                           cv::Scalar( 0, 0, 255), 2, 8, 0  );
      }
      /// Display
      cv::cvtColor(histImage,histImage,CV_BGR2RGB);
      QImage q_img_output ((uchar*) histImage.data,histImage.cols,histImage.rows,histImage.step,QImage::Format_RGB888);
      return q_img_output;
}


QImage MainWindow::get_block_normalize(cv::Mat input){
    int kernel_size=ui->threshold_hsb->value();
    if (kernel_size!=0){
        cv::blur( input, output_frame, cv::Size(kernel_size, kernel_size), cv::Point(-1,-1));
        QImage q_img_output ((uchar*) output_frame.data,output_frame.cols,output_frame.rows,output_frame.step,QImage::Format_RGB888);
        return q_img_output;
    }else{
        QImage q_img_output ((uchar*) input.data,input.cols,input.rows,input.step,QImage::Format_RGB888);
        return q_img_output;
    }
}

QImage MainWindow::get_guassian(cv::Mat input){
    int kernel_size=ui->threshold_hsb->value();
    if (kernel_size%2== 0){
        kernel_size++;
    }
    cv::GaussianBlur( input, output_frame, cv::Size(kernel_size, kernel_size), 0, 0);
    QImage q_img_output ((uchar*) output_frame.data,output_frame.cols,output_frame.rows,output_frame.step,QImage::Format_RGB888);
    return q_img_output;
}

QImage MainWindow::get_median(cv::Mat input){
    int kernel_size=ui->threshold_hsb->value();
    if (kernel_size%2== 0){
        kernel_size++;
    }
    cv::medianBlur( input, output_frame, kernel_size);
    QImage q_img_output ((uchar*) output_frame.data,output_frame.cols,output_frame.rows,output_frame.step,QImage::Format_RGB888);
    return q_img_output;
}

QImage MainWindow::get_canny(cv::Mat input){
    cv::Mat detected_edges;

    int kernel_size=ui->k_size_hsb->value();
    if (kernel_size%2== 0 and kernel_size<8){
        kernel_size++;
    }else{
        kernel_size = 3;
    }

    cv::cvtColor( input, output_frame, CV_BGR2GRAY );

    /// Reduce noise with a kernel 3x3
    blur( output_frame, detected_edges, cv::Size(3,3) );
    auto lower_thres = ui->lower_thres_hsb->value();
    auto ratio = ui->ratio_hsb->value();
    /// Canny detector
    Canny( detected_edges, detected_edges, lower_thres, lower_thres*ratio, kernel_size );
    /// Using Canny's output as a mask, we display our result
    output_frame = cv::Scalar::all(0);

    input.copyTo( output_frame, detected_edges);
//    std::cout<<output_frame.rows<<" "<<output_frame.cols<<std::endl;
    QImage q_img_output ((uchar*) output_frame.data,output_frame.cols,output_frame.rows,output_frame.step,QImage::Format_RGB888);
    return q_img_output;
}

QImage MainWindow::get_hsv(cv::Mat input){
    auto low_H = ui->low_h_hsb->value();
    auto low_S = ui->low_s_hsb_2->value();
    auto low_V = ui->low_v_hsb->value();
    auto high_H = ui->high_h_hsb->value();
    auto high_S = ui->high_s_hsb->value();
    auto high_V = ui->high_v_hsb->value();
    // Convert from BGR to HSV colorspace
    cv::cvtColor(input, output_frame, cv::COLOR_BGR2HSV);
    // Detect the object based on HSV Range Values
    cv::inRange(output_frame, cv::Scalar(low_H, low_S, low_V), cv::Scalar(high_H, high_S, high_V), output_frame);
    QImage q_img_output ((uchar*) output_frame.data,output_frame.cols,output_frame.rows,output_frame.step,QImage::Format_Indexed8);
    return q_img_output;
}

void MainWindow::on_groupBox_clicked(bool checked)
{
    std::cout<<"clicked"<<std::endl;
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

void MainWindow::on_k_size_hsb_valueChanged(int value)
{
    ui->k_size_value_lbl->setText(QString::fromStdString(std::to_string(value)));
}

void MainWindow::on_ratio_hsb_valueChanged(int value)
{
    ui->ratio_value_lbl->setText(QString::fromStdString(std::to_string(value)));
}

void MainWindow::on_lower_thres_hsb_valueChanged(int value)
{
    ui->lower_thres_value_lbl->setText(QString::fromStdString(std::to_string(value)));
}

void MainWindow::on_threshold_hsb_valueChanged(int value)
{
    ui->threshold_value_lbl->setText(QString::fromStdString(std::to_string(value)));
}

void MainWindow::on_low_h_hsb_valueChanged(int value)
{
    ui->low_h_value_lbl->setText(QString::fromStdString(std::to_string(value)));
}

void MainWindow::on_high_h_hsb_valueChanged(int value)
{
    ui->high_h_value_lbl->setText(QString::fromStdString(std::to_string(value)));
}

void MainWindow::on_low_s_hsb_2_valueChanged(int value)
{
    ui->low_s_value_lbl->setText(QString::fromStdString(std::to_string(value)));
}

void MainWindow::on_high_s_hsb_valueChanged(int value)
{
    ui->high_s_value_lbl->setText(QString::fromStdString(std::to_string(value)));
}

void MainWindow::on_low_s_hsb_valueChanged(int value)
{
    ui->low_v_value_lbl->setText(QString::fromStdString(std::to_string(value)));
}

void MainWindow::on_high_v_hsb_valueChanged(int value)
{
    ui->low_v_value_lbl->setText(QString::fromStdString(std::to_string(value)));
}

void MainWindow::on_low_v_hsb_valueChanged(int value)
{
    ui->high_v_value_lbl->setText(QString::fromStdString(std::to_string(value)));
}

void MainWindow::on_histo_rb_clicked(bool checked)
{
    type = output_type::histogram;
    ui->sobel_gb->hide();
    ui->hsv_gb->hide();
    ui->binary_gb->hide();
}

void MainWindow::on_binary_rb_clicked(bool checked)
{
    type = output_type::binary_black_white;
    ui->sobel_gb->hide();
    ui->binary_gb->show();
    ui->hsv_gb->hide();
    ui->threshold_lbl->setText("Threshold:");
    ui->binary_gb->raise();
    ui->binary_gb->setTitle("Binary Parameter");
}

void MainWindow::on_gray_scale_rb_clicked(bool checked)
{
     type = output_type::gray_scale;
     ui->sobel_gb->hide();
     ui->binary_gb->hide();
     ui->canny_gb->hide();
     ui->hsv_gb->hide();

}

void MainWindow::on_sobel_rb_clicked(bool checked)
{
    type = output_type::sobel_operator;
    ui->sobel_gb->show();
    ui->canny_gb->hide();
    ui->binary_gb->hide();
    ui->sobel_gb->raise();
}

void MainWindow::on_normalize_filter_rb_clicked(bool checked)
{
    type = output_type::normalize_block_filter;
    ui->sobel_gb->hide();
    ui->canny_gb->hide();
    ui->hsv_gb->hide();
    ui->binary_gb->show();
    ui->binary_gb->setTitle("Parameters");

    ui->threshold_lbl->setText("Kernel Size:");

}

void MainWindow::on_gaussian_rb_clicked(bool checked)
{
    type = output_type::guassian_filter;
    ui->sobel_gb->hide();
    ui->canny_gb->hide();
    ui->hsv_gb->hide();
    ui->binary_gb->show();
    ui->threshold_lbl->setText("Kernel Size:");
    ui->binary_gb->setTitle("Parameters");
}

void MainWindow::on_median_rb_clicked(bool checked)
{
    type = output_type::median_filter;
    ui->sobel_gb->hide();
    ui->canny_gb->hide();
    ui->hsv_gb->hide();
    ui->binary_gb->show();
    ui->threshold_lbl->setText("Kernel Size:");
    ui->binary_gb->setTitle("Parameters");
}

void MainWindow::on_canny_rb_clicked(bool checked)
{
    type = output_type::canny;
    ui->sobel_gb->hide();
    ui->binary_gb->hide();
    ui->hsv_gb->hide();
    ui->canny_gb->show();
    ui->canny_gb->raise();
}

void MainWindow::on_hsv_rb_clicked(bool checked)
{
    type = output_type::hsv;
    ui->sobel_gb->hide();
    ui->binary_gb->hide();
    ui->canny_gb->hide();
    ui->hsv_gb->show();
    ui->hsv_gb->raise();
}


