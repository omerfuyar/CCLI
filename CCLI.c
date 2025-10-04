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
			<CCLI_EXECUTABLE> guest <GUEST_NAME> <ROOM_HTTP_URL_TO_JOIN>

	-Example:
		Room:
			./CCLI room MyRoom 3000
			[ngrok http 3000]
		Guests:
			./CCLI guest LocalGuest http://localhost:3000
			[./CCLI guest RemoteGuest http://my-ngrok-url.com]
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
#define HTTP_BUFFER 1024
#define QUIT_STRING "!q"
#define QUERY_STRING "\n"

int main(int argc, char **argv)
{
	if (argc != 4)
	{
		goto errArgCount;
	}

	if (!strcmp(argv[1], "room"))
	{
		int port = atoi(argv[3]);
		if(port < 0 || port > 65535){
			goto errBadPort;
		}

		struct sockaddr_in roomAddress = {
			.sin_family = AF_INET,
			.sin_port = htons(port),
			.sin_addr.s_addr = htonl(INADDR_ANY)
		};

		int roomSocket = socket(AF_INET, SOCK_STREAM, 0);

		if(roomSocket == -1)
		{
			goto errSocket;
		}

		if(setsockopt(roomSocket, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) == -1)
		{
			close(roomSocket);
			goto errOption;
		}

		if(bind(roomSocket, (struct sockaddr *)&roomAddress, sizeof(roomAddress)) == -1)
		{
			close(roomSocket);
			goto errBind;
		}

		if(listen(roomSocket, ROOM_SIZE) == -1)
		{
			close(roomSocket);
			goto errListen;
		}

		for(;;)
		{
			int clientSocket = accept(roomSocket, NULL, NULL);

			if(clientSocket == -1)
			{
				close(roomSocket);
				goto errAccept;
			}

			char readBuffer[HTTP_BUFFER] = {0};
			int readCount = read(clientSocket, readBuffer, HTTP_BUFFER);
			if(readCount == -1)
			{
				close(clientSocket);
				close(roomSocket);
				goto errRead;
			}

			printf("%.*s", readCount, readBuffer);

		 	int writeCount = write(clientSocket, "test", 4);
			if(writeCount == -1)
			{
				close(clientSocket);
				close(roomSocket);
				goto errWrite;
			}

			//parse http
			//read
			//write if get

			close(clientSocket);
		}
	}
	else if (!strcmp(argv[1], "guest"))
	{
		struct addrinfo addressHints = {
			.ai_family = AF_INET,
			.ai_socktype = SOCK_STREAM
		};

		struct addrinfo *addressResult = NULL;

		if(getaddrinfo(argv[3], "http", &addressHints, &addressResult) == -1)
		{
			goto errInfo;
		}

		int clientSocket = socket(addressResult->ai_family, addressResult->ai_socktype, addressResult->ai_protocol);

		if(clientSocket == -1)
		{
			freeaddrinfo(addressResult);
			goto errSocket;
		}

		char inputBuffer[HTTP_BUFFER] = {0};

		for(;;)
		{
			if(fgets(inputBuffer, HTTP_BUFFER, stdin) == NULL)
			{
				close(clientSocket);
				freeaddrinfo(addressResult);
				goto errInput;
			}

			if(!strcmp(inputBuffer, QUIT_STRING))
			{
				break;
			}

			if(connect(clientSocket, addressResult->ai_addr, addressResult->ai_addrlen) == -1)
			{
				close(clientSocket);
				freeaddrinfo(addressResult);
				goto errConnect;
			}

			int writeStatus = write(clientSocket, inputBuffer, strlen(inputBuffer) + 1);

			if(writeStatus == -1)
			{
				close(clientSocket);
				freeaddrinfo(addressResult);
				goto errWrite;
			}

			char readBuffer[HTTP_BUFFER] = {0};

			int readStatus = read(clientSocket, readBuffer, HTTP_BUFFER);

			if(readStatus == -1)
			{
				close(clientSocket);
				freeaddrinfo(addressResult);
				goto errRead;
			}

			close(clientSocket);
		}

		close(clientSocket);
		freeaddrinfo(addressResult);
	}
	else
	{
		goto errBadMode;
	}

	return 0;

errArgCount:
	printf("Invalid use of app. Please read the guide in source code.\n");
	return 1;

errBadMode:
	printf("Invalid user mode. Please read the guide in source code.\n");
	return 2;

errBadPort:
	printf("Invalid port. Please enter a usable port.\n");
	return 3;

errBadAddress:
	printf("Invalid address trying to access. Please enter a usable URL.\n");
	return 4;

errSocket:
	printf("Error creating socket.\n");
	perror("socket");
	return 5;

errOption:
	printf("Error setting socket option.\n");
	perror("option");
	return 6;

errBind:
	printf("Error binding socket.\n");
	perror("bind");
	return 7;

errListen:
	printf("Error listening socket.\n");
	perror("listen");
	return 8;

errAccept:
	printf("Error accepting connection.\n");
	perror("accept");
	return 9;

errRead:
	printf("Error reading file.\n");
	perror("accept");
	return 10;

errWrite:
	printf("Error writing file.\n");
	perror("write");
	return 11;

errInfo:
	printf("Error getting address info.\n");
	perror("info");
	return 12;

errConnect:
	printf("Error connecting socket.\n");
	perror("connect");
	return 13;

errInput:
	printf("Error getting input.\n");
	perror("input");
	return 14;
}
