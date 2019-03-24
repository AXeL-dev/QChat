#include <QApplication>
#include "fenPrincipale.h"

int main (int argc, char *argv[])
{
    QApplication app(argc, argv);

    fenPrincipale fenetre;
    fenetre.show();

    return app.exec();
}
