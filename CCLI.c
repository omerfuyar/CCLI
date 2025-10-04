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

#pragma region HashTable

#define MAP_SIZE 128

typedef char *(*RouteFunction)(char *);

RouteFunction ROUTE_MAP[MAP_SIZE] = {0};

int hash(const char *key)
{
    size_t strLength = strlen(key);

    int sum = 0;
    int mul = 1;

    for (int i = 0; i < strLength; i++)
    {
        mul = (i % 4 == 0) ? 1 : mul * 256;
        sum += key[i] * mul;
    }

    return sum % MAP_SIZE;
}

int mapRegister(const char *key, const RouteFunction value)
{
    int index = hash(key);

    ROUTE_MAP[index] = value;

    return index;
}

RouteFunction mapAccess(const char *key)
{
    int index = hash(key);

    return ROUTE_MAP[index];
}

#pragma endregion HashTable

char *test(char *param)
{
    printf("%s\n", param);
    return "bar";
}

int main(int argc, char **argv)
{
    printf("index for 'testasd' : %d\n", mapRegister("testasd", test));
    printf("fun return : %s\n", mapAccess("testasd")("foo"));

    return 0;
}