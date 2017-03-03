#ifndef GARBAGEAPP_H
#define GARBAGEAPP_H

#include <QMainWindow>
#include <QBasicTimer>

namespace Ui {
    class garbageApp;
}

class garbageApp : public QMainWindow
{
    Q_OBJECT

public:
    explicit garbageApp(QWidget *parent = 0);
    ~garbageApp();

private slots:
    void on_timerInit_clicked();

    void on_simInit_clicked();

private:
    Ui::garbageApp *ui;
    int timerId;
    int simMain();

protected:
    void timerEvent(QTimerEvent *event);
};

#endif // GARBAGEAPP_H
