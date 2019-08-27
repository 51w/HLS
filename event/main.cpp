#include <iostream>
#include "HLS.h"

int main()
{
    int aa = (int)time(NULL);
    LOG(INFO) << aa;
    LOGI("%x\n", aa);

    HLSServer hls;
    hls.StartServer();
    
    return 0;
}