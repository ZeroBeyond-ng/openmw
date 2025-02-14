#include "operations.hpp"

#include <QVBoxLayout>

#include "operation.hpp"

CSVDoc::Operations::Operations()
{
    /// \todo make widget height fixed (exactly the height required to display all operations)

    setFeatures (QDockWidget::NoDockWidgetFeatures);

    QWidget *widgetContainer = new QWidget (this);
    mLayout = new QVBoxLayout;

    widgetContainer->setLayout (mLayout);
    setWidget (widgetContainer);
    setVisible (false);
    setFixedHeight (widgetContainer->height());
    setTitleBarWidget (new QWidget (this));
}

void CSVDoc::Operations::setProgress (int current, int max, int type, int threads)
{
    for (std::vector<Operation *>::iterator iter (mOperations.begin()); iter!=mOperations.end(); ++iter)
        if ((*iter)->getType()==type)
        {
            (*iter)->setProgress (current, max, threads);
            return;
        }

    int oldCount = static_cast<int>(mOperations.size());
    int newCount = oldCount + 1;

    Operation *operation = new Operation (type, this);
    connect (operation, qOverload<int>(&Operation::abortOperation), this, &Operations::abortOperation);

    mLayout->addLayout (operation->getLayout());
    mOperations.push_back (operation);
    operation->setProgress (current, max, threads);

    if ( oldCount > 0)
        setFixedHeight (height()/oldCount * newCount);

    setVisible (true);
}

void CSVDoc::Operations::quitOperation (int type)
{
    for (std::vector<Operation *>::iterator iter (mOperations.begin()); iter!=mOperations.end(); ++iter)
        if ((*iter)->getType()==type)
        {
            int oldCount = static_cast<int>(mOperations.size());
            int newCount = oldCount - 1;

            mLayout->removeItem ((*iter)->getLayout());

            (*iter)->deleteLater();
            mOperations.erase (iter);

            if (oldCount > 1)
                setFixedHeight (height() / oldCount * newCount);
            else
                setVisible (false);

            break;
        }
}
