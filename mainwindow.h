#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QPushButton>

#include <openssl/aes.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

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


    QVector<QByteArray> encryptedCodes;
    QVector<QPushButton*> promoCard;
    QByteArray encryptionKey;

    void initializePromoCodes();
    void appendNewPromoCode();
    QString createRandomCode();


    QByteArray encrypter(const QString &promoCode);
    QString decrypter(const QByteArray &encryptedCode);
    QByteArray generateKey(const QString &pin);
};
#endif // MAINWINDOW_H
