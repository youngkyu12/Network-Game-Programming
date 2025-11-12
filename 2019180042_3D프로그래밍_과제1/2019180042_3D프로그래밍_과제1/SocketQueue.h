#pragma once


#pragma pack(1)
struct InputPacket
{
    BYTE keyW;      // 0 or 1
    BYTE keyS;      // 0 or 1
    LONG mouseX;    // 커서 X
    LONG mouseY;    // 커서 Y
};
#pragma pack()

enum Packet_ID
{
    KEY_INPUT = 1,
    MOVE_MOUSE = 2,
    FIRE_OBJECT = 3,
    PLAYER_STATE = 4
};

#pragma pack(1)
struct PacketHeader
{
    uint16_t size;
    char id; // Packet_ID
};
#pragma pack()

#pragma pack(1)
struct PlayerState
{
    char Player_ID;
    char hp;
    float pos_x;
    float pos_y;
    float pos_z;
    float yaw;
    char Shield;
    bool fire;

};
#pragma pack()

class SendQueue {
public:
    SendQueue() { InitializeCriticalSection(&cs); }
    ~SendQueue() { DeleteCriticalSection(&cs); }

    void push(InputPacket data) {
        EnterCriticalSection(&cs);
        sendqueue.push(data);
        LeaveCriticalSection(&cs);
    }

    void pop() {
        EnterCriticalSection(&cs);
        if (!sendqueue.empty()) sendqueue.pop();
        LeaveCriticalSection(&cs);
    }

    bool empty() {
        EnterCriticalSection(&cs);
        bool result = sendqueue.empty();
        LeaveCriticalSection(&cs);
        return result;
    }

    InputPacket front() {
        EnterCriticalSection(&cs);
        if (!sendqueue.empty()) {
            InputPacket result = sendqueue.front();
            LeaveCriticalSection(&cs);
            return result;
        }
        LeaveCriticalSection(&cs);
        throw std::runtime_error("Queue is empty");
    }

private:
    std::queue<InputPacket> sendqueue;
    CRITICAL_SECTION cs;
};

class RecvQueue {
public:
    RecvQueue() {
        InitializeCriticalSection(&cs);
    }

    ~RecvQueue() {
        DeleteCriticalSection(&cs);
    }

    void push(char* data) {
        EnterCriticalSection(&cs);
        recvqueue.push(data);
        LeaveCriticalSection(&cs);
    }

    void pop() {
        EnterCriticalSection(&cs);
        if (!recvqueue.empty()) {
            recvqueue.pop();
        }
        LeaveCriticalSection(&cs);
    }

    bool empty() {
        EnterCriticalSection(&cs);
        bool result = recvqueue.empty();
        LeaveCriticalSection(&cs);
        return result;
    }

    char* front() {
        EnterCriticalSection(&cs);
        if (!recvqueue.empty()) {
            char* result = recvqueue.front();
            LeaveCriticalSection(&cs);
            return result;
        }
        LeaveCriticalSection(&cs);
        throw std::runtime_error("Queue is empty");
    }

private:
    std::queue<char*> recvqueue;
    CRITICAL_SECTION cs;
};