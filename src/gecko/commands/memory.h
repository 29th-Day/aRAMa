#pragma once

#include "commands.h"
#include "../../tcp/server.h"

void Write8(const Socket socket);

void ReadMemory(const Socket socket);

void ReadMemoryKernel(const Socket socket);

void ValidateAddressRange(const Socket socket);

void Search32(const Socket socket);

void AdvancedSearch(const Socket socket);
