#pragma once

enum Packet_ID
{
    START = 0,
    MOVE = 1,
    TEMP = 2,
    UPDATE = 3,
};


#pragma pack(1)
struct Packetheader
{
    uint16_t size;
    uint16_t ID; // Packet_ID
};
#pragma pack()

#pragma pack(1)
struct MovePacket
{
    Packetheader header;
    uint16_t keyW = 0;
    uint16_t keyS = 0;
    //-----서버와 크기 맞추기 용 입니다.
    float yaw = 0;
    //uint32_t mouseY = 0;
};
#pragma pack()

#pragma pack(1)
struct PlayerState
{
    //int16_t UpdateID;
    int32_t Player_ID;
   // char hp;
    float pos_x;
    float pos_y;
    float pos_z;
    float Lookx;
    float Looky;
    float Lookz;
    //float yaw;
    //char Shield;
    //bool fire;
};
#pragma pack()

#pragma pack(1)
struct UpdateState
{
    Packetheader header;
    int32_t numPlayers;
    PlayerState players[3];
};
#pragma pack()

class SendQueue {
public:
    SendQueue() { InitializeCriticalSection(&cs); }
    ~SendQueue() { DeleteCriticalSection(&cs); }

    void push(MovePacket data) {
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

    MovePacket front() {
        EnterCriticalSection(&cs);
        if (!sendqueue.empty()) {
            MovePacket result = sendqueue.front();
            LeaveCriticalSection(&cs);
            return result;
        }
        LeaveCriticalSection(&cs);
        //throw std::runtime_error("Queue is empty");
    }

private:
    std::queue<MovePacket> sendqueue;
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

    void push(PlayerState data) {
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

    PlayerState front() {
        EnterCriticalSection(&cs);
        if (!recvqueue.empty()) {
            PlayerState result = recvqueue.front();
            LeaveCriticalSection(&cs);
            return result;
        }
        LeaveCriticalSection(&cs);
        //throw std::runtime_error("Queue is empty");
    }

private:
    std::queue<PlayerState> recvqueue;
    CRITICAL_SECTION cs;
};