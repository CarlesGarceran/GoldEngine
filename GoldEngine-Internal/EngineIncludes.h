#pragma once
#include "Includes.h"

// REFLECTION AND INTEROP \\

#include "Reflection/ReflectedType.h"
#include "Reflection/ReflectableType.h"

#include "Attribute.h"
#include "PropertyAttribute.h"
#include "AttributeManager.h"

// Structures \\

#include "Object/Color.h"

// INCLUDE ENGINE CLASSES \\

#include "SceneFormat.h"
#include "Objects/BoundingBoxRenderer.h"
#include "Objects/CubeRenderer.h"
#include "Objects/GridRenderer.h"
#include "Objects/ModelRenderer.h"
#include "Objects/Skybox.h"

// last class to be imported, it must access to all the components included \\

#include "Objects/ScriptBehaviour.h"
#include "Objects/LightManager.h"

#include "Objects/Daemon.h"
#include "Objects/Camera.h"
#include "Objects/Camera2D.h"
#include "Objects/Camera3D.h"

#if !defined(PRODUCTION_BUILD)
#include "Objects/EditorCamera.h"
#include "Objects/EditorPlugin.h"
#endif
// 2D ELEMENTS
#include "Objects/Sprite.h"
// GUI
#include "Objects/UI/Button.h"
#include "Objects/UI/Image.h"
