#pragma once
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Table.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Multiline_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Pack.H>
#include <FL/Fl_Tabs.H>
#include <FL/fl_ask.H>
#include <FL/fl_draw.H>

#include <stdint.h>
#include <assert.h>
#include <string>
#include <vector>
#include <filesystem>
#include <mutex>
#include <functional>
#include <algorithm>

#include "plog/Log.h"

// Changing product name and major version will affect the compatibility with previous versions
#define VER_MAJOR 0
#define VER_MINOR 1
#define PRODUCT_NAME "Elwood's Passtore"
