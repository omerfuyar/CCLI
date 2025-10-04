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
        Windows:
            <YOUR_COMPILER> CCLI.c -o CCLI.exe -lws2_32
        UNIX:
            <YOUR_COMPILER> CCLI.c -o CCLI

    -Run:
        Room:
            <CCLI_EXECUTABLE> room <ROOM_NAME> <ROOM_PORT_TO_LISTEN>
        Guest:
            <CCLI_EXECUTABLE> guest <GUEST_NAME> <ROOM_URL_TO_JOIN>

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

int main(int argc, char **argv)
{
    if (argc != 4)
    {
        goto errArgCount;
    }

    if (!strcmp(argv[1], "room"))
    {
    }
    else if (!strcmp(argv[1], "guest"))
    {
    }
    else
    {
        goto errArgUse;
    }

success:
    return 0;

errArgCount:
    printf("Invalid use of app. Please read the guide in source code.");
    return 1;

errArgUse:
    printf("Invalid Arguments. Please read the guide in source code.");
    return 2;
}