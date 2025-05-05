// Copyright (c) 2011-2015 The Bitcoin Core developers
// Copyright (c) 2017-2021 The Tenzura Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "tenzuraamountfield.h"

#include "tenzuraunits.h"
#include "guiconstants.h"
#include "qvaluecombobox.h"
#include "platformstyle.h"

#include <QDebug>
#include <QApplication>
#include <QAbstractSpinBox>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLineEdit>

#if QT_VERSION < QT_VERSION_CHECK(5, 11, 0)
#define QTversionPreFiveEleven
#endif

/** QSpinBox that uses fixed-point numbers internally and uses our own
 * formatting/parsing functions.
 */
class AmountSpinBox: public QAbstractSpinBox
{
    Q_OBJECT

public:
    explicit AmountSpinBox(QWidget *parent):
        QAbstractSpinBox(parent),
        currentUnit(TenzuraUnits::TENZ),
        singleStep(100000), // satoshis
        assetUnit(-1)
    {
        setAlignment(Qt::AlignRight);

        connect(lineEdit(), SIGNAL(textEdited(QString)), this, SIGNAL(valueChanged()));
    }

    QValidator::State validate(QString &text, int &pos) const
    {
        if(text.isEmpty())
            return QValidator::Intermediate;
        bool valid = false;
        parse(text, &valid);
        /* Make sure we return Intermediate so that fixup() is called on defocus */
        return valid ? QValidator::Intermediate : QValidator::Invalid;
    }

    void fixup(QString &input) const
    {
        bool valid = false;
        CAmount val = parse(input, &valid);
        if(valid)
        {
            input = TenzuraUnits::format(currentUnit, val, false, TenzuraUnits::separatorAlways, assetUnit);
            lineEdit()->setText(input);
        }
    }

    CAmount value(bool *valid_out=0) const
    {
        return parse(text(), valid_out);
    }

    void setValue(const CAmount& value)
    {
        lineEdit()->setText(TenzuraUnits::format(currentUnit, value, false, TenzuraUnits::separatorAlways, assetUnit));
        Q_EMIT valueChanged();
    }

    void stepBy(int steps)
    {
        bool valid = false;
        CAmount val = value(&valid);
        val = val + steps * singleStep;
        val = qMin(qMax(val, CAmount(0)), TenzuraUnits::maxMoney());
        setValue(val);
    }

    void setDisplayUnit(int unit)
    {
        bool valid = false;
        CAmount val = value(&valid);

        currentUnit = unit;

        if(valid)
            setValue(val);
        else
            clear();
    }

    void setSingleStep(const CAmount& step)
    {
        singleStep = step;
    }

    void setAssetUnit(int unit)
    {
        if (unit > MAX_ASSET_UNITS)
            unit = MAX_ASSET_UNITS;

        assetUnit = unit;

        bool valid = false;
        CAmount val = value(&valid);

        if(valid)
            setValue(val);
        else
            clear();
    }

    QSize minimumSizeHint() const
    {
        if(cachedMinimumSizeHint.isEmpty())
        {
            ensurePolished();

            const QFontMetrics fm(fontMetrics());
            int h = lineEdit()->minimumSizeHint().height();
			#ifndef QTversionPreFiveEleven
            	int w = fm.horizontalAdvance(TenzuraUnits::format(TenzuraUnits::TENZ, TenzuraUnits::maxMoney(), false, TenzuraUnits::separatorAlways, assetUnit));
			#else
				int w = fm.width(TenzuraUnits::format(TenzuraUnits::TENZ, TenzuraUnits::maxMoney(), false, TenzuraUnits::separatorAlways, assetUnit));
			#endif
            w += 2; // cursor blinking space

            QStyleOptionSpinBox opt;
            initStyleOption(&opt);
            QSize hint(w, h);
            QSize extra(35, 6);
            opt.rect.setSize(hint + extra);
            extra += hint - style()->subControlRect(QStyle::CC_SpinBox, &opt,
                                                    QStyle::SC_SpinBoxEditField, this).size();
            // get closer to final result by repeating the calculation
            opt.rect.setSize(hint + extra);
            extra += hint - style()->subControlRect(QStyle::CC_SpinBox, &opt,
                                                    QStyle::SC_SpinBoxEditField, this).size();
            hint += extra;
            hint.setHeight(h);

            opt.rect = rect();

            cachedMinimumSizeHint = style()->sizeFromContents(QStyle::CT_SpinBox, &opt, hint, this)
                                    .expandedTo(QApplication::globalStrut());
        }
        return cachedMinimumSizeHint;
    }

private:
    int currentUnit;
    CAmount singleStep;
    mutable QSize cachedMinimumSizeHint;
    int assetUnit;

    /**
     * Parse a string into a number of base monetary units and
     * return validity.
     * @note Must return 0 if !valid.
     */
    CAmount parse(const QString &text, bool *valid_out=0) const
    {
        CAmount val = 0;

        // Update parsing function to work with asset parsing units
        bool valid = false;
        if (assetUnit >= 0) {
            valid = TenzuraUnits::assetParse(assetUnit, text, &val);
        }
        else
            valid = TenzuraUnits::parse(currentUnit, text, &val);

        if(valid)
        {
            if(val < 0 || val > TenzuraUnits::maxMoney())
                valid = false;
        }
        if(valid_out)
            *valid_out = valid;
        return valid ? val : 0;
    }

protected:
    bool event(QEvent *event)
    {
        if (event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if (keyEvent->key() == Qt::Key_Comma)
            {
                // Translate a comma into a period
                QKeyEvent periodKeyEvent(event->type(), Qt::Key_Period, keyEvent->modifiers(), ".", keyEvent->isAutoRepeat(), keyEvent->count());
                return QAbstractSpinBox::event(&periodKeyEvent);
            }
        }
        return QAbstractSpinBox::event(event);
    }

    StepEnabled stepEnabled() const
    {
        if (isReadOnly()) // Disable steps when AmountSpinBox is read-only
            return StepNone;
        if (text().isEmpty()) // Allow step-up with empty field
            return StepUpEnabled;

        StepEnabled rv = StepNone;
        bool valid = false;
        CAmount val = value(&valid);
        if(valid)
        {
            if(val > 0)
                rv |= StepDownEnabled;
            if(val < TenzuraUnits::maxMoney())
                rv |= StepUpEnabled;
        }
        return rv;
    }

Q_SIGNALS:
    void valueChanged();
};

#include "tenzuraamountfield.moc"

TenzuraAmountField::TenzuraAmountField(QWidget *parent) :
    QWidget(parent),
    amount(0)
{
    amount = new AmountSpinBox(this);
    amount->setLocale(QLocale::c());
    amount->installEventFilter(this);
    amount->setMaximumWidth(170);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(amount);
    unit = new QValueComboBox();
    unit->setModel(new TenzuraUnits(this));
    layout->addWidget(unit);
    layout->addStretch(1);
    layout->setContentsMargins(0,0,0,0);

    setLayout(layout);

    setFocusPolicy(Qt::TabFocus);
    setFocusProxy(amount);

    // If one if the widgets changes, the combined content changes as well
    connect(amount, SIGNAL(valueChanged()), this, SIGNAL(valueChanged()));
    connect(unit, SIGNAL(currentIndexChanged(int)), this, SLOT(unitChanged(int)));

    // Set default based on configuration
    unitChanged(unit->currentIndex());

}

void TenzuraAmountField::clear()
{
    amount->clear();
    unit->setCurrentIndex(0);
}

void TenzuraAmountField::setEnabled(bool fEnabled)
{
    amount->setEnabled(fEnabled);
    unit->setEnabled(fEnabled);
}

bool TenzuraAmountField::validate()
{
    bool valid = false;
    value(&valid);
    setValid(valid);
    return valid;
}

void TenzuraAmountField::setValid(bool valid)
{
    if (valid) {
            amount->setStyleSheet("");
    } else {
            amount->setStyleSheet(STYLE_INVALID);
    }
}

bool TenzuraAmountField::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::FocusIn)
    {
        // Clear invalid flag on focus
        setValid(true);
    }
    return QWidget::eventFilter(object, event);
}

QWidget *TenzuraAmountField::setupTabChain(QWidget *prev)
{
    QWidget::setTabOrder(prev, amount);
    QWidget::setTabOrder(amount, unit);
    return unit;
}

CAmount TenzuraAmountField::value(bool *valid_out) const
{
    return amount->value(valid_out);
}

void TenzuraAmountField::setValue(const CAmount& value)
{
    amount->setValue(value);
}

void TenzuraAmountField::setReadOnly(bool fReadOnly)
{
    amount->setReadOnly(fReadOnly);
}

void TenzuraAmountField::unitChanged(int idx)
{
    // Use description tooltip for current unit for the combobox
    unit->setToolTip(unit->itemData(idx, Qt::ToolTipRole).toString());

    // Determine new unit ID
    int newUnit = unit->itemData(idx, TenzuraUnits::UnitRole).toInt();

    amount->setDisplayUnit(newUnit);
}

void TenzuraAmountField::setDisplayUnit(int newUnit)
{
    unit->setValue(newUnit);
}

void TenzuraAmountField::setSingleStep(const CAmount& step)
{
    amount->setSingleStep(step);
}

AssetAmountField::AssetAmountField(QWidget *parent) :
        QWidget(parent),
        amount(0)
{
    amount = new AmountSpinBox(this);
    amount->setLocale(QLocale::c());
    amount->installEventFilter(this);
    amount->setMaximumWidth(170);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(amount);
    layout->addStretch(1);
    layout->setContentsMargins(0,0,0,0);

    setLayout(layout);

    setFocusPolicy(Qt::TabFocus);
    setFocusProxy(amount);

    // If one if the widgets changes, the combined content changes as well
    connect(amount, SIGNAL(valueChanged()), this, SIGNAL(valueChanged()));

    // Set default based on configuration
    setUnit(MAX_ASSET_UNITS);
}

void AssetAmountField::clear()
{
    amount->clear();
    setUnit(MAX_ASSET_UNITS);
}

void AssetAmountField::setEnabled(bool fEnabled)
{
    amount->setEnabled(fEnabled);
}

bool AssetAmountField::validate()
{
    bool valid = false;
    value(&valid);
    setValid(valid);
    return valid;
}

void AssetAmountField::setValid(bool valid)
{
    if (valid) {
        amount->setStyleSheet("");
    } else {
        amount->setStyleSheet(STYLE_INVALID);
    }
}

bool AssetAmountField::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::FocusIn)
    {
        // Clear invalid flag on focus
        setValid(true);
    }
    return QWidget::eventFilter(object, event);
}

CAmount AssetAmountField::value(bool *valid_out) const
{
    return amount->value(valid_out) * TenzuraUnits::factorAsset(8 - assetUnit);
}

void AssetAmountField::setValue(const CAmount& value)
{
    amount->setValue(value);
}

void AssetAmountField::setReadOnly(bool fReadOnly)
{
    amount->setReadOnly(fReadOnly);
}

void AssetAmountField::setSingleStep(const CAmount& step)
{
    amount->setSingleStep(step);
}

void AssetAmountField::setUnit(int unit)
{
    assetUnit = unit;
    amount->setAssetUnit(assetUnit);
}
