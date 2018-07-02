#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <opencv2/opencv.hpp>
#include <QTimer>

enum output_type{
    gray_scale,
    sobel_operator,
    histogram,
    binary_black_white,
    normalize_block_filter,
    guassian_filter,
    median_filter
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    cv::VideoCapture capture;
    cv::Mat origial_frame;
    cv::Mat output_frame;

    QImage original_img;
    QImage output_img;

    QTimer *capture_timer;

    output_type type=output_type::gray_scale;

    ///Opertaors
    QImage get_gray_scale(cv::Mat input);
    QImage get_sobel_operator(cv::Mat input);
    QImage get_binary(cv::Mat input);
    QImage get_histogram(cv::Mat input);
    QImage get_median(cv::Mat input);
    QImage get_guassian(cv::Mat input);
    QImage get_block_normalize(cv::Mat input);


public slots:
    void get_new_frame();

private slots:
    void on_groupBox_clicked(bool checked);
    void on_gray_scale_rb_clicked(bool checked);
    void on_sobel_rb_clicked(bool checked);
    void on_scale_hsb_rangeChanged(int min, int max);
    void on_delta_hsb_valueChanged(int value);
    void on_scale_hsb_valueChanged(int value);
    void on_binary_rb_clicked(bool checked);
    void on_threshold_hsb_valueChanged(int value);
    void on_histo_rb_clicked(bool checked);
    void on_normalize_filter_rb_clicked(bool checked);
    void on_gaussian_rb_clicked(bool checked);
    void on_median_rb_clicked(bool checked);
};

#endif // MAINWINDOW_H
