/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Nathan Osman
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
 */

#ifndef SENDACTION_H
#define SENDACTION_H

#include <nitroshare/action.h>

class Application;

/**
 * @brief Send a bundle to the specified device with the specified files and directories
 *
 * The action expects three parameters:
 *
 * - "device" (QString) - name of the device to send the bundle to
 * - "root" (QString) - directory to which all item paths will be relative
 * - "items" (QStringList) - list of items
 */
class SendAction : public Action
{
    Q_OBJECT

public:

    SendAction(Application *application);

    virtual QString name() const;

public Q_SLOTS:

    virtual bool invoke(const QVariantMap &params);

private:

    Application *mApplication;
};

#endif // SENDACTION_H
