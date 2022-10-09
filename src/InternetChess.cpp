#include <InternetChess.h>
#include <memory.h>
#include <sstream>
using namespace std;
using namespace chess;
const string cols = "ABCDEFGH";
int newsockfd;
Move* HostBot::findMove(Board* board)
{
    cout << "Please enter your move in format [A1 B2]: ";
    string token;
    getline(cin, token);
    if (token == "resign")
    {
        return Move::resign();
    }
    int srcCol = cols.find(token[0]);
    int srcRow = token[1] - '1';
    int destCol = cols.find(token[3]);
    int destRow = token[4] - '1';
    cout << "(" << srcRow << ", " << srcCol << "), (" << destRow << ", " << destCol << ")\n";
    Move* move = new Move(Point(srcRow, srcCol), Point(destRow, destCol));
    if (!board->canMove(move, false))
    {
        cout << "Invalid move.\n";
        return findMove(board);
    }
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
    int srcRow = token[1] - '1';
    int destCol = cols.find(token[3]);
    int destRow = token[4] - '1';
    Move* move = new Move(Point(srcRow, srcCol), Point(destRow, destCol));
    if (!board->canMove(move, false)) goto get_move;
    return move;
}
int hostChessGame()
{
    Game game = Game(new HostBot(), new ClientBot());
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) < 0)
    {
        return 1;
    }
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
    memcpy(buffer, "ready5483", 10);
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
        cout << *game.getCurrentBoard();
        memcpy(buffer, "print", 6);
        write(newsockfd, buffer, 15);
        memset(buffer, 0, 16);
        read(newsockfd, buffer, 15);
        if (strcmp(buffer, "ready") == 0)
        {
            stringstream stream;
            stream << *game.getCurrentBoard();
            string boardString = stream.str();
            size_t stringLength = boardString.size() + 1;
            write(newsockfd, &stringLength, sizeof(size_t));
            write(newsockfd, boardString.c_str(), stringLength);
        }
        game.step();
    }
    cout << "Winner: " << game.getCurrentBoard()->winner ? "Black" : "White";
    memcpy(buffer, "game_over", 10);
    write(newsockfd, buffer, 15);
    memcpy(buffer, (game.getCurrentBoard()->winner ? "white" : "black"), 6);
    write(newsockfd, buffer, 15);
    close(newsockfd);
    close(sockfd);
    return 0;
}
int connectChessGame(const char* hostName)
{
    char buffer[16];
    sockaddr_in hostAddress;
    hostent* host;
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) < 0)
    {
        return 1;
    }
    host = gethostbyname(hostName);
    memset(&hostAddress, 0, sizeof(sockaddr_in));
    hostAddress.sin_family = AF_INET;
    memcpy(&hostAddress.sin_addr.s_addr, host->h_addr, host->h_length);
    hostAddress.sin_port = DEFAULT_PORT;
    if (connect(sockfd, (sockaddr*)&hostAddress, sizeof(hostAddress)) < 0)
    {
        cout << "Error connecting to " << hostName << endl;
        return 1;
    }
    memset(buffer, 0, 16);
    read(sockfd, buffer, 15);
    if (strcmp(buffer, "ready5483") != 0)
    {
        cout << "Host technology incompatable\n";
        return 2;
    }
    memcpy(buffer, "ready3845", 10);
    write(sockfd, buffer, 15);
    while (true)
    {
        read(sockfd, buffer, 15);
        string cmd = buffer;
        if (cmd == "print")
        {
            memcpy(buffer, "ready", 6);
            write(sockfd, buffer, 15);
            size_t stringLength;
            read(sockfd, &stringLength, sizeof(size_t));
            char* boardString = new char[stringLength];
            read(sockfd, boardString, stringLength);
            cout << boardString;
        }
        else if (cmd == "enter_move")
        {
            cout << "Please enter your move in format [A1 B2]: ";
            string token;
            getline(cin, token);
            memcpy(buffer, token.c_str(), token.size() + 1);
            write(sockfd, buffer, 15);
        }
        else if (cmd == "game_over")
        {
            read(sockfd, buffer, 15);
            cout << "Winner is " << buffer << endl;
            break;
        }
    }
    close(sockfd);
    return 0;
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