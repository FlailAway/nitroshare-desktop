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

#ifndef LIBNITROSHARE_LOGGER_H
#define LIBNITROSHARE_LOGGER_H

#include <QObject>

#include <nitroshare/config.h>

/**
 * @brief Manage status and error message from the application and its plugins
 *
 * This class models the fan-out messaging pattern. All info, warning, and
 * error messages are sent to this class for dispatch, allowing plugins to
 * react to the messages as they are generated.
 */
class NITROSHARE_EXPORT Logger : public QObject
{
    Q_OBJECT
    Q_ENUMS(MessageType)

public:

    /**
     * @brief Type of message
     */
    enum MessageType {
        Debug,
        Info,
        Warning,
        Error
    };

    /**
     * @brief Create a new logger
     * @param parent QObject
     */
    explicit Logger(QObject *parent = nullptr);

public Q_SLOTS:

    /**
     * @brief Log the specified message
     * @param messageType type of message
     * @param tag classifier for the message
     * @param message body of the message
     */
    void log(MessageType messageType, const QString &tag, const QString &message);

Q_SIGNALS:

    /**
     * @brief Indicate that a status or informational message was logged
     * @param messageType type of message
     * @param tag classifier for the message
     * @param message body of the message
     */
    void messageLogged(MessageType messageType, const QString &tag, const QString &message);
};

#endif // LIBNITROSHARE_LOGGER_H
