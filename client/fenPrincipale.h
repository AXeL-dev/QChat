#ifndef FENPRINCIPALE_H
#define FENPRINCIPALE_H

#include <QMainWindow>
#include <QMessageBox>
#include <QtNetwork>
#include "ui_fenprincipale.h"
#include "fenClient.h"

class fenPrincipale : public QMainWindow, private Ui::fenprincipale
{
    Q_OBJECT

    public:
        //Constr.
        fenPrincipale();

    private slots:
        void on_boutonConnexion_clicked(); // slot en connexion automatique avec le signal clicked du boutonConnexion
        void aPropos();
        void erreurSocket(QAbstractSocket::SocketError erreur);
        void connecte();
        void deconnecte();

    private:
        //Attributs
        QTcpSocket *socket; // Représente la connexion au serveur (la socket du client qui se connectera au serveur)

        //Méthodes
        void identification();
};

#endif // FENPRINCIPALE_H
