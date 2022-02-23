#ifndef LOGGER_H
#define LOGGER_H

#include "fmt/format.h"

class DiskFlux;
class TrackDataFlux;
class TrackFlux;
class Sector;

struct BeginSpeedOperationLogMessage
{
};
struct EndSpeedOperationLogMessage
{
    nanoseconds_t rotationalPeriod;
};

struct TrackReadLogMessage
{
    std::shared_ptr<TrackFlux> track;
};

struct DiskReadLogMessage
{
	std::shared_ptr<DiskFlux> disk;
};

struct BeginReadOperationLogMessage
{
    unsigned cylinder;
    unsigned head;
};

struct EndReadOperationLogMessage
{
};

struct BeginWriteOperationLogMessage
{
    unsigned cylinder;
    unsigned head;
};

struct EndWriteOperationLogMessage
{
};

class TrackFlux;

typedef std::variant<std::string,
    TrackReadLogMessage,
	DiskReadLogMessage,
    BeginSpeedOperationLogMessage,
    EndSpeedOperationLogMessage,
    BeginReadOperationLogMessage,
    EndReadOperationLogMessage,
    BeginWriteOperationLogMessage,
    EndWriteOperationLogMessage>
    AnyLogMessage;

class Logger
{
public:
    Logger& operator<<(std::shared_ptr<const AnyLogMessage> message);

    template <class T>
    Logger& operator<<(const T& message)
    {
        return *this << std::make_shared<const AnyLogMessage>(message);
    }

    static void setLogger(
        std::function<void(std::shared_ptr<const AnyLogMessage>)> cb);

    static std::string toString(const AnyLogMessage&);
    static void textLogger(std::shared_ptr<const AnyLogMessage>);
};

#endif
