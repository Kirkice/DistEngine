#include <iostream>
#include "LogSystem/Log.h"

using namespace LogSystem;
int main() {
    Log log;
    log.Init();
    log.Debug("1111111111");
    return 0;
}
