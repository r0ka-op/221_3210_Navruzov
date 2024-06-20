#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QPushButton>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void authenticateUser();
    void revealPromoCode();

private:
    Ui::MainWindow *ui;

    QVector<QPushButton*> promoCard;
    QVector<QString> promoCodeArray;

    void initializePromoCodes();
    void appendNewPromoCode();
    QString createRandomCode();
};
#endif // MAINWINDOW_H
