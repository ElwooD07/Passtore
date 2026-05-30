#pragma once
#include <nana/gui.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/listbox.hpp>
#include <nana/gui/widgets/panel.hpp>
#include <nana/gui/widgets/menubar.hpp>
#include <nana/gui/msgbox.hpp>

#include <stdint.h>
#include <assert.h>
#include <string>
#include <vector>
#include <filesystem>
#include <mutex>
#include <functional>
#include <algorithm>
#include <cstring>

#include "plog/Log.h"

// Changing product name and major version will affect the compatibility with previous versions
#define VER_MAJOR 0
#define VER_MINOR 1
#define PRODUCT_NAME "Elwood's Passtore"
