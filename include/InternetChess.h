#ifndef INTERNETCHESS_H
#define INTERNETCHESS_H
#define DEFAULT_PORT 3879
#include <chess/Chess.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <iostream>
namespace chess
{
    class HostBot : public Bot
    {
    public:
        HostBot() = default;
        Move* findMove(Board* board);
    };
    class ClientBot : public Bot
    {
    public:
        ClientBot() = default;
        Move* findMove(Board* board);
    };
}
#endif