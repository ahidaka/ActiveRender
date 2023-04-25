// ActiveRender.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

//#include <iostream>
#include <Windows.h>
#include <stdio.h>
#include "RenderRegistry.h"

//
//
//
int main()
{
    WCHAR DeviceGuid[MAX_PATH];

    //std::cout << "Hello Render!\n";

    RenderRegistry *Rr = new RenderRegistry();

    Rr->ActiveRender(DeviceGuid);

    printf("Active Device: %ws\n", DeviceGuid);

    return 0;
}
