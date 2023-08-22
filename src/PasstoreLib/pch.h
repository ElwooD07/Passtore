#pragma once
#include <QtCore>

#include <stdint.h>
#include <assert.h>
#include <string>
#include <vector>
#include <filesystem>
#include <mutex>

#include "sqlite3.h"
#include "plog/Log.h"

// Changing product name and major version will affect the compatibility with previous versions
#define VER_MAJOR 0
#define VER_MINOR 1
#define PRODUCT_NAME "Elwood's Passtore"

#define USE_QT
