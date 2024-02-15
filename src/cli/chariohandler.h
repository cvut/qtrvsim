#ifndef CHARIOHANDLER_H
#define CHARIOHANDLER_H

#include <QIODevice>
#include <QObject>
#include <QSet>

class CharIOHandler : public QIODevice {
    Q_OBJECT

    using Super = QIODevice;

public:
    explicit CharIOHandler(QIODevice *iodev, QObject *parent = nullptr);
    ~CharIOHandler() override;

public slots:
    void writeByte(unsigned int data);
    void writeByte(int fd, unsigned int data);
    void readBytePoll(int fd, unsigned int &data, bool &available);

public:
    void insertFd(const int &fd);
    void removeFd(const int &fd);

    [[nodiscard]] bool isSequential() const override;
    bool open(OpenMode mode) override;
    void close() override;
    [[nodiscard]] qint64 pos() const override;
    [[nodiscard]] qint64 size() const override;
    bool seek(qint64 pos) override;
    [[nodiscard]] bool atEnd() const override;
    bool reset() override;
    [[nodiscard]] qint64 bytesAvailable() const override;
    [[nodiscard]] qint64 bytesToWrite() const override;
    [[nodiscard]] bool canReadLine() const override;
    bool waitForReadyRead(int msecs) override;
    bool waitForBytesWritten(int msecs) override;

protected:
    qint64 readData(char *data, qint64 maxSize) override;
    qint64 readLineData(char *data, qint64 maxSize) override;
    qint64 writeData(const char *data, qint64 maxSize) override;

private:
    QIODevice *iodev;
    bool fd_specific;
    QSet<int> fd_list;
};

#endif // CHARIOHANDLER_H
