#pragma once

#pragma managed(push, on)

// ATTRIBUTES \\

#include "ExecuteInEditModeAttribute.h"
#include "LuaAPI.h"
#include "Instantiable.h"

#include "Singleton.h"
#include <msclr/gcroot.h>
#include <msclr/lock.h>
#include <pplawait.h>
#include <ppltasks.h>
#include "EngineState.h"
#include "WinAPI.h"
#include "Cast.h"
#include "CastToNative.h"

#pragma managed(pop)