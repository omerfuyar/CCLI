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

        -The user who created the room can open
        another terminal and join to the room.

    -Compile:
        For Windows:
            <YOUR_COMPILER> CCLI.c -o CCLI.exe -lws2_32
        For UNIX:
            <YOUR_COMPILER> CCLI.c -o CCLI

    -Run:
        ./CCLI <ROOM_NAME / YOUR_NICK> <room / guest> <ROOM_PORT>
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32 // Windows

#include <winsock2.h>
#include <ws2tcpip.h>

typedef unsigned int SocketHandle;
typedef struct sockaddr_in SocketAddress;

#define SocketInitialize()                                                     \
    do                                                                         \
    {                                                                          \
        WSADATA wsa_data = {0};                                                \
        if (WSAStartup(MAKEWORD(1, 1), &wsa_data) != 0)                        \
        {                                                                      \
            printf("Socket Initialization failed : %d.\n", WSAGetLastError()); \
            exit(-1);                                                          \
        }                                                                      \
    } while (0)

#define SocketTerminate() \
    do                    \
    {                     \
        WSACleanup();     \
    } while (0)

#define SocketCreate(socketPtr)                                                                             \
    do                                                                                                      \
    {                                                                                                       \
        /*if ((*socketPtr = socket(AF_INET, SOCK_STREAM, isServer ? IPPROTO_TCP : 0)) == INVALID_SOCKET) */ \
        if ((*socketPtr = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)                               \
        {                                                                                                   \
            printf("Socket creation failed : %d.\n", WSAGetLastError());                                    \
            SocketTerminate();                                                                              \
            exit(-1);                                                                                       \
        }                                                                                                   \
    } while (0)

#define SocketClose(socketPtr)                                        \
    do                                                                \
    {                                                                 \
        shutdown(*socketPtr, SD_BOTH);                                \
        if (closesocket(*socketPtr) == SOCKET_ERROR)                  \
        {                                                             \
            printf("Socket close failed : %d.\n", WSAGetLastError()); \
            SocketTerminate();                                        \
            exit(-1);                                                 \
        }                                                             \
    } while (0)

#define SocketSetOption(socketPtr, optLevel, optName)                                                         \
    do                                                                                                        \
    {                                                                                                         \
        int option = 1;                                                                                       \
        if (setsockopt(*socketPtr, optLevel, optName, (const char *)&option, sizeof(option)) == SOCKET_ERROR) \
        {                                                                                                     \
            printf("Socket option setting failed : %d.\n", WSAGetLastError());                                \
            SocketClose(socketPtr);                                                                           \
            SocketTerminate();                                                                                \
            exit(-1);                                                                                         \
        }                                                                                                     \
    } while (0)

#define SocketBind(socketPtr, socketAddressPtr)                                                               \
    do                                                                                                        \
    {                                                                                                         \
        if (bind(*socketPtr, (struct sockaddr *)socketAddressPtr, sizeof(*socketAddressPtr)) == SOCKET_ERROR) \
        {                                                                                                     \
            printf("Socket bind failed : %d.\n", WSAGetLastError());                                          \
            SocketClose(socketPtr);                                                                           \
            SocketTerminate();                                                                                \
            exit(-1);                                                                                         \
        }                                                                                                     \
    } while (0)

#define SocketListen(socketPtr, maxQueueLength)                        \
    do                                                                 \
    {                                                                  \
        if (listen(*socketPtr, maxQueueLength) != 0)                   \
        {                                                              \
            printf("Socket listen failed : %d.\n", WSAGetLastError()); \
            SocketClose(socketPtr);                                    \
            SocketTerminate();                                         \
            exit(-1);                                                  \
        }                                                              \
    } while (0)

#define SocketAccept(socketPtr, clientSocketPtr)                                   \
    do                                                                             \
    {                                                                              \
        if ((*clientSocketPtr = accept(*socketPtr, NULL, NULL)) == INVALID_SOCKET) \
        {                                                                          \
            printf("Socket accept failed : %d.\n", WSAGetLastError());             \
            SocketClose(socketPtr);                                                \
            SocketTerminate();                                                     \
            exit(-1);                                                              \
        }                                                                          \
    } while (0)

#define SocketConnect(socketPtr, socketAddressPtr)                                                               \
    do                                                                                                           \
    {                                                                                                            \
        if (connect(*socketPtr, (struct sockaddr *)socketAddressPtr, sizeof(*socketAddressPtr)) == SOCKET_ERROR) \
        {                                                                                                        \
            printf("Socket connection failed : %d.\n", WSAGetLastError());                                       \
            SocketClose(socketPtr);                                                                              \
            SocketTerminate();                                                                                   \
            exit(-1);                                                                                            \
        }                                                                                                        \
    } while (0)

#define SocketSend(clientSocket, msgPtr, msgSize)                            \
    do                                                                       \
    {                                                                        \
        if (send(*clientSocket, msgPtr, msgSize, 0) == SOCKET_ERROR)         \
        {                                                                    \
            printf("Socket message send failed : %d.\n", WSAGetLastError()); \
            SocketTerminate();                                               \
            exit(-1);                                                        \
        }                                                                    \
    } while (0)

#define SocketReceive(socketPtr, bufferPtr, bufferSize)                         \
    do                                                                          \
    {                                                                           \
        if (recv(*socketPtr, bufferPtr, bufferSize, 0) == SOCKET_ERROR)         \
        {                                                                       \
            printf("Socket message receive failed : %d.\n", WSAGetLastError()); \
            SocketTerminate();                                                  \
            exit(-1);                                                           \
        }                                                                       \
    } while (0)

#else // UNIX

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

typedef int SocketHandle;
typedef struct sockaddr_in SocketAddress;

#define SocketInitialize()

#define SocketTerminate()

#define SocketCreate(socketPtr)                                   \
    do                                                            \
    {                                                             \
        if ((*socketPtr = socket(AF_INET, SOCK_STREAM, 0)) == -1) \
        {                                                         \
            perror("Socket creation failed");                     \
            SocketTerminate();                                    \
            exit(-1);                                             \
        }                                                         \
    } while (0)

#define SocketClose(socketPtr)             \
    do                                     \
    {                                      \
        shutdown(*socketPtr, SHUT_RDWR);   \
        if (close(*socketPtr) == -1)       \
        {                                  \
            perror("Socket close failed"); \
            SocketTerminate();             \
            exit(-1);                      \
        }                                  \
    } while (0)

#define SocketSetOption(socketPtr, optLevel, optName)                                               \
    do                                                                                              \
    {                                                                                               \
        int option = 1;                                                                             \
        if (setsockopt(*socketPtr, optLevel, optName, (const char *)&option, sizeof(option)) == -1) \
        {                                                                                           \
            perror("Socket option setting failed");                                                 \
            SocketClose(socketPtr);                                                                 \
            SocketTerminate();                                                                      \
            exit(-1);                                                                               \
        }                                                                                           \
    } while (0)

#define SocketBind(socketPtr, socketAddressPtr)                                                     \
    do                                                                                              \
    {                                                                                               \
        if (bind(*socketPtr, (struct sockaddr *)socketAddressPtr, sizeof(*socketAddressPtr)) == -1) \
        {                                                                                           \
            perror("Socket bind failed");                                                           \
            SocketClose(socketPtr);                                                                 \
            SocketTerminate();                                                                      \
            exit(-1);                                                                               \
        }                                                                                           \
    } while (0)

#define SocketListen(socketPtr, maxQueueLength)       \
    do                                                \
    {                                                 \
        if (listen(*socketPtr, maxQueueLength) == -1) \
        {                                             \
            perror("Socket listen failed");           \
            SocketClose(socketPtr);                   \
            SocketTerminate();                        \
            exit(-1);                                 \
        }                                             \
    } while (0)

#define SocketAccept(socketPtr, clientSocketPtr)                       \
    do                                                                 \
    {                                                                  \
        if ((*clientSocketPtr = accept(*socketPtr, NULL, NULL)) == -1) \
        {                                                              \
            perror("Socket accept failed");                            \
            SocketClose(socketPtr);                                    \
            SocketTerminate();                                         \
            exit(-1);                                                  \
        }                                                              \
    } while (0)

#define SocketConnect(socketPtr, socketAddressPtr)                                                     \
    do                                                                                                 \
    {                                                                                                  \
        if (connect(*socketPtr, (struct sockaddr *)socketAddressPtr, sizeof(*socketAddressPtr)) == -1) \
        {                                                                                              \
            perror("Socket connection failed");                                                        \
            SocketClose(socketPtr);                                                                    \
            SocketTerminate();                                                                         \
            exit(-1);                                                                                  \
        }                                                                                              \
    } while (0)

#define SocketSend(clientSocket, msgPtr, msgSize)          \
    do                                                     \
    {                                                      \
        if (send(*clientSocket, msgPtr, msgSize, 0) == -1) \
        {                                                  \
            perror("Socket message send failed");          \
            SocketTerminate();                             \
            exit(-1);                                      \
        }                                                  \
    } while (0)

#define SocketReceive(socketPtr, bufferPtr, bufferSize)       \
    do                                                        \
    {                                                         \
        if (recv(*socketPtr, bufferPtr, bufferSize, 0) == -1) \
        {                                                     \
            perror("Socket message receive failed");          \
            SocketTerminate();                                \
            exit(-1);                                         \
        }                                                     \
    } while (0)

#endif

#define USER_NICK_MAX_LENGTH 32
#define ROOM_IP_MAX_LENGTH 128
#define ROOM_MAX_GUEST_COUNT 5

typedef enum
{
    UserModeInvalid = -1,
    UserModeGuest = 0,
    UserModeRoom = 1
} UserMode;

char USER_NICK[USER_NICK_MAX_LENGTH];
UserMode USER_MODE = UserModeInvalid;
short unsigned ROOM_PORT = 0;

SocketAddress ROOM_ADDRESS = {0};
SocketHandle USER_SOCKET = {0};
SocketHandle ROOM_GUEST = {0};

int main(const int argc, const char **argv)
{
    if (argc != 4)
    {
        printf("Invalid argument count. Read instructions in the source file.\n");
        exit(-1);
    }

    unsigned char userNickLength = (unsigned char)strlen(argv[1]);
    if (userNickLength > USER_NICK_MAX_LENGTH)
    {
        printf("User nick is too long. Max length must be %d.\n", USER_NICK_MAX_LENGTH);
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
        printf("Invalid user mode. Read instructions in the source file.\n");
        exit(-1);
    }

    unsigned char roomPortLength = (unsigned char)strlen(argv[3]);
    if (roomPortLength > 5 && roomPortLength < 1)
    {
        printf("Invalid room port.\n");
        exit(-1);
    }

    ROOM_PORT = (unsigned short)atoi(argv[3]);

    printf("user nick : '%s'\n", USER_NICK);
    printf("user mode : '%s'\n", argv[2]);
    printf("room port : '%hu'\n", ROOM_PORT);

    SocketInitialize();

    SocketCreate(&USER_SOCKET);

    SocketSetOption(&USER_SOCKET, SOL_SOCKET, SO_REUSEADDR);

    ROOM_ADDRESS.sin_family = AF_INET;
    ROOM_ADDRESS.sin_port = htons(ROOM_PORT);

    if (USER_MODE == UserModeRoom) // user is room
    {
        ROOM_ADDRESS.sin_addr.s_addr = INADDR_ANY;

        SocketBind(&USER_SOCKET, &ROOM_ADDRESS);
        SocketListen(&USER_SOCKET, ROOM_MAX_GUEST_COUNT); // blocking
        SocketAccept(&USER_SOCKET, &ROOM_GUEST);

        while (1)
        {
            char inputBuffer[255];
            fgets(inputBuffer, sizeof(inputBuffer), stdin); // blocking
            SocketSend(&ROOM_GUEST, inputBuffer, sizeof(inputBuffer));
        }
    }
    else // user is guest
    {
        ROOM_ADDRESS.sin_addr.s_addr = inet_addr("127.0.0.1");

        SocketConnect(&USER_SOCKET, &ROOM_ADDRESS);

        while (1)
        {
            char messageBuffer[255];
            SocketReceive(&USER_SOCKET, messageBuffer, sizeof(messageBuffer)); // blocking
            printf("message received from room : %s\n", messageBuffer);
        }
    }

    SocketClose(&USER_SOCKET);

    SocketTerminate();
}
