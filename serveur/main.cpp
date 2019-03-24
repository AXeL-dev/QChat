#include <QApplication>
#include "fenServeur.h"

int main (int argc, char *argv[])
{
    QApplication app(argc, argv);

    fenServeur fenetre;
    fenetre.show();

    return app.exec();
}
