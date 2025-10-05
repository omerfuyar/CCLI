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

		printf("Creating room '%s'...\n", argv[2]);

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

		printf("Room is ready, listening to port : %d...\n", port);

		for(;;)
		{
			int clientSocket = accept(roomSocket, NULL, NULL);

			if(clientSocket == -1)
			{
				close(roomSocket);
				goto errAccept;
			}

			printf("Connected");

			char readBuffer[HTTP_BUFFER] = {0};
			int readCount = read(clientSocket, readBuffer, HTTP_BUFFER);
			if(readCount == -1)
			{
				close(clientSocket);
				close(roomSocket);
				goto errRead;
			}

			printf("%s", readBuffer);

		 	int writeCount = write(clientSocket, "test", 4);
			if(writeCount == -1)
			{
				close(clientSocket);
				close(roomSocket);
				goto errWrite;
			}

			close(clientSocket);
		}
	}
	else if (!strcmp(argv[1], "guest"))
	{
		struct addrinfo addressHints = {
			.ai_family = AF_INET,
			.ai_socktype = SOCK_STREAM,
			.ai_flags = 0,
			.ai_protocol = 0
		};

		struct addrinfo *addressResults = NULL;

		printf("Getting addres information...\n");

		if(getaddrinfo(argv[3], "http", &addressHints, &addressResults) == -1)
		{
			goto errInfo;
		}

		struct addrinfo *addressFound = addressResults;

		for(; addressFound != NULL; addressFound = addressFound->ai_next)
		{
			int tempSocket = socket(addressFound->ai_family, addressFound->ai_socktype, addressFound->ai_protocol);

			printf("%d, %d, %d\n", addressFound->ai_family, addressFound->ai_socktype, addressFound->ai_protocol);

			if(tempSocket == -1)
			{
				continue;
			}

			if(connect(tempSocket, addressFound->ai_addr, addressFound->ai_addrlen) != -1)
			{
				close(tempSocket);
				break;
			}

			close(tempSocket);
		}

		if(addressFound == NULL)
		{
			freeaddrinfo(addressResults);
			goto errBadAddress;
		}

		printf("Address found. Creating client...\n");

		char inputBuffer[HTTP_BUFFER] = {0};

		for(;;)
		{
			printf("\n[%s] : ", argv[2]);

			if(fgets(inputBuffer, HTTP_BUFFER, stdin) == NULL)
			{
				freeaddrinfo(addressResults);
				goto errInput;
			}

			int clientSocket = socket(addressFound->ai_family, addressFound->ai_socktype, addressFound->ai_protocol);

			if(clientSocket == -1)
			{
				freeaddrinfo(addressResults);
				goto errSocket;
			}

			if(connect(clientSocket, addressResults->ai_addr, addressResults->ai_addrlen) == -1)
			{
				close(clientSocket);
				freeaddrinfo(addressResults);
				goto errConnect;
			}

			int writeStatus = -1;

			if(strcmp(inputBuffer, QUERY_STRING))
			{
				writeStatus = write(clientSocket, inputBuffer, strlen(inputBuffer));
			}
			else
			{
				writeStatus = write(clientSocket, "get", 3);
			}

			if(writeStatus == -1)
			{
				close(clientSocket);
				freeaddrinfo(addressResults);
				goto errWrite;
			}

			char readBuffer[HTTP_BUFFER] = {0};

			int readStatus = read(clientSocket, readBuffer, HTTP_BUFFER);

			if(readStatus == -1)
			{
				close(clientSocket);
				freeaddrinfo(addressResults);
				goto errRead;
			}

			close(clientSocket);
		}

		freeaddrinfo(addressResults);
	}
	else
	{
		goto errBadMode;
	}

	printf("Exited successfully.\n");
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
