#include "existinginstallationpage.hpp"

#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QFile>

#include "mainwizard.hpp"

Wizard::ExistingInstallationPage::ExistingInstallationPage(QWidget *parent) :
    QWizardPage(parent)
{
    mWizard = qobject_cast<MainWizard*>(parent);

    setupUi(this);

    // Add a placeholder item to the list of installations
    QListWidgetItem *emptyItem = new QListWidgetItem(tr("No existing installations detected"));
    emptyItem->setFlags(Qt::NoItemFlags);

    installationsList->insertItem(0, emptyItem);

}

void Wizard::ExistingInstallationPage::initializePage()
{
    // Add the available installation paths
    QStringList paths(mWizard->mInstallations.keys());

    // Hide the default item if there are installations to choose from
    installationsList->item(0)->setHidden(!paths.isEmpty());

    for (const QString &path : paths)
    {
        if (installationsList->findItems(path, Qt::MatchExactly).isEmpty())
        {
            QListWidgetItem *item = new QListWidgetItem(path);
            installationsList->addItem(item);
        }
    }

    connect(installationsList, &QListWidget::currentTextChanged,
            this, &ExistingInstallationPage::textChanged);

    connect(installationsList, &QListWidget::itemSelectionChanged,
            this, &ExistingInstallationPage::completeChanged);
}

bool Wizard::ExistingInstallationPage::validatePage()
{
    // See if Morrowind.ini is detected, if not, ask the user
    // It can be missing entirely
    // Or failed to be detected due to the target being a symlink

    QString path(field(QLatin1String("installation.path")).toString());
    QFile file(mWizard->mInstallations[path].iniPath);

    if (!file.exists()) {
        QMessageBox msgBox;
        msgBox.setWindowTitle(tr("Error detecting Morrowind configuration"));
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setStandardButtons(QMessageBox::Cancel);
        msgBox.setText(QObject::tr("<br><b>Could not find Morrowind.ini</b><br><br> \
                                   The Wizard needs to update settings in this file.<br><br> \
                                   Press \"Browse...\" to specify the location manually.<br>"));

        QAbstractButton *browseButton2 =
                msgBox.addButton(QObject::tr("B&rowse..."), QMessageBox::ActionRole);

        msgBox.exec();

        QString iniFile;
        if (msgBox.clickedButton() == browseButton2) {
            iniFile = QFileDialog::getOpenFileName(
                        this,
                        QObject::tr("Select configuration file"),
                        QDir::currentPath(),
                        QString(tr("Morrowind configuration file (*.ini)")));
        }

        if (iniFile.isEmpty()) {
            return false; // Cancel was clicked;
        }

        // A proper Morrowind.ini was selected, set it
        QFileInfo info(iniFile);
        mWizard->mInstallations[path].iniPath = info.absoluteFilePath();
    }

    return true;
}

void Wizard::ExistingInstallationPage::on_browseButton_clicked()
{
    QString selectedFile = QFileDialog::getOpenFileName(
                this,
                tr("Select Morrowind.esm (located in Data Files)"),
                QDir::currentPath(),
                QString(tr("Morrowind master file (Morrowind.esm)")),
                nullptr,
                QFileDialog::DontResolveSymlinks);

    if (selectedFile.isEmpty())
        return;

    QFileInfo info(selectedFile);

    if (!info.exists())
        return;

    if (!mWizard->findFiles(QLatin1String("Morrowind"), info.absolutePath()))
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle(tr("Error detecting Morrowind files"));
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setText(QObject::tr(
            "<b>Morrowind.bsa</b> is missing!<br>\
            Make sure your Morrowind installation is complete."
        ));
        msgBox.exec();
        return;
    }

    if (!versionIsOK(info.absolutePath()))
    {
        return;
    }

    QString path(QDir::toNativeSeparators(info.absolutePath()));
    QList<QListWidgetItem*> items = installationsList->findItems(path, Qt::MatchExactly);

    if (items.isEmpty()) {
        // Path is not yet in the list, add it
        mWizard->addInstallation(path);

        // Hide the default item
        installationsList->item(0)->setHidden(true);

        QListWidgetItem *item = new QListWidgetItem(path);
        installationsList->addItem(item);
        installationsList->setCurrentItem(item); // Select it too
    } else {
        installationsList->setCurrentItem(items.first());
    }

    // Update the button
    emit completeChanged();
}

void Wizard::ExistingInstallationPage::textChanged(const QString &text)
{
    // Set the installation path manually, as registerField doesn't work
    // Because it doesn't accept two widgets operating on a single field
    if (!text.isEmpty())
        mWizard->setField(QLatin1String("installation.path"), text);
}

bool Wizard::ExistingInstallationPage::isComplete() const
{
    if (installationsList->selectionModel()->hasSelection()) {
        return true;
    } else {
        return false;
    }
}

int Wizard::ExistingInstallationPage::nextId() const
{
    return MainWizard::Page_LanguageSelection;
}

bool Wizard::ExistingInstallationPage::versionIsOK(QString directory_name)
{
    QDir directory = QDir(directory_name);
    QFileInfoList infoList = directory.entryInfoList(QStringList(QString("Morrowind.bsa")));
    if (infoList.size() == 1)
    {
        qint64 actualFileSize = infoList.at(0).size();
        const qint64 expectedFileSize = 310459500; // Size of Morrowind.bsa in Steam and GOG editions.

        if (actualFileSize == expectedFileSize)
        {
            return true;
        }

        QMessageBox msgBox;
        msgBox.setWindowTitle(QObject::tr("Most recent Morrowind not detected"));
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        msgBox.setText(QObject::tr("<br><b>There may be a more recent version of Morrowind available.</b><br><br>\
                              Do you wish to continue anyway?<br>"));
        int ret = msgBox.exec();
        if (ret == QMessageBox::Yes)
        {
            return true;
        }

        return false;
    }

    return false;
}
