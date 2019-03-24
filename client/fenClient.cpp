#include "fenClient.h"

fenClient::fenClient(QTcpSocket *socket, QString const &nomClient, QMainWindow *parent) : QDialog(parent)
{
    setupUi(this);

    fenConnexion = parent; // on pointe sur la fenetre parente
    pseudoClient = nomClient; // on sauvegarde le pseudo du client

    setWindowTitle(nomClient); // on met un nom a la boite de dialog

    // icons
    label_2->setPixmap(QPixmap("res/enligne.png"));
    boutonDeconnexion->setIcon(QIcon("res/deconnect.png"));
    boutonEmo->setIcon(QIcon("res/smile.png"));
    boutonCouleur->setIcon(QIcon("res/color.png"));
    boutonEnvoyer->setIcon(QIcon("res/envoyer.png"));
    tabWidget->setTabIcon(0, QIcon("res/public.png"));
    setWindowIcon(QIcon("res/chat.png"));

    socketClient = socket; // on récupère la socket sur laquelle on travaille

    // connexion du signal de fermeture de la dialogbox avec le slot de deconnexion
    connect(this, SIGNAL(rejected()), this, SLOT(deconnexion()));
    // connexion du signal de deconnexion de la socket avec la fermeture de la dialogbox
    connect(socketClient, SIGNAL(disconnected()), this, SLOT(accept()));
    connect(socketClient, SIGNAL(readyRead()), this, SLOT(donneesRecues()));

    listeMessages->append(tr("<em>Connexion au serveur réussie !</em>")); // on annonce qu'on est bien connecté
    messageEdit->setFocus();

    // ajout des boutons de zoom
    QWidget *conteneur = new QWidget(this);
    // (+)
    boutonZoomIn = new QPushButton;
    boutonZoomIn->setMaximumSize(32, 32);
    boutonZoomIn->setToolTip("Zoomer le texte");
    boutonZoomIn->setIcon(QIcon("res/plus.png"));
    boutonZoomIn->setIconSize(QSize(16, 16));
    boutonZoomIn->setAutoDefault(false);
    connect(boutonZoomIn, SIGNAL(clicked()), this, SLOT(zoomIn()));
    // (-)
    boutonZoomOut = new QPushButton;
    boutonZoomOut->setMaximumSize(32, 32);
    boutonZoomOut->setToolTip("Dézoomer le texte");
    boutonZoomOut->setIcon(QIcon("res/moin.png"));
    boutonZoomOut->setIconSize(QSize(16, 16));
    boutonZoomOut->setAutoDefault(false);
    connect(boutonZoomOut, SIGNAL(clicked()), this, SLOT(zoomOut()));
    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addWidget(boutonZoomOut);
    hLayout->addWidget(boutonZoomIn);
    hLayout->setMargin(0);
    hLayout->setSpacing(1);
    conteneur->setLayout(hLayout);
    //conteneur->setContentsMargins(0, 0, 0, 0);
    tabWidget->setCornerWidget(conteneur); // on les mets au coté du tabWidget
}

// Slots :

void fenClient::on_boutonDeconnexion_clicked()
{
    deconnexion();
    accept(); // on ferme la dialogbox
}

void fenClient::deconnexion()
{
    socketClient->disconnectFromHost(); // on déconnecte
    fenConnexion->statusBar()->showMessage(tr("Vous vous êtes deconnecté du serveur."), 2000);
}

void fenClient::on_messageEdit_returnPressed()
{
    on_boutonEnvoyer_clicked();
}

void fenClient::on_boutonEnvoyer_clicked() // Envoi de message au serveur
{
    // On prépare le paquet a envoyé
    QByteArray paquet;
    QDataStream out(&paquet, QIODevice::WriteOnly);

    QColor couleurText = messageEdit->palette().color(QPalette::Text);
    QString messageAEnvoyer = tr("<strong>") + pseudoClient + tr("</strong> : <") + QString::number(couleurText.red()) + tr(",") + QString::number(couleurText.green()) + tr(",") + QString::number(couleurText.blue()) + tr(">") + messageEdit->text();

    out << (quint16) 0; // 0 comme taille du paquet au début
    out << messageAEnvoyer; // ajout du message
    out.device()->seek(0); // on se replace au début du paquet
    out << (quint16) (paquet.size() - sizeof(quint16)); // on écrase le 0 par la taille réelle du paquet qu'on vien de calculer

    socketClient->write(paquet); // Envoi du paquet (qui contient le message biensur)

    messageEdit->clear(); // on vide la zone texte ou on ecrit les messages qu'on envoie
    messageEdit->setFocus(); // on lui remet le focus (le curseur)
}

void fenClient::on_boutonEmo_clicked() // si bouton emoticones clické
{
    fenEmoticones emoticonesBox(this);
    connect(&emoticonesBox, SIGNAL(emoticone(QString)), this, SLOT(addMessageEmoticone(QString)));
    emoticonesBox.exec();
}

void fenClient::on_boutonCouleur_clicked()
{
    QColor couleur = QColorDialog::getColor(messageEdit->palette().color(QPalette::Text), this); // dialogbox de choix de couleur

    QPalette palette;
    palette.setColor(QPalette::Text, couleur); // on configure une palette qui contiendra la couleur
    messageEdit->setPalette(palette); // on applique la couleur
    messageEdit->setFocus();
}

void fenClient::donneesRecues() // On a reçu un paquet (ou un sous-paquet)
{
    /* Mme principe que lorsque le serveur reçoit un paquet : On essaie de récupérer la taille du message
        Une fois qu'on l'a, on attend d'avoir reçu le message entier (en se basant sur la taille annonce tailleMessage) */

    QDataStream in(socketClient);
    if (tailleMessage == 0)
    {
        if (socketClient->bytesAvailable() < (int)sizeof(quint16))
            return;
    }

    in >> tailleMessage;
    if (socketClient->bytesAvailable() < tailleMessage)
        return;

    // Si on arrive jusqu'à cette ligne, on peut récupérer le message entier
    QString messageRecu;
    in >> messageRecu;

    if (!messageRecu.contains(':')) // si ce n'est pas un message de chat d'un client, alors :
    {
        if (messageRecu.right(26) == tr("vient de se connecter</em>")) // si un client vien de se connecter
        {
            QString nomExtrait = messageRecu.left(messageRecu.length() - 36); // -36 => - "</strong> vient de se connecter</em>"
            nomExtrait = nomExtrait.right(nomExtrait.length() - 12); // -12 => - "<em><strong>"
            // on l'ajoute a la liste des clients en ligne
            listeClients->addItem(nomExtrait);
            listeClients->item(listeClients->count() - 1)->setIcon(QIcon("res/user.png")); // on lui met une icone
            nombreClientsLabel->setText(tr("En ligne (") + QString::number(listeClients->count()) + tr(")")); // on actualise le nombre de clients en ligne
        }
        else if (messageRecu.right(28) == tr("vient de se deconnecter</em>")) // si nn s'il vient de se deconnecter
        {
            // on extrait son nom
            QString nomExtrait = messageRecu.left(messageRecu.length() - 38); // -38 => - "</strong> vient de se deconnecter</em>"
            nomExtrait = nomExtrait.right(nomExtrait.length() - 12); // -12 => - "<em><strong>"
            // on le cherche dans la liste des clients en ligne
            QListWidgetItem *widgetItem;
            int indexClient;
            for (indexClient = 0; indexClient < listeClients->count(); indexClient++)
            {
                widgetItem = listeClients->item(indexClient); // on récupère les widgetItem (qui contient le texte d'un élément de la liste)
                if (widgetItem->text() == nomExtrait) // si on a trouvé le client qui s'est deconnecté
                    break; // on arrête la boucle for
            }
            listeClients->takeItem(indexClient); // on l'enleve de la liste des clients en ligne
            nombreClientsLabel->setText(tr("En ligne (") + QString::number(listeClients->count()) + tr(")")); // on actualise le nombre de clients en ligne
        }
        else // si nn, c'est probablement la liste des clients en ligne que le serveur nous envoie
        {
            QString nomClientEnLigne;
            for (int i = 0; i < messageRecu.length(); i++)
            {
                if (messageRecu[i] != '|') // si on ne trouve pas de barrière '|', on récupère le nom du client (1 par 1)
                    nomClientEnLigne += messageRecu[i];
                else // barrière trouvée, on ajoute le nom a la liste des clients en ligne
                {
                    if (!nomClientEnLigne.isEmpty()) // si le nom n'est pas vide
                    {
                        listeClients->addItem(nomClientEnLigne); // on l'ajoute a la liste
                        listeClients->item(listeClients->count() - 1)->setIcon(QIcon("res/user.png")); // on lui met une icone
                        nomClientEnLigne.clear(); // on efface le contenu, pour récupré le nom suivant
                    }
                }
            }
            nombreClientsLabel->setText(tr("En ligne (") + QString::number(listeClients->count()) + tr(")")); // on actualise le nombre de clients en ligne
            tailleMessage = 0;
            return; // on arrête la méthode
        }
    }
    else if (messageRecu == tr("Err : Pseudo_duplicate")) // si doublant d'un pseudo trouvé
    {
        QMessageBox::warning(this, tr("Erreur"), tr("Ce pseudo est déjà utilisé !"));
        tailleMessage = 0;
        return;
    }
    // On affiche/ajoute le message sur la zone de Chat
    int indexNom = messageRecu.indexOf(':');
    if (indexNom != -1) // si c'est un message envoyé par un client
    {
        indexNom += 2; // +2 => ' ' + 1 (car les indexes commencent de la case 0)
        listeMessages->append(messageRecu.left(indexNom)); // nom du client
        // couleur du message
        int indexRed, indexGreen, indexBlue;
        QColor couleur;
        // rouge
        messageRecu = messageRecu.right((messageRecu.length())-indexNom-1); // message sans nom et '<'
        indexRed = messageRecu.indexOf(',');
        couleur.setRed(messageRecu.left(indexRed).toInt()); // niveau de couleur rouge
        // vert
        messageRecu = messageRecu.right((messageRecu.length())-indexRed-1); // message sans couleur rouge
        indexGreen = messageRecu.indexOf(',');
        couleur.setGreen(messageRecu.left(indexGreen).toInt()); // niveau de couleur verte
        // bleu
        messageRecu = messageRecu.right((messageRecu.length())-indexGreen-1); // message sans couleur verte
        indexBlue = messageRecu.indexOf('>');
        couleur.setBlue(messageRecu.left(indexBlue).toInt()); // niveau de couleur bleu
        // on applique la couleur
        listeMessages->setTextColor(couleur);
        // message
        messageRecu = messageRecu.right((messageRecu.length())-indexBlue-1); // message sans rien :p
        QList<QString> emo; // liste des emoticones
        emo << ":)" << ";)" << "<3" << ":/" << ":(" << ":g" << ":p" << ":D" << ";D" << "^^" << ":3" << ":'(";
        int nombreEmo = emo.size(), indexEmo[12]; // car c'est un tableau statique le nombre de case doit être constant
        bool emoticoneExist;
        do
        {
            emoticoneExist = false; // on suppose qu'aucune emoticone n'existe
            // on récupère les indexs des emoticones
            for (int i = 0; i < nombreEmo; i++)
            {
                indexEmo[i] = messageRecu.indexOf(emo[i]);
                if (indexEmo[i] != -1) // si une emoticone existe
                    emoticoneExist = true; // on le marque (true !)
            }

            for (int i = 0; i < nombreEmo; i++)
            {
                bool premiereEmo = true; // on suppose que c'est l'emoticone qui a le premier index (le plus petit)
                while (indexEmo[i] != -1 && premiereEmo) // tant que l'emoticone existe est que notre supposition est tj vrai
                {
                    for (int j = 0; j < nombreEmo; j++) // on vérifie la supposition (que l'emoticone a le 1er index)
                    {
                        if (indexEmo[j] != -1 && indexEmo[i] > indexEmo[j]) // si l'emoticone n'est pas en première position
                        {
                            premiereEmo = false; // bah nn, ce n'est pas la première emoticone
                            break; // on sort de la boucle for (j...)
                        }
                    }

                    if (premiereEmo) // si c bon, l'emotcione est en première place :p
                    {
                        listeMessages->insertPlainText(messageRecu.left(indexEmo[i])); // on ecrit le debut du message qui ne contient pas l'emoticone
                        listeMessages->textCursor().insertImage("emoticones/" + QString::number(i) + ".png"); // on insert l'emoticone
                        listeMessages->setTextColor(couleur); // on réapplique la couleur (car elle redevient par défaut)
                        messageRecu = messageRecu.right((messageRecu.length())-indexEmo[i]-emo[i].length()); // on tronque le message pour une autre recherche (-emo[i].length() par ex: ":)")
                        emoticoneExist = false; // on suppose qu'aucune emoticone n'existe
                        for (int j = 0; j < nombreEmo; j++) // on rénitiaslise tt les index (vue que le message a changer)
                        {
                            indexEmo[j] = messageRecu.indexOf(emo[j]);
                            if (indexEmo[j] != -1) // si une emoticone existe
                                emoticoneExist = true; // on le marque (true !)
                        }
                    }
                }
            }
         // tant qu'on trouve au moin une emoticone a afficher
        }while (emoticoneExist);

        listeMessages->insertPlainText(messageRecu); // il faut s'assurer qu'il n'y a pas d'interference (selection sourie/clavier)
        listeMessages->moveCursor(QTextCursor::End); // on déplace le cursor à la fin pour déplacer la barre vertical aussi
    }
    else
    {
        listeMessages->setTextColor(Qt::black); // couleur par défaut
        listeMessages->append(messageRecu); // on affiche le message tel qu'il est
    }

    // On remet la taille du message à 0 pour pouvoir recevoir de futurs messages
    tailleMessage = 0;
}

void fenClient::addMessageEmoticone(QString const& emoticone)
{
    messageEdit->insert(emoticone); // on insert l'emoticone a la fin du texte
    messageEdit->setFocus();
}

void fenClient::zoomIn()
{
    listeMessages->zoomIn();
}

void fenClient::zoomOut()
{
    listeMessages->zoomOut();
}
