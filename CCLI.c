/*
CCLI
    -Chat-CLI is a cross platform minimal
    dependency command line interface
    chat application. All you need is just
    a working compiler, internet connection
    and knowledge about how to use the app
    which indicated above.

HOW TO USE
    -Logic:
        -A user starts the app in room mode
        and other users connects to that room
        in guest mode.

        -Both user modes can send messages to
        room message pool and see each others
        messages.

    -Compile:
        <YOUR_COMPILER> main.c -o main[.exe]

    -Run:
        ./main <YOUR_NICK> <room / guest> < / ROOM_IP_TO_CONNECT>
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32 // Windows

#include <winsock2.h>
#include <Ws2tcpip.h>

typedef unsigned int SocketHandle;

#define SocketInitialize()                     \
    do                                         \
    {                                          \
        WSADATA wsa_data;                      \
        WSAStartup(MAKEWORD(1, 1), &wsa_data); \
    } while (0)

#define SocketTerminate() \
    do                    \
    {                     \
        WSACleanup();     \
    } while (0)

#define SocketCreate()

#define SocketClose(socket)                 \
    do                                      \
    {                                       \
        status = shutdown(socket, SD_BOTH); \
        if (status == 0)                    \
        {                                   \
            status = closesocket(socket);   \
        }                                   \
        statusRet = status                  \
    } while (0)

#else // POSIX

#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>  /* Needed for getaddrinfo() and freeaddrinfo() */
#include <unistd.h> /* Needed for close() */

typedef int SocketHandle;

#define SocketInitialize()
#define SocketTerminate()

#define SocketCreate()

#define SocketClose(socket, statusRet)        \
    do                                        \
    {                                         \
        int status = 0;                       \
        status = shutdown(socket, SHUT_RDWR); \
        if (status == 0)                      \
        {                                     \
            status = close(socket);           \
        }                                     \
        statusRet = status                    \
    } while (0)

#endif

#define USER_NICK_MAX_LENGTH 32
#define ROOM_IP_MAX_LENGTH 128

typedef enum
{
    UserModeInvalid = -1,
    UserModeRoom = 0,
    UserModeGuest = 1
} UserMode;

UserMode USER_MODE = UserModeInvalid;
char USER_NICK[USER_NICK_MAX_LENGTH];

char serMsg[255] = "Message from the server to the client \"Hello Client\"";

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        printf("Invalid argument count. Read instructions in the source file");
        exit(-1);
    }

    unsigned char userNickLength = strlen(argv[1]);
    if (userNickLength > USER_NICK_MAX_LENGTH)
    {
        printf("User nick is too long. Max length must be %d", USER_NICK_MAX_LENGTH);
        exit(-1);
    }

    strncpy(USER_NICK, argv[1], userNickLength);

    if (strcmp("room", argv[2]) == 0)
    {
        USER_MODE = UserModeRoom;
    }
    else if (strcmp("guest", argv[2]) == 0)
    {
        USER_MODE = UserModeGuest;
    }
    else
    {
        printf("Invalid user mode. Read instructions in the source file");
        exit(-1);
    }

    printf("user nick : %s\n", USER_NICK);
    printf("user mode : %s\n", argv[2]);
}
