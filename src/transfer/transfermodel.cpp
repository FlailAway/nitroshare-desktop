/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Nathan Osman
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 **/

#include "transfermodel.h"

int TransferModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : mTransfers.count();
}

int TransferModel::columnCount(const QModelIndex &parent) const
{
    // The model displays device name and transfer progress
    return parent.isValid() ? 0 : 2;
}

QVariant TransferModel::data(const QModelIndex &index, int role) const
{
    TransferPointer transfer(mTransfers.at(index.row()));

    switch(role) {
    case Qt::DisplayRole:
        switch(index.column()) {
        case 0: return QVariant();
        case 1: return QVariant();
        }
    case Qt::UserRole:
        return QVariant::fromValue(transfer);
    }

    return QVariant();
}

QVariant TransferModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch(section) {
        case 0: return tr("Device Name");
        case 1: return tr("Percentage");
        }
    }

    return QVariant();
}

void TransferModel::add(TransferPointer transfer)
{
    beginInsertRows(QModelIndex(), mTransfers.count(), mTransfers.count());
    mTransfers.append(transfer);
    endInsertRows();

    connect(transfer.data(), &Transfer::finished, this, &TransferModel::removeTransfer);
    transfer->start();
}

void TransferModel::removeTransfer()
{
    //...
}