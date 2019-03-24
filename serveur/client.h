#ifndef CLIENT_H
#define CLIENT_H

#include <QtNetwork>

class client
{
    public:
        // Constr.
        client();

        // Accesseurs/propriétés
        QTcpSocket* getSocket() const;
        void setSocket(QTcpSocket* socketClient);
        QString getNomClient() const;
        void setNomClient(const QString &nom);
        quint16 getTailleMessage() const;
        void setTailleMessage(QDataStream &in);
        void setIntTailleMessage(const quint16 &taille);

    private:
        // Attributs
        QTcpSocket *socket;
        QString nomClient;
        quint16 tailleMessage;
};

#endif // CLIENT_H
