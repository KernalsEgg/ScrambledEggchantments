#pragma once

#include "RE/Skyrim.h"
#include "REL/Relocation.h"
#include "SKSE/SKSE.h"

#include <spdlog/sinks/basic_file_sink.h>

using namespace std::literals;

#include <cmath> // std::floor, std::round
#include <cstring> // std::memcmp

#include "Offsets.h"
#include "Settings.h"

#define DLLEXPORT __declspec(dllexport)
