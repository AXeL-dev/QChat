#include "client.h"

client::client()
{
     tailleMessage = 0; // initialisation
}

// Accesseurs

QTcpSocket* client::getSocket() const
{
    return socket;
}

void client::setSocket(QTcpSocket* socketClient)
{
    socket = socketClient;
}

QString client::getNomClient() const
{
    return nomClient;
}

void client::setNomClient(const QString &nom)
{
    nomClient = nom;
}

quint16 client::getTailleMessage() const
{
    return tailleMessage;
}

void client::setTailleMessage(QDataStream &in)
{
    in >> tailleMessage;
}

void client::setIntTailleMessage(const quint16 &taille)
{
    tailleMessage = taille;
}
