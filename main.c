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

#ifdef _WIN32 // Windows

#define CreateSocket()
#else // POSIX

#define CreateSocket()
#endif

int main(int argc, char **argv)
{
    printf("test");
}
