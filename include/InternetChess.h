#ifndef INTERNETCHESS_H
#define INTERNETCHESS_H
#define DEFAULT_PLAYER_PORT 3879
#define DEFAULT_VIEWING_PORT 3880
#include <chess/Chess.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <iostream>
namespace chess
{
    class RandomBot : public Bot
    {
    public:
        RandomBot()
        {
            srand(time(NULL));
        }
        Move* findMove(Board* board);
    };
    class HostBot : public Bot
    {
    public:
        HostBot() = default;
        Move* findMove(Board* board);
    };
    class ClientBot : public Bot
    {
    private:
        int newsockfd, sockfd;
    public:
        ClientBot();
        Move* findMove(Board* board);
        void handlePrint(Board* board);
        void handleWinner(int winner);
    };
}
#endif