/*
 * Bon, il reste plein de choses a ajouter, moi j'en ai assez ^^, c'est un peu chiant
 * 1- on peut sauvegarder l'historique des connexions et des messages "le traffic !" (depuis le serveur biensur)
 * 2- ajouter les conversations privés...
 * 3- crypter le traffic, envoyer des messages depuis le serveur, bannir des clients, etc...
 * 4- autoriser l'envoi de fichier sur le réseau (le tout étant de récupérer le fichier à envoyer sous forme de QByteArray).
 * 5- selectionner plusieurs clients et les déconnecter.
 * 6- pour récuperer l'adresse ip du client, mais le serveur se ferme après ché pas pq :
 *    //QHostAddress adresse = clients[i]->getSocket()->localAddress();
 *    //QHostAddress adresse = clients[i]->getSocket()->peerAddress();
 *    //QMessageBox::information(this, "adresse ip", adresse.toString());
 *
 *
 * Date de la dernière modification : 21/03/2014
 *
 */
#include "fenServeur.h"

fenServeur::fenServeur()
{
    // Création et disposition des widgets de la fenêtre
    etatServeur = new QLabel;
    nombreClients = new QLabel;
    boutonQuitter = new QPushButton(tr("Quitter"));
    connect(boutonQuitter, SIGNAL(clicked()), qApp, SLOT(quit()));
    boutonDeconnecterClient = new QPushButton(tr("Deconnecter le client"));
    boutonDeconnecterClient->setMaximumWidth(190);
    connect(boutonDeconnecterClient, SIGNAL(clicked()), this, SLOT(deconnecterClient()));
    boutonArreter = new QPushButton(tr("Arrêter le serveur"));
    connect(boutonArreter, SIGNAL(clicked()), this, SLOT(arreterServeur()));
    vue = new QListWidget;
    vue->setMaximumWidth(190);
    historique = new QTextEdit;
    historique->setReadOnly(true);

    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addWidget(vue);
    hLayout->addWidget(historique);
    QHBoxLayout *hLayout2 = new QHBoxLayout;
    hLayout2->addWidget(boutonDeconnecterClient);
    hLayout2->addWidget(boutonArreter);
    hLayout2->addWidget(boutonQuitter);
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(etatServeur);
    layout->addWidget(nombreClients);
    layout->addLayout(hLayout);
    layout->addLayout(hLayout2);

    // Zone fenêtre
    setLayout(layout);
    setWindowTitle(tr("QChat - Serveur"));
    setWindowIcon(QIcon("server.png"));
    resize(400, 380);

    // Gestion du serveur
    serveur = new QTcpServer(this); // this sert a détruire le serveur si fenêtre fermé
    if (!serveur->listen(QHostAddress::Any, 50885)) // Démarrage du serveur sur toutes les ip disponibles et sur le port 50885
    {
        // Si le serveur n'a pas pu démarrer correctement
        etatServeur->setText(tr("Le serveur n'a pas pu etre demarré. Raison :<br />") + serveur->errorString());
    }
    else // le serveur a été démarré correctement
    {
        etatServeur->setText(tr("Le serveur a été démarré sur le port <strong>") + QString::number(serveur->serverPort()) + tr("</strong>. Des clients peuvent maintenant se connecter."));
        nombreClients->setText(tr("Il y'a <strong>") + QString::number(clients.size()) + tr("</strong> client(s) en ligne :"));
        connect(serveur, SIGNAL(newConnection()), this, SLOT(nouvelleConnexion()));
    }

     //tailleMessage = 0; // initialisation
}

// Slots :

void fenServeur::nouvelleConnexion()
{
    //envoyerATous(tr("<em>Un nouveau client vient de se connecter</em>"));

    QTcpSocket *nouveauClient = serveur->nextPendingConnection(); // Cette méthode retourne la QTcpSocket (le/la socket) du nouveau client
    // on l'ajoute a la liste des clients
    //clients << nouveauClient;
    client *nouveau = new client; // on crée un pointeur vers un objet de type client
    nouveau->setSocket(nouveauClient); // on le configure :p
    clients << nouveau; // on passe son adresse pour le stocker dans la liste des clients
    nombreClients->setText(tr("Il y'a <strong>") + QString::number(clients.size()) + tr("</strong> client(s) en ligne :")); // on actualise le nombre de client sur la fenêtre du serveur

    connect(nouveauClient, SIGNAL(readyRead()), this, SLOT(donneesRecues())); // signale que le client a envoyé des données
    connect(nouveauClient, SIGNAL(disconnected()), this, SLOT(deconnexionClient()));
}

void fenServeur::donneesRecues()
{
    // Recherche du QTcpSocket du client (celui qui a envoyé le message)
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    if (socket == 0) // si le client n'a pa été déterminé
        return; // on arrête la diffusion du message

    // Si tt va bien, on récupère le message
    QDataStream in(socket);

    int i;
    for (i = 0; i < clients.size(); i++) // on cherche le client depuis son/sa socket
    {
        if (clients[i]->getSocket() == socket) // le client a été trouvé
            break; // on arrête la boucle
    }

    //QMessageBox::information(this, "Le client suivant vien d'envoyer un message :", clients[i]->getNomClient()); // a la connexion ce message sera vide (car le nom du client se définit après son identification/son premier message)

    if (clients[i]->getTailleMessage() == 0) // Si la taille du message est inconnue
    {
        if (socket->bytesAvailable() < (int)sizeof(quint16)) // on n'a pa reçu la taille du message entier
            return; // stop, on attend les autres sous paquets

        // si on a reçu la taille du message entier, on la récupère
        //in >> tailleMessage;
        clients[i]->setTailleMessage(in);
    }

    // Maintenant on connait la taille du message et tt, on vérifie si on a reçu le message en entier
    if (socket->bytesAvailable() < clients[i]->getTailleMessage()) // si on n'a pas tt reçu
        return; // on arrête la méthode

    // Si on est la, c'est qu'on a reçu le message en entier, on le récupère
    QString message;
    in >> message;

    // si le client vient de se connecter
    if (message.right(26) == tr("vient de se connecter</em>"))
    {
        // on va extraire son nom depuis le message d'identification qu'il a envoyé
        QString nomExtrait = message.left(message.length() - 36); // -36 => - "</strong> vient de se connecter</em>"
        nomExtrait = nomExtrait.right(nomExtrait.length() - 12); // -12 => - "<em><strong>"
        // on vérifie que le pseudoname n'est pas utilisé actuellement
        for (int j = 0; j < clients.size(); j++)
        {
            if (clients[j]->getNomClient() == nomExtrait) // si le pseudoname existe
            {
                envoyerA(clients[i], "Err : Pseudo_duplicate");
                clients[i]->getSocket()->disconnectFromHost(); // pour le reste, le slot deconnexionClient() va s'en occuper
                return; // on arrête la méthode
            }
        }
        // on enregistre son nom
        clients[i]->setNomClient(nomExtrait);
        // on l'affiche sur la fenêtre du serveur
        vue->addItem(clients[i]->getNomClient());
        vue->item(i)->setIcon(QIcon("enligne.png"));
        // Envoi de la liste des clients en ligne au client qui vien de se connecter
        QString clientsEnLigne;
        for (int j = 0; j < clients.size(); j++) // chargement de la liste
        {
            clientsEnLigne += clients[j]->getNomClient() + "|";
        }

        for (int j = 0; j < clients.size(); j++) // envoi de la liste
        {
            if (j == i) // si c'est le client qui vien de se connecter, on lui envoie les clients en ligne
                envoyerA(clients[i], clientsEnLigne);
            else // si nn on envoie aux autres clients que "le client vient de se connecter"
                envoyerA(clients[j], message);
        }

        clients[i]->setIntTailleMessage(0); // remise a zéro pour permettre de futurs récéptions
        return; // on arrête la méthode
    }
    // on renvoie le message a tt les clients
    envoyerATous(message);

    // Remise de la taille du message a 0, pour permettre la récéption de futurs messages
    //tailleMessage = 0;
    clients[i]->setIntTailleMessage(0);
}

void fenServeur::deconnexionClient()
{
    // On annonce a tt le monde que le client s'est deconnecté
    //envoyerATous(tr("<em> vient de se deconnecter</em>"));

    // On détermine quel client se déconnecte
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    if (socket == 0) // si par hasard on ne trouve pas ce client, on arrête la méthode
        return;

    // on enlève le client de la liste des clients
    //clients.removeOne(socket);
    for (int i = 0; i < clients.size(); i++) // on cherche le client depuis son/sa socket
    {
        if (clients[i]->getSocket() == socket)
        {
            QString nomClient = clients[i]->getNomClient();
            if (!nomClient.isEmpty()) // si le client c'est déjà bien identifié (si on a son nom)
            {
                // une fois trouvé on annonce a tt le monde que le client s'est deconnecté
                envoyerATous(tr("<em><strong>") + nomClient + tr("</strong> vient de se deconnecter</em>"));
                vue->takeItem(i); // on l'enlève de la liste d'affichage sur la fenêtre du serveur
            }
            clients.removeOne(clients[i]); // on enlève ses données (du client)
            break; // on arrête la boucle for (car on a trouvé le client qui s'est deconnecté)
        }
    }
    nombreClients->setText(tr("Il y'a <strong>") + QString::number(clients.size()) + tr("</strong> client(s) en ligne :")); // on actualise le nombre de client sur la fenêtre du serveur

    socket->deleteLater(); // on supprimera plustard l'objet (la socket du client)
}

void fenServeur::deconnecterClient()
{
    if (vue->count() > 0) // s'il y'a des clients en lignes
    {
        int curRow = vue->currentRow(); // retourne -1 si rien n'est séléctionné
        if (curRow >= 0) // si un client a été sélectionné
        {
            clients[curRow]->getSocket()->disconnectFromHost(); // on le déconnecte
            // le slot, deconnexionClient va s'occuper du reste
        }
        else
            QMessageBox::warning(this, tr("Erreur"), tr("Veuillez selectionner un client !"));
    }
    else
        QMessageBox::critical(this, tr("Erreur"), tr("Aucun client n'est en ligne !"));
}

// Méthodes :

void fenServeur::envoyerATous(const QString &message)
{
    // Préparation du paquet
    QByteArray paquet;
    QDataStream out(&paquet, QIODevice::WriteOnly);

    out << (quint16) 0; // on ecrit 0 au début du paquet pour réserver la place pour ecrire la taille
    out << message; // on y ajoute le message
    out.device()->seek(0); // on se replace au début du paquet
    out << (quint16) (paquet.size() - sizeof(quint16)); // on écrase le 0 qu'on avait réservé par la longueur du message

    // Envoi du paquet a ts les clients connectés au serveur
    for (int i = 0; i < clients.size(); i++)
    {
        //clients[i]->write(paquet);
        clients[i]->getSocket()->write(paquet);
    }

    historique->append(QDate::currentDate().toString() + " : " + message); // on affiche le message sur la fenêtre du serveur
    historique->moveCursor(QTextCursor::End);
    // il reste a afficher la couleur du texte aussi
}

void fenServeur::envoyerA(client *monClient, const QString &message)
{
    // Préparation du paquet
    QByteArray paquet;
    QDataStream out(&paquet, QIODevice::WriteOnly);

    out << (quint16) 0; // on ecrit 0 au début du paquet pour réserver la place pour ecrire la taille
    out << message; // on y ajoute le message
    out.device()->seek(0); // on se replace au début du paquet
    out << (quint16) (paquet.size() - sizeof(quint16)); // on écrase le 0 qu'on avait réservé par la longueur du message

    // Envoi du paquet au client
    monClient->getSocket()->write(paquet);

    historique->append(QDate::currentDate().toString() + " : " + message); // on affiche le message sur la fenêtre du serveur
    historique->moveCursor(QTextCursor::End);
}

void fenServeur::arreterServeur()
{
    delete serveur; // on detruit le serveur, on ne peut plus l'utiliser
    boutonArreter->setEnabled(false); // on désactive le bouton arreter
    boutonDeconnecterClient->setEnabled(false); // on désactive le bouton de déconnexion de client aussi
}
