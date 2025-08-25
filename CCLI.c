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
        For Room:
            ./CCLI <ROOM_NAME> room <ROOM_PORT>
        For Guest:
            ./CCLI <YOUR_NICK> guest <ROOM_PORT> <SERVER_IP_ADDRESS>
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32 // Windows

#include <winsock2.h>
#include <ws2tcpip.h>

typedef unsigned int SocketHandle;
typedef fd_set SocketSet;
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
            printf("Socket create failed : %d.\n", WSAGetLastError());                                      \
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

#define SocketOption(socketPtr, optLevel, optName)                                                            \
    do                                                                                                        \
    {                                                                                                         \
        int option = 1;                                                                                       \
        if (setsockopt(*socketPtr, optLevel, optName, (const char *)&option, sizeof(option)) == SOCKET_ERROR) \
        {                                                                                                     \
            printf("Socket option set failed : %d.\n", WSAGetLastError());                                    \
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
            printf("Socket connect failed : %d.\n", WSAGetLastError());                                          \
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

#define SocketSetSelect(setPtr, setSize)                               \
    do                                                                 \
    {                                                                  \
        if (select(0, setPtr, NULL, NULL, NULL) == SOCKET_ERROR)       \
        {                                                              \
            printf("Socket select failed : %d.\n", WSAGetLastError()); \
            SocketTerminate();                                         \
            exit(-1);                                                  \
        }                                                              \
    } while (0)

#else // UNIX

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

typedef int SocketHandle;
typedef fd_set SocketSet;
typedef struct sockaddr_in SocketAddress;

#define SocketInitialize()

#define SocketTerminate()

#define SocketCreate(socketPtr)                                   \
    do                                                            \
    {                                                             \
        if ((*socketPtr = socket(AF_INET, SOCK_STREAM, 0)) == -1) \
        {                                                         \
            perror("Socket create failed");                       \
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

#define SocketOption(socketPtr, optLevel, optName)                                                  \
    do                                                                                              \
    {                                                                                               \
        int option = 1;                                                                             \
        if (setsockopt(*socketPtr, optLevel, optName, (const char *)&option, sizeof(option)) == -1) \
        {                                                                                           \
            perror("Socket option set failed");                                                     \
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
            perror("Socket connect failed");                                                           \
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

#define SocketSetSelect() \
    do                    \
    {                     \
    } while (0)

#endif

#define USER_NICK_MAX_LENGTH 32
#define MESSAGE_MAX_LENGTH 512
#define ROOM_IP_MAX_LENGTH 128
#define ROOM_MAX_GUEST_COUNT FD_SETSIZE

typedef enum
{
    UserModeInvalid = -1,
    UserModeGuest = 0,
    UserModeRoom = 1
} UserMode;

char USER_NICK[USER_NICK_MAX_LENGTH] = {0};
UserMode USER_MODE = UserModeInvalid;
short unsigned ROOM_PORT = 0;

SocketAddress ROOM_ADDRESS = {0};
SocketHandle USER_SOCKET = {0}; // either room or guest (server or client)

int main(const int argc, const char **argv)
{
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

    if ((USER_MODE == UserModeRoom && argc != 4) || (USER_MODE == UserModeGuest && argc != 5))
    {
        printf("Invalid argument count for selected mode. Read instructions.\n");
        exit(-1);
    }

    unsigned char userNickLength = (unsigned char)strlen(argv[1]);
    if (userNickLength > USER_NICK_MAX_LENGTH)
    {
        printf("%s is too long. Max length must be %d.\n", USER_MODE == UserModeRoom ? "Room name" : "User nick", USER_NICK_MAX_LENGTH);
        exit(-1);
    }

    strncpy(USER_NICK, argv[1], userNickLength);

    unsigned char roomPortLength = (unsigned char)strlen(argv[3]);
    if (roomPortLength > 5 && roomPortLength < 1)
    {
        printf("Invalid room port.\n");
        exit(-1);
    }

    ROOM_PORT = (unsigned short)atoi(argv[3]);

    printf("\n");

    SocketInitialize();

    SocketCreate(&USER_SOCKET);

    ROOM_ADDRESS.sin_family = AF_INET;
    ROOM_ADDRESS.sin_port = htons(ROOM_PORT);

    char messageBuffer[MESSAGE_MAX_LENGTH] = {0};

    if (USER_MODE == UserModeRoom) // user is room
    {
        char hostName[256];
        gethostname(hostName, sizeof(hostName));
        printf("Server is running on host: %s\n", hostName);
        printf("Potential room IPs to connect:\n");

        struct hostent *pHostEntry = gethostbyname(hostName);
        if (pHostEntry != NULL)
        {
            for (int i = 0; pHostEntry->h_addr_list[i] != NULL; i++)
            {
                struct in_addr addr;
                memcpy(&addr, pHostEntry->h_addr_list[i], sizeof(struct in_addr));
                printf("  - %s\n", inet_ntoa(addr));
            }
        }

        ROOM_ADDRESS.sin_addr.s_addr = INADDR_ANY;

        SocketOption(&USER_SOCKET, SOL_SOCKET, SO_REUSEADDR);

        SocketBind(&USER_SOCKET, &ROOM_ADDRESS);

        SocketListen(&USER_SOCKET, ROOM_MAX_GUEST_COUNT);

        SocketSet roomGuestSet = {0};
        SocketSet roomGuestEventSet = {0};

        FD_ZERO(&roomGuestSet);
        FD_SET(USER_SOCKET, &roomGuestSet);

        while (1)
        {
            roomGuestEventSet = roomGuestSet;

            SocketSetSelect(&roomGuestEventSet, ROOM_MAX_GUEST_COUNT);

            for (unsigned int index = 0; index < roomGuestEventSet.fd_count; index++)
            {
                SocketHandle triggeredSocket = roomGuestEventSet.fd_array[index];

                if (FD_ISSET(triggeredSocket, &roomGuestEventSet))
                {
                    if (triggeredSocket == USER_SOCKET) // event from server, new connection
                    {
                        SocketHandle newGuestSocket = {0};
                        SocketAccept(&USER_SOCKET, &newGuestSocket);
                        FD_SET(newGuestSocket, &roomGuestSet);
                    }
                    else // event from already connected socket, handle connection
                    {
                        SocketReceive(&triggeredSocket, messageBuffer, sizeof(messageBuffer));

                        if (strncmp(messageBuffer, "!q", 2) == 0)
                        {
                            SocketClose(&triggeredSocket);
                            FD_CLR(triggeredSocket, &roomGuestSet);
                        }
                        else
                        {
                            printf("%s", messageBuffer);
                            for (unsigned int i = 0; i < roomGuestSet.fd_count; i++)
                            {
                                SocketHandle targetSocket = roomGuestSet.fd_array[i];
                                if (targetSocket != USER_SOCKET && targetSocket != triggeredSocket)
                                {
                                    SocketSend(&targetSocket, messageBuffer, sizeof(messageBuffer));
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    else // user is guest
    {
        const char *serverIP = argv[4];
        ROOM_ADDRESS.sin_addr.s_addr = inet_addr(serverIP);

        SocketConnect(&USER_SOCKET, &ROOM_ADDRESS);

        while (1)
        {
            unsigned int nickOffset = userNickLength + sizeof("[] : ");

            printf("[%s] : ", USER_NICK);
            fgets((char *)messageBuffer + nickOffset, (int)(sizeof(messageBuffer) - nickOffset), stdin);
            sprintf(messageBuffer, "[%s] : %s", argv[1], messageBuffer + nickOffset);
            SocketSend(&USER_SOCKET, messageBuffer, sizeof(messageBuffer));

            SocketReceive(&USER_SOCKET, messageBuffer, sizeof(messageBuffer));
            printf("%s", messageBuffer);
        }
    }

    SocketTerminate();
}
