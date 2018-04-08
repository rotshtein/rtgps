#pragma once
#include "windows.h"
int __cdecl tcpdata_init(PCSTR ip, int port);
void tcpdata_close();
_Bool __cdecl tcpdata_send(char *p, int size);
_Bool __cdecl tcpdata_send_and_wait(char *p, int size);
