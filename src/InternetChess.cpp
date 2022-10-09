#include <InternetChess.h>
#include <memory.h>
#include <sstream>
using namespace std;
using namespace chess;
const string cols = "ABCDEFGH";
int newsockfd;
Move* HostBot::findMove(Board* board)
{
request_move:
    cout << "Please enter your move in format [A1 B2]: ";
    string token;
    cin >> token;
    if (token == "resign")
    {
        return Move::resign();
    }
    int srcCol = cols.find(token[0]);
    int srcRow = token[1] - '0';
    int destCol = cols.find(token[3]);
    int destRow = token[4] - '0';
    Move* move = new Move(Point(srcRow, srcCol), Point(destRow, destCol));
    if (!board->canMove(move)) goto request_move;
    return move;
}
Move* ClientBot::findMove(Board* board)
{
    char buffer[16];
get_move:
    memset(buffer, 0, 16);
    memcpy(buffer, "enter_move", 11);
    write(newsockfd, buffer, 15);
    memset(buffer, 0, 16);
    read(newsockfd, buffer, 15);
    string token = buffer;
    if (token == "resign")
    {
        return Move::resign();
    }
    int srcCol = cols.find(token[0]);
    int srcRow = token[1] - '0';
    int destCol = cols.find(token[3]);
    int destRow = token[4] - '0';
    Move* move = new Move(Point(srcRow, srcCol), Point(destRow, destCol));
    if (!board->canMove(move)) goto get_move;
    return move;
}
int hostChessGame()
{
    Game game = Game(new HostBot(), new ClientBot());
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        cout << "Error opening socket" << endl;
        return 2;
    }
    sockaddr_in hostAddress;
    memset(&hostAddress, 0, sizeof(sockaddr_in));
    hostAddress.sin_family = AF_INET;
    hostAddress.sin_addr.s_addr = INADDR_ANY;
    hostAddress.sin_port = DEFAULT_PORT;
    if (bind(sockfd, (sockaddr*)&hostAddress, sizeof(sockaddr_in)) < 0)
    {
        cout << "Error binding" << endl;
        return 3;
    }
    cout << "Waiting for client" << endl;
    listen(sockfd, 5);
    sockaddr clientAddress;
    socklen_t clientLength = sizeof(clientAddress);
    newsockfd = accept(sockfd, &clientAddress, &clientLength);
    if (newsockfd < 0)
    {
        cout << "Error accepting" << endl;
        return 4;
    }
    char buffer[16];
    memcpy(buffer, "read5483", 9);
    write(newsockfd, buffer, 15);
    memset(buffer, 0, 16);
    read(newsockfd, buffer, 15);
    if (strcmp(buffer, "ready3845") != 0)
    {
        cout << "Client technology incompatable\n";
        return 5;
    }
    cout << "Found client" << endl;
    while (game.getCurrentBoard()->winner == -1)
    {
        cout << game.getCurrentBoard();
        memcpy(buffer, "print", 6);
        write(newsockfd, buffer, 15);
        memset(buffer, 0, 16);
        read(newsockfd, buffer, 15);
        if (strcmp(buffer, "ready") == 0)
        {
            string boardString;
            stringstream stream(boardString);
            stream << game.getCurrentBoard();
            size_t stringLength = boardString.size() + 1;
            write(newsockfd, &stringLength, sizeof(size_t));
            write(newsockfd, boardString.c_str(), stringLength);
        }
        game.step();
    }
    cout << "Winner: " << game.getCurrentBoard()->winner ? "Black" : "White";
    close(newsockfd);
    close(sockfd);
    return 0;
}
int connectChessGame(const char* host)
{
}
int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        cout << "Invalid arguments. Needs [host|computername]" << endl;
        return 1;
    }
    if (string(argv[1]) == "host")
    {
        return hostChessGame();
    }
    else
    {
        return connectChessGame(argv[1]);
    }
}