#include "lineedit.hpp"

LineEdit::LineEdit(QWidget *parent)
    : QLineEdit(parent)
{
    setupClearButton();
}

void LineEdit::setupClearButton()
{
    mClearButton = new QToolButton(this);
    QPixmap pixmap(":images/clear.png");
    mClearButton->setIcon(QIcon(pixmap));
    mClearButton->setIconSize(pixmap.size());
    mClearButton->setCursor(Qt::ArrowCursor);
    mClearButton->setStyleSheet("QToolButton { border: none; padding: 0px; }");
    mClearButton->hide();
    connect(mClearButton, &QToolButton::clicked, this, &LineEdit::clear);
    connect(this, &LineEdit::textChanged, this, &LineEdit::updateClearButton);
}

void LineEdit::resizeEvent(QResizeEvent *)
{
    QSize sz = mClearButton->sizeHint();
    int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
    mClearButton->move(rect().right() - frameWidth - sz.width(),
                      (rect().bottom() + 1 - sz.height())/2);
}

void LineEdit::updateClearButton(const QString& text)
{
    mClearButton->setVisible(!text.isEmpty());
}
