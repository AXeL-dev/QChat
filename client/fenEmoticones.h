#ifndef FENEMOTICONES_H
#define FENEMOTICONES_H

#include <QDialog>
#include "ui_fenemoticones.h"

class fenEmoticones : public QDialog, private Ui::fenemoticones
{
    Q_OBJECT

    public:
        //Constr.
        fenEmoticones(QDialog *parent);

    signals:
        void emoticone(QString const& emot);

    private slots:
        void on_emo0_clicked();
        void on_emo1_clicked();
        void on_emo2_clicked();
        void on_emo3_clicked();
        void on_emo4_clicked();
        void on_emo5_clicked();
        void on_emo6_clicked();
        void on_emo7_clicked();
        void on_emo8_clicked();
        void on_emo9_clicked();
        void on_emo10_clicked();
        void on_emo11_clicked();

    private:
        //Attributs
        // Rappel toi qu'on ne peut pas créer un objet ussu d'une classe mère içi dans les attributs, exemple : fenClient *fen; // est faux
};

#endif // FENEMOTICONES_H
