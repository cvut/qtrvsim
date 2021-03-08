#include "chariohandler.h"

CharIOHandler::CharIOHandler(QIODevice *iodev, QObject *parent)
    : QIODevice(parent)
    , fd_list() {
    this->iodev = iodev;
    if (!iodev->parent()) {
        iodev->setParent(this);
    }
    fd_specific = false;
    if (iodev->isOpen()) {
        Super::open(iodev->openMode());
    }
    connect(iodev, &Super::aboutToClose, this, &CharIOHandler::aboutToClose);
    connect(iodev, &Super::bytesWritten, this, &CharIOHandler::bytesWritten);
    connect(
        iodev, &Super::channelBytesWritten, this,
        &CharIOHandler::channelBytesWritten);
    connect(
        iodev, &Super::channelReadyRead, this,
        &CharIOHandler::channelReadyRead);
    connect(
        iodev, &Super::readChannelFinished, this,
        &CharIOHandler::readChannelFinished);
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
    if (!fd_specific || fd_list.contains(fd))
        writeByte(data);
}

void CharIOHandler::readBytePoll(int fd, unsigned int &data, bool &available) {
    char ch;
    qint64 res;
    if (!fd_specific || fd_list.contains(fd)) {
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
