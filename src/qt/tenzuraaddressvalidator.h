// Copyright (c) 2011-2014 The Bitcoin Core developers
// Copyright (c) 2017-2019 The Tenzura Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef TENZURA_QT_TENZURAADDRESSVALIDATOR_H
#define TENZURA_QT_TENZURAADDRESSVALIDATOR_H

#include <QValidator>

/** Base58 entry widget validator, checks for valid characters and
 * removes some whitespace.
 */
class TenzuraAddressEntryValidator : public QValidator
{
    Q_OBJECT

public:
    explicit TenzuraAddressEntryValidator(QObject *parent);

    State validate(QString &input, int &pos) const;
};

/** Tenzura address widget validator, checks for a valid tenzura address.
 */
class TenzuraAddressCheckValidator : public QValidator
{
    Q_OBJECT

public:
    explicit TenzuraAddressCheckValidator(QObject *parent);

    State validate(QString &input, int &pos) const;
};

#endif // TENZURA_QT_TENZURAADDRESSVALIDATOR_H
