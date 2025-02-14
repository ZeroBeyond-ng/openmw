#include "globaldebugprofilemenu.hpp"

#include <vector>
#include <algorithm>

#include <QActionGroup>

#include "../../model/world/idtable.hpp"
#include "../../model/world/record.hpp"

void CSVDoc::GlobalDebugProfileMenu::rebuild()
{
    clear();

    delete mActions;
    mActions = nullptr;

    int idColumn = mDebugProfiles->findColumnIndex (CSMWorld::Columns::ColumnId_Id);
    int stateColumn = mDebugProfiles->findColumnIndex (CSMWorld::Columns::ColumnId_Modification);
    int globalColumn = mDebugProfiles->findColumnIndex (
        CSMWorld::Columns::ColumnId_GlobalProfile);

    int size = mDebugProfiles->rowCount();

    std::vector<QString> ids;

    for (int i=0; i<size; ++i)
    {
        int state = mDebugProfiles->data (mDebugProfiles->index (i, stateColumn)).toInt();

        bool global = mDebugProfiles->data (mDebugProfiles->index (i, globalColumn)).toInt();

        if (state!=CSMWorld::RecordBase::State_Deleted && global)
            ids.push_back (
                mDebugProfiles->data (mDebugProfiles->index (i, idColumn)).toString());
    }

    mActions = new QActionGroup (this);
    connect (mActions, &QActionGroup::triggered, this, &GlobalDebugProfileMenu::actionTriggered);

    std::sort (ids.begin(), ids.end());

    for (std::vector<QString>::const_iterator iter (ids.begin()); iter!=ids.end(); ++iter)
    {
        mActions->addAction (addAction (*iter));
    }
}

CSVDoc::GlobalDebugProfileMenu::GlobalDebugProfileMenu (CSMWorld::IdTable *debugProfiles,
    QWidget *parent)
: QMenu (parent), mDebugProfiles (debugProfiles), mActions (nullptr)
{
    rebuild();

    connect (mDebugProfiles, &CSMWorld::IdTable::rowsAboutToBeRemoved,
        this, &GlobalDebugProfileMenu::profileAboutToBeRemoved);

    connect (mDebugProfiles, &CSMWorld::IdTable::rowsInserted,
        this, &GlobalDebugProfileMenu::profileInserted);

    connect (mDebugProfiles, &CSMWorld::IdTable::dataChanged,
        this, &GlobalDebugProfileMenu::profileChanged);
}

void CSVDoc::GlobalDebugProfileMenu::updateActions (bool running)
{
    if (mActions)
        mActions->setEnabled (!running);
}

void CSVDoc::GlobalDebugProfileMenu::profileAboutToBeRemoved (const QModelIndex& parent,
    int start, int end)
{
    rebuild();
}

void CSVDoc::GlobalDebugProfileMenu::profileInserted (const QModelIndex& parent, int start,
    int end)
{
    rebuild();
}

void CSVDoc::GlobalDebugProfileMenu::profileChanged (const QModelIndex& topLeft,
    const QModelIndex& bottomRight)
{
    rebuild();
}

void CSVDoc::GlobalDebugProfileMenu::actionTriggered (QAction *action)
{
    emit triggered (std::string (action->text().toUtf8().constData()));
}
