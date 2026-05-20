#pragma once
#include <Arduino.h>

extern volatile bool g_buttonEdgeFlag;

void interrupts_init();