/*
CCLI
	-Chat-CLI is a cross platform minimal
	dependency command line interface local
	network based chat application. All you
	need is just a working compiler, internet
	connection and knowledge about how to
	use the app which indicated down below.

HOW TO USE
	-Logic:
		One user creates a room which listens to
		the specified local port and responds to
		requests. There can be 2 types of requests:
		POST for sending messages,
		GET for polled messages in the room.
		You can use things like ngrok to make your
		room available worldwide.

		Other users creates a guest and joins to
		the room with required arguments. Guests
		enters a blank newline character to get
		polled messages, and basically anything
		else to send messages.

	-Compile:
		UNIX:
			<YOUR_COMPILER> CCLI.c -o CCLI

	-Run:
		Room:
			<CCLI_EXECUTABLE> room <ROOM_NAME> <ROOM_PORT_TO_LISTEN>
		Guest:
			<CCLI_EXECUTABLE> guest <GUEST_NAME> <ROOM_URL_TO_JOIN> <ROOM_PORT_TO_JOIN>

	-Example:
		Room:
			./CCLI room MyRoom 3000
			[ngrok http 3000]
		Guests:
			./CCLI guest LocalGuest localhost 3000
			[./CCLI guest RemoteGuest my-ngrok-url.com 80]
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#define ROOM_SIZE 16
#define MESSAGE_BUFFER 1024
#define MESSAGE_HISTORY 128
#define GUEST_NAME_BUFFER 32
#define QUERY_STRING "\n"

int main(int argc, char **argv)
{
	if (argc != 4 && argc != 5)
	{
		goto errArgCount;
	}

	if (strlen(argv[2]) > GUEST_NAME_BUFFER)
	{
		goto errBuffer;
	}

	if (!strcmp(argv[1], "room"))
	{
		if (argc != 4)
		{
			goto errArgCount;
		}

		int port = atoi(argv[3]);

		if (port < 0 || port > 65535)
		{
			goto errBadPort;
		}

		struct sockaddr_in roomAddress = {
			.sin_family = AF_INET,
			.sin_port = htons(port),
			.sin_addr.s_addr = htonl(INADDR_ANY)};

		printf("Creating room '%s'...\n", argv[2]);

		int roomSocket = socket(AF_INET, SOCK_STREAM, 0);

		if (roomSocket == -1)
		{
			goto errSocket;
		}

		if (setsockopt(roomSocket, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) == -1)
		{
			close(roomSocket);
			goto errOption;
		}

		if (bind(roomSocket, (struct sockaddr *)&roomAddress, sizeof(roomAddress)) == -1)
		{
			close(roomSocket);
			goto errBind;
		}

		if (listen(roomSocket, ROOM_SIZE) == -1)
		{
			close(roomSocket);
			goto errListen;
		}

		char history[MESSAGE_HISTORY * MESSAGE_BUFFER] = {0};
		size_t historyIndex = 0;

		printf("Room is ready, listening to port : %d...\n", port);

		for (;;)
		{
			int clientSocket = accept(roomSocket, NULL, NULL);

			if (clientSocket == -1)
			{
				close(roomSocket);
				goto errAccept;
			}

			char readBuffer[MESSAGE_BUFFER] = {0};

			int readCount = read(clientSocket, readBuffer, MESSAGE_BUFFER);

			if (readCount == -1)
			{
				close(roomSocket);
				goto errRead;
			}
			else if (readCount == 0)
			{
				continue;
			}

			if (!strcmp(readBuffer, QUERY_STRING))
			{
				int writeCount = write(clientSocket, history, MESSAGE_HISTORY * MESSAGE_BUFFER);

				if (writeCount == -1)
				{
					close(roomSocket);
					goto errWrite;
				}
			}
			else
			{
				snprintf(history + historyIndex, MESSAGE_HISTORY * MESSAGE_BUFFER - historyIndex, "%s", readBuffer);

				historyIndex += strlen(readBuffer);

				if (historyIndex >= MESSAGE_HISTORY * MESSAGE_BUFFER)
				{
					historyIndex = 0;
					memset(history, 0, MESSAGE_HISTORY * MESSAGE_BUFFER);
				}

				history[historyIndex] = '\0';
			}
		}
	}
	else if (!strcmp(argv[1], "guest"))
	{
		if (argc != 5)
		{
			goto errArgCount;
		}

		struct addrinfo addressHints = {
			.ai_family = AF_INET,
			.ai_socktype = SOCK_STREAM,
			.ai_flags = 0,
			.ai_protocol = 0};

		struct addrinfo *addressResults = NULL;

		printf("Getting addres information...\n");

		if (getaddrinfo(argv[3], argv[4], &addressHints, &addressResults) == -1)
		{
			goto errInfo;
		}

		struct addrinfo *addressFound = addressResults;

		for (; addressFound != NULL; addressFound = addressFound->ai_next)
		{
			int tempSocket = socket(addressFound->ai_family, addressFound->ai_socktype, addressFound->ai_protocol);

			if (tempSocket == -1)
			{
				continue;
			}

			if (connect(tempSocket, addressFound->ai_addr, addressFound->ai_addrlen) != -1)
			{
				close(tempSocket);
				break;
			}

			close(tempSocket);
		}

		if (addressFound == NULL)
		{
			freeaddrinfo(addressResults);
			goto errBadAddress;
		}

		char inputBuffer[MESSAGE_BUFFER] = {0};

		printf("Address found. Creating client...\n");

		for (;;)
		{
			printf("[%s] : ", argv[2]);

			if (fgets(inputBuffer, MESSAGE_BUFFER, stdin) == NULL)
			{
				freeaddrinfo(addressResults);
				goto errInput;
			}

			printf("\x1b[2J");

			char request[MESSAGE_BUFFER] = {0};
			char isGet = 0;

			if (!strcmp(inputBuffer, QUERY_STRING))
			{
				snprintf(request, MESSAGE_BUFFER, QUERY_STRING);
				isGet = 1;
			}
			else
			{
				snprintf(request, MESSAGE_BUFFER, "[%s] : %s", argv[2], inputBuffer);
				isGet = 0;
			}

			int clientSocket = socket(addressFound->ai_family, addressFound->ai_socktype, addressFound->ai_protocol);

			if (clientSocket == -1)
			{
				freeaddrinfo(addressResults);
				goto errSocket;
			}

			if (connect(clientSocket, addressResults->ai_addr, addressResults->ai_addrlen) == -1)
			{
				close(clientSocket);
				freeaddrinfo(addressResults);
				goto errConnect;
			}

			int writeStatus = write(clientSocket, request, MESSAGE_BUFFER);

			if (writeStatus == -1)
			{
				close(clientSocket);
				freeaddrinfo(addressResults);
				goto errWrite;
			}

			if (isGet == 1)
			{
				char readBuffer[MESSAGE_HISTORY * MESSAGE_BUFFER] = {0};

				int readStatus = read(clientSocket, readBuffer, MESSAGE_HISTORY * MESSAGE_BUFFER);

				if (readStatus == -1)
				{
					close(clientSocket);
					freeaddrinfo(addressResults);
					goto errRead;
				}

				printf("\x1b[2J%s", readBuffer);
			}

			close(clientSocket);
		}
	}
	else
	{
		goto errBadMode;
	}

errArgCount:
	printf("\nInvalid use of app. Please read the guide in source code.\n");
	return 1;

errBuffer:
	printf("\nBuffer error. Name should be shorter than %d characters\n", GUEST_NAME_BUFFER);
	return 2;

errBadMode:
	printf("\nInvalid user mode. Please read the guide in source code.\n");
	return 3;

errBadPort:
	printf("\nInvalid port. Please enter a usable port.\n");
	return 4;

errBadAddress:
	printf("\nInvalid address trying to access. Please enter a usable URL.\n");
	return 5;

errSocket:
	printf("\nError creating socket.\n");
	perror("socket");
	return 6;

errOption:
	printf("\nError setting socket option.\n");
	perror("option");
	return 7;

errBind:
	printf("\nError binding socket.\n");
	perror("bind");
	return 8;

errListen:
	printf("\nError listening socket.\n");
	perror("listen");
	return 9;

errAccept:
	printf("\nError accepting connection.\n");
	perror("accept");
	return 10;

errRead:
	printf("\nError reading file.\n");
	perror("read");
	return 11;

errWrite:
	printf("\nError writing file.\n");
	perror("write");
	return 12;

errInfo:
	printf("\nError getting address info.\n");
	perror("info");
	return 13;

errConnect:
	printf("\nError connecting socket.\n");
	perror("connect");
	return 14;

errInput:
	printf("\nError getting input.\n");
	perror("input");
	return 15;
}
