
#include "intsetting.hpp"

#include <limits>

#include <QLabel>
#include <QSpinBox>
#include <QMutexLocker>

#include <components/settings/settings.hpp>

#include "category.hpp"
#include "state.hpp"

CSMPrefs::IntSetting::IntSetting (Category *parent,
  QMutex *mutex, const std::string& key, const std::string& label, int default_)
: Setting (parent, mutex, key, label), mMin (0), mMax (std::numeric_limits<int>::max()),
  mDefault (default_), mWidget(nullptr)
{}

CSMPrefs::IntSetting& CSMPrefs::IntSetting::setRange (int min, int max)
{
    mMin = min;
    mMax = max;
    return *this;
}

CSMPrefs::IntSetting& CSMPrefs::IntSetting::setMin (int min)
{
    mMin = min;
    return *this;
}

CSMPrefs::IntSetting& CSMPrefs::IntSetting::setMax (int max)
{
    mMax = max;
    return *this;
}

CSMPrefs::IntSetting& CSMPrefs::IntSetting::setTooltip (const std::string& tooltip)
{
    mTooltip = tooltip;
    return *this;
}

std::pair<QWidget *, QWidget *> CSMPrefs::IntSetting::makeWidgets (QWidget *parent)
{
    QLabel *label = new QLabel (QString::fromUtf8 (getLabel().c_str()), parent);

    mWidget = new QSpinBox (parent);
    mWidget->setRange (mMin, mMax);
    mWidget->setValue (mDefault);

    if (!mTooltip.empty())
    {
        QString tooltip = QString::fromUtf8 (mTooltip.c_str());
        label->setToolTip (tooltip);
        mWidget->setToolTip (tooltip);
    }

    connect (mWidget, qOverload<int>(&QSpinBox::valueChanged), this, &IntSetting::valueChanged);

    return std::make_pair (label, mWidget);
}

void CSMPrefs::IntSetting::updateWidget()
{
    if (mWidget)
    {
        mWidget->setValue(Settings::Manager::getInt(getKey(), getParent()->getKey()));
    }
}

void CSMPrefs::IntSetting::valueChanged (int value)
{
    {
        QMutexLocker lock (getMutex());
        Settings::Manager::setInt (getKey(), getParent()->getKey(), value);
    }

    getParent()->getState()->update (*this);
}
