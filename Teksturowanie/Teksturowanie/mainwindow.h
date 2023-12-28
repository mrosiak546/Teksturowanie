#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QQueue>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QImage im_source, im_destination;
    QPoint startPos, endPos;
    bool im_source_clicked=false, im_destination_clicked=false;
    int iSourcePoints=-1, iDestinationPoints=-1;
    QQueue<QPoint> sourcePoints, destinationPoints;

    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void drawPixel(QImage *im, QPoint p, QColor color);
    void drawLine(QImage *im, QPoint start, QPoint end, QColor color);
    void drawCircle(QImage *im, QPoint start, QPoint end, QColor color);
    void draw();
    void drawTriangle(QImage *im, QQueue<QPoint> points);
    void fill();
    int findPoints(QPoint p, QQueue<QPoint> points);

    //void fill(QImage *im, QQueue list);

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
