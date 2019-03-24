#ifndef FENCLIENT_H
#define FENCLIENT_H

#include <QtGui>
#include <QtNetwork>
#include "ui_fenclient.h"
#include "fenEmoticones.h"

class fenClient : public QDialog, private Ui::fenclient
{
    Q_OBJECT

    public:
        //Constr.
        fenClient(QTcpSocket *socket, QString const &nomClient, QMainWindow *parent);

    private slots:
        void on_boutonDeconnexion_clicked();
        void deconnexion();
        void on_messageEdit_returnPressed();
        void on_boutonEnvoyer_clicked();
        void on_boutonEmo_clicked();
        void on_boutonCouleur_clicked();
        void donneesRecues();
        void addMessageEmoticone(QString const& emoticone);
        void zoomIn();
        void zoomOut();

    private:
        //Attirbuts
        QTcpSocket *socketClient;
        QString pseudoClient;
        quint16 tailleMessage; // taille du message(s) qu'on va recevoir
        QMainWindow *fenConnexion;
        //QCommandLinkButton *boutonZoomIn;
        //QCommandLinkButton *boutonZoomOut;
        QPushButton *boutonZoomIn;
        QPushButton *boutonZoomOut;
};

#endif // FENCLIENT_H
