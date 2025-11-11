#pragma once

#include "stdafx.h"

#pragma pack(1)
struct InputPacket
{
    BYTE keyW;      // 0 or 1
    BYTE keyS;      // 0 or 1
    LONG mouseX;    // 커서 X
    LONG mouseY;    // 커서 Y
};
#pragma pack(0)

class SendQueue {
public:
    SendQueue() = default;
    ~SendQueue() = default;

    void push(InputPacket data) {
        std::lock_guard<std::mutex> lock(mutex_);
        sendqueue.push(data);
    }

    void pop() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!sendqueue.empty()) {
            sendqueue.pop();
        }
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return sendqueue.empty();
    }

    InputPacket front() const {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!sendqueue.empty()) {
            return sendqueue.front();
        }
        throw std::runtime_error("Queue is empty");
    }

private:
    std::queue<InputPacket> sendqueue;
    mutable std::mutex mutex_;  
};