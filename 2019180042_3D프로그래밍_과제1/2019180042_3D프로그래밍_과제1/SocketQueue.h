#pragma once

enum Packet_ID
{
    START = 0,
    MOVE = 1,
    FIRE = 2,
    TEMP = 3,
    UPDATE = 4,
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
    uint8_t FireFlag = 0; // false = 0 true = 1

};
#pragma pack()

#pragma pack(1)
struct FirePacket
{
    Packetheader header;
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
    bool fire;
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

    void push(MovePacket data) 
    {
        EnterCriticalSection(&cs);
        sendqueue.push(data);
        LeaveCriticalSection(&cs);
    }

    bool pop(MovePacket& outData) 
    {
        EnterCriticalSection(&cs);
        if (sendqueue.empty())
        {
            LeaveCriticalSection(&cs);
            return false; // 비어있을시 실패 반환
        }
        outData = sendqueue.front();
        sendqueue.pop();
        LeaveCriticalSection(&cs);
        return true; // 성공적으로 데이터를 넘겼다면 true 반환
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

    bool pop(PlayerState& outData) 
    {
        EnterCriticalSection(&cs);
        if (recvqueue.empty())
        {
            LeaveCriticalSection(&cs);
            return false;
        }
        outData = recvqueue.front();
        recvqueue.pop();
        LeaveCriticalSection(&cs);
        return true;
    }

private:
    std::queue<PlayerState> recvqueue;
    CRITICAL_SECTION cs;
};