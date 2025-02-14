#include "newgame.hpp"

#include <QGuiApplication>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QScreen>

#include "filewidget.hpp"
#include "adjusterwidget.hpp"

CSVDoc::NewGameDialogue::NewGameDialogue()
{
    setWindowTitle ("Create New Game");

    QVBoxLayout *layout = new QVBoxLayout (this);

    mFileWidget = new FileWidget (this);
    mFileWidget->setType (false);

    layout->addWidget (mFileWidget, 1);

    mAdjusterWidget = new AdjusterWidget (this);

    layout->addWidget (mAdjusterWidget, 1);

    QDialogButtonBox *buttons = new QDialogButtonBox (this);

    mCreate = new QPushButton ("Create", this);
    mCreate->setDefault (true);
    mCreate->setEnabled (false);

    buttons->addButton (mCreate, QDialogButtonBox::AcceptRole);

    QPushButton *cancel = new QPushButton ("Cancel", this);

    buttons->addButton (cancel, QDialogButtonBox::RejectRole);

    layout->addWidget (buttons);

    setLayout (layout);

    connect (mAdjusterWidget, &AdjusterWidget::stateChanged, this, &NewGameDialogue::stateChanged);
    connect (mCreate, &QPushButton::clicked, this, &NewGameDialogue::create);
    connect (cancel, &QPushButton::clicked, this, &NewGameDialogue::reject);
    connect (mFileWidget, &FileWidget::nameChanged, mAdjusterWidget, &AdjusterWidget::setName);

    QRect scr = QGuiApplication::primaryScreen()->geometry();
    QRect rect = geometry();
    move (scr.center().x() - rect.center().x(), scr.center().y() - rect.center().y());
}

void CSVDoc::NewGameDialogue::setLocalData (const boost::filesystem::path& localData)
{
    mAdjusterWidget->setLocalData (localData);
}

void CSVDoc::NewGameDialogue::stateChanged (bool valid)
{
    mCreate->setEnabled (valid);
}

void CSVDoc::NewGameDialogue::create()
{
    emit createRequest (mAdjusterWidget->getPath());
}

void CSVDoc::NewGameDialogue::reject()
{
    emit cancelCreateGame ();
    QDialog::reject();
}
