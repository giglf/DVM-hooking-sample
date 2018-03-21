#include "MethodHooker.h"

extern "C" void hook_entry(){
    Hook();
}