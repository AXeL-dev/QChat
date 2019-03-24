#ifndef FENSERVEUR_H
#define FENSERVEUR_H

#include <QtGui>
#include <QtNetwork>
#include "client.h"

class fenServeur : public QWidget
{
    Q_OBJECT

    public:
        // Constr.
        fenServeur();
        // Méthodes
        void envoyerATous(const QString &message);
        void envoyerA(client *monClient, const QString &message);

    private slots:
        void nouvelleConnexion();
        void donneesRecues();
        void deconnexionClient();
        void deconnecterClient();
        void arreterServeur();

    private:
        // Attributs
        QLabel *etatServeur, *nombreClients;
        QPushButton *boutonQuitter;
        QPushButton *boutonDeconnecterClient;
        QPushButton *boutonArreter;

        QTcpServer *serveur;
        //QList<QTcpSocket *> clients;
        QList<client *> clients;
        QTextEdit *historique;
        //quint16 tailleMessage;

        QListWidget *vue;
};

#endif // FENSERVEUR_H
