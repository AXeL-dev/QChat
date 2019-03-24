#include "fenPrincipale.h"

fenPrincipale::fenPrincipale()
{
    setupUi(this);

    socket = new QTcpSocket(this); // on d�marre le client (la socket qui permettra de communiquer avec le serveur)

    setWindowTitle(tr("QChat - Client"));
    setWindowIcon(QIcon("res/chat.png"));

    // connexion des actions du menu aux slots
    connect(actionConnexion, SIGNAL(triggered()), this, SLOT(on_boutonConnexion_clicked()));
    connect(actionQuitter, SIGNAL(triggered()), qApp, SLOT(quit()));
    connect(actionApropos, SIGNAL(triggered()), this, SLOT(aPropos()));
    // connexion des signaux (appartenant aux attributs)
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(erreurSocket(QAbstractSocket::SocketError)));
    connect(socket, SIGNAL(connected()), this, SLOT(connecte()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(deconnecte()));
    connect(pseudoEdit, SIGNAL(returnPressed()), this, SLOT(on_boutonConnexion_clicked()));
    connect(ipServeurEdit, SIGNAL(returnPressed()), this, SLOT(on_boutonConnexion_clicked()));

    // icons
    label->setPixmap(QPixmap("res/chat.png"));
    boutonConnexion->setIcon(QIcon("res/connect.png"));
    actionConnexion->setIcon(QIcon("res/connect.png"));
}

// Slots :

void fenPrincipale::on_boutonConnexion_clicked()
{
    if (pseudoEdit->text().isEmpty()) // si le client n'a pas sp�cifi� de pseudoname
    {
        QMessageBox::warning(this, tr("Erreur"), tr("Veuillez saisir un pseudo !"));
        pseudoEdit->setFocus();
        return; // on quitte/arr�te la m�thode
    }
    else if (pseudoEdit->text().contains('|') || pseudoEdit->text().contains(':') || pseudoEdit->text().contains('<') || pseudoEdit->text().contains('>')) // si le pseudo contient un caract�re interdit (car si on autorise �a va foir� la liste des clients en ligne, ...)
    {
        QMessageBox::warning(this, tr("Erreur"), tr("Il est interdit d'utiliser les caract�res\n'|', ':', '<', '>' dans le pseudo !"));
        pseudoEdit->setText(pseudoEdit->text().remove('|')); // on enl�ve le caract�re interdit '|'
        pseudoEdit->setText(pseudoEdit->text().remove(':')); // de meme pour ':' et les autres
        pseudoEdit->setText(pseudoEdit->text().remove('<'));
        pseudoEdit->setText(pseudoEdit->text().remove('>'));
        pseudoEdit->setFocus();
        return;
    }

    statusBar()->showMessage(tr("Tentative de connexion en cours..."), 2000);
    boutonConnexion->setEnabled(false); // on d�sactive le bouton de connexion

    socket->abort(); // on d�sactive les connexions pr�c�dentes s'il y'en a
    socket->connectToHost(ipServeurEdit->text(), portSpinBox->value()); // on se connecte au serveur
}

void fenPrincipale::aPropos()
{
    QMessageBox::information(this, tr("A propos"), tr("<strong>QChat</strong> maded by <strong>AXeL</strong> ;)<br />Thanks to <a href=\"http://www.openclassrooms.com\">OpenClassrooms</a>."));
}

void fenPrincipale::erreurSocket(QAbstractSocket::SocketError erreur) // ce slot est appel� lorsqu'il y'a une erreur
{
    switch(erreur) // on affiche un message diff�rent selon l'erreur qu'on nous indique
    {
        case QAbstractSocket::HostNotFoundError:
            statusBar()->showMessage(tr("Erreur : le serveur est introuvable."), 2000);
            break;
        case QAbstractSocket::ConnectionRefusedError:
            statusBar()->showMessage(tr("Erreur : le serveur a refus� la connexion."), 2000);
            break;
        case QAbstractSocket::RemoteHostClosedError:
            statusBar()->showMessage(tr("Erreur : le serveur a coup� la connexion."), 2000);
            break;
        default: // autres erreurs
            statusBar()->showMessage(tr("Erreur : ") + socket->errorString(), 2000);
    }

    boutonConnexion->setEnabled(true); // On r�active le bouton Connexion
}

void fenPrincipale::connecte() // ce slot est appell� lorsque la connexion au serveur a r�ussi
{
    identification(); // on envoie un message d'identification

    fenClient fenetreClient(socket, pseudoEdit->text(), this); // cr�ation de la fen�tre du client
    fenetreClient.exec(); // affichage/�xecution
}

void fenPrincipale::deconnecte() // ce slot est appell� lorsque la connexion au serveur est perdu
{
    //statusBar()->showMessage(tr("Vous �tes deconnect� du serveur."), 2000);
    boutonConnexion->setEnabled(true); // On r�active le bouton Connexion
}

// M�thodes :

void fenPrincipale::identification() // Envoi d'un message d'identification au serveur
{
    // On pr�pare le paquet a envoy�
    QByteArray paquet;
    QDataStream out(&paquet, QIODevice::WriteOnly);

    QString messageIdentification = tr("<em><strong>") + pseudoEdit->text() + tr("</strong> vient de se connecter</em>");

    out << (quint16) 0; // 0 comme taille du paquet au d�but
    out << messageIdentification; // ajout du message
    out.device()->seek(0); // on se replace au d�but du paquet
    out << (quint16) (paquet.size() - sizeof(quint16)); // on �crase le 0 par la taille r�elle du paquet qu'on vien de calculer

    socket->write(paquet); // Envoi du paquet (qui contient le message d'identification)
}
