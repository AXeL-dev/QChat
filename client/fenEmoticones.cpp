#include "fenEmoticones.h"

fenEmoticones::fenEmoticones(QDialog *parent) : QDialog(parent)
{
    setupUi(this);

    setWindowFlags(Qt::Tool); // style de la fenêtre
    //setWindowTitle("Emoticones"); // fait dans Qt designer

    // icons
    emo0->setIcon(QIcon("emoticones/0.png"));
    emo1->setIcon(QIcon("emoticones/1.png"));
    emo2->setIcon(QIcon("emoticones/2.png"));
    emo3->setIcon(QIcon("emoticones/3.png"));
    emo4->setIcon(QIcon("emoticones/4.png"));
    emo5->setIcon(QIcon("emoticones/5.png"));
    emo6->setIcon(QIcon("emoticones/6.png"));
    emo7->setIcon(QIcon("emoticones/7.png"));
    emo8->setIcon(QIcon("emoticones/8.png"));
    emo9->setIcon(QIcon("emoticones/9.png"));
    emo10->setIcon(QIcon("emoticones/10.png"));
    emo11->setIcon(QIcon("emoticones/11.png"));
    setWindowIcon(QIcon("res/smile.png"));
}

// Slots :

void fenEmoticones::on_emo0_clicked()
{
    emit emoticone(":)");
    accept();
}

void fenEmoticones::on_emo1_clicked()
{
    emit emoticone(";)");
    accept();
}

void fenEmoticones::on_emo2_clicked()
{
    emit emoticone("<3");
    accept();
}

void fenEmoticones::on_emo3_clicked()
{
    emit emoticone(":/");
    accept();
}

void fenEmoticones::on_emo4_clicked()
{
    emit emoticone(":(");
    accept();
}

void fenEmoticones::on_emo5_clicked()
{
    emit emoticone(":g");
    accept();
}

void fenEmoticones::on_emo6_clicked()
{
    emit emoticone(":p");
    accept();
}

void fenEmoticones::on_emo7_clicked()
{
    emit emoticone(":D");
    accept();
}

void fenEmoticones::on_emo8_clicked()
{
    emit emoticone(";D");
    accept();
}

void fenEmoticones::on_emo9_clicked()
{
    emit emoticone("^^");
    accept();
}

void fenEmoticones::on_emo10_clicked()
{
    emit emoticone(":3");
    accept();
}

void fenEmoticones::on_emo11_clicked()
{
    emit emoticone(":'(");
    accept();
}
