// SPDX-License-Identifier: GPL-2.0+
/*******************************************************************************
 * QtMips - MIPS 32-bit Architecture Subset Simulator
 *
 * Implemented to support following courses:
 *
 *   B35APO - Computer Architectures
 *   https://cw.fel.cvut.cz/wiki/courses/b35apo
 *
 *   B4M35PAP - Advanced Computer Architectures
 *   https://cw.fel.cvut.cz/wiki/courses/b4m35pap/start
 *
 * Copyright (c) 2017-2019 Karel Koci<cynerd@email.cz>
 * Copyright (c) 2019      Pavel Pisa <pisa@cmp.felk.cvut.cz>
 *
 * Faculty of Electrical Engineering (http://www.fel.cvut.cz)
 * Czech Technical University        (http://www.cvut.cz/)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 *
 ******************************************************************************/

#ifndef CHARIOHANDLER_H
#define CHARIOHANDLER_H

#include <QObject>
#include <QIODevice>
#include <QSet>

class CharIOHandler : public QIODevice
{
    Q_OBJECT

    using Super = QIODevice;

public:
    explicit CharIOHandler(QIODevice *iodev, QObject *parent = nullptr);
    virtual ~CharIOHandler() override;

public slots:
    void writeByte(unsigned int data);
    void writeByte(int fd, unsigned int data);
    void readBytePoll(int fd, unsigned int &data, bool &available);

public:
    void insertFd(const int &fd);
    void removeFd(const int &fd);

    virtual bool isSequential() const override;
    virtual bool open(OpenMode mode) override;
    virtual void close() override;
    virtual qint64 pos() const override;
    virtual qint64 size() const override;
    virtual bool seek(qint64 pos) override;
    virtual bool atEnd() const override;
    virtual bool reset() override;
    virtual qint64 bytesAvailable() const override;
    virtual qint64 bytesToWrite() const override;
    virtual bool canReadLine() const override;
    virtual bool waitForReadyRead(int msecs) override;
    virtual bool waitForBytesWritten(int msecs) override;
protected:
    virtual qint64 	readData(char *data, qint64 maxSize) override;
    virtual qint64 	readLineData(char *data, qint64 maxSize) override;
    virtual qint64 	writeData(const char *data, qint64 maxSize) override;
private:
    QIODevice *iodev;
    bool fd_specific;
    QSet<int> fd_list;
};

#endif // CHARIOHANDLER_H
