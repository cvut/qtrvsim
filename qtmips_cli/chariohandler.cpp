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

#include <chariohandler.h>

CharIOHandler::CharIOHandler(QIODevice *iodev, QObject *parent) :
               QIODevice(parent), fd_list() {
    this->iodev = iodev;
    if (!iodev->parent())
        iodev->setParent(this);
    fd_specific = false;
    if (iodev->isOpen())
        Super::open(iodev->openMode());
    connect(iodev, &Super::aboutToClose, this, &CharIOHandler::aboutToClose);
    connect(iodev, &Super::bytesWritten, this, &CharIOHandler::bytesWritten);
    connect(iodev, &Super::channelBytesWritten, this, &CharIOHandler::channelBytesWritten);
    connect(iodev, &Super::channelReadyRead, this, &CharIOHandler::channelReadyRead);
    connect(iodev, &Super::readChannelFinished, this, &CharIOHandler::readChannelFinished);
    connect(iodev, &Super::readyRead, this, &CharIOHandler::readyRead);
}

CharIOHandler::~CharIOHandler() {
    if (iodev->parent() == this)
        delete iodev;
}

void CharIOHandler::writeByte(unsigned int data) {
    char ch = (char)data;
    write(&ch, 1);
}

void CharIOHandler::writeByte(int fd, unsigned int data) {
   if(!fd_specific || fd_list.contains(fd))
       writeByte(data);
}

void CharIOHandler::readBytePoll(int fd, unsigned int &data, bool &available) {
    char ch;
    qint64 res;
    if(!fd_specific || fd_list.contains(fd)) {
        if (bytesAvailable() > 0) {
            res = read(&ch, 1);
            if (res > 0) {
                data = ch & 0xff;
                available = true;
            }
        }
    }
}

void CharIOHandler::insertFd(const int &fd) {
    fd_list.insert(fd);
}

void CharIOHandler::removeFd(const int &fd) {
    fd_list.remove(fd);
}

bool CharIOHandler::isSequential() const {
    return iodev->isSequential();
}

bool CharIOHandler::open(OpenMode mode) {
    if (!iodev->open(mode)) {

        return false;
    }
    Super::open(mode);
    return true;
}

void CharIOHandler::close() {
    Super::close();
    iodev->close();
}

qint64 CharIOHandler::pos() const {
    return iodev->pos();
}

qint64 CharIOHandler::size() const {
    return iodev->size();
}

bool CharIOHandler::seek(qint64 pos) {
    return iodev->seek(pos);
}

bool CharIOHandler::atEnd() const {
    return iodev->atEnd();
}

bool CharIOHandler::reset() {
    return iodev->reset();
}

qint64 CharIOHandler::bytesAvailable() const {
    return iodev->bytesAvailable() + Super::bytesAvailable();
}

qint64 CharIOHandler::bytesToWrite() const {
    return iodev->bytesToWrite() + Super::bytesToWrite();
}

bool CharIOHandler::canReadLine() const {
    return iodev->canReadLine();
}

bool CharIOHandler::waitForReadyRead(int msecs) {
    return iodev->waitForReadyRead(msecs);
}

bool CharIOHandler::waitForBytesWritten(int msecs) {
    return iodev->waitForBytesWritten(msecs);
}

qint64 CharIOHandler::readData(char *data, qint64 maxSize) {
    return iodev->read(data, maxSize);
}

qint64 CharIOHandler::readLineData(char *data, qint64 maxSize) {
    return iodev->readLine(data, maxSize);
}

qint64 CharIOHandler::writeData(const char *data, qint64 maxSize) {
    return iodev->write(data, maxSize);
}
