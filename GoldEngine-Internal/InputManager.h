#pragma once

#include "EngineState.h"

namespace Engine::Scripting
{
    [MoonSharp::Interpreter::MoonSharpUserDataAttribute]
        [Engine::Attributes::LuaAPIAttribute("MouseButton")]
    public ref class MouseButtons
    {
    public:
        static int const MOUSE_BUTTON_LEFT = 0;
        static int const MOUSE_BUTTON_RIGHT = 1;
        static int const MOUSE_BUTTON_MIDDLE = 2;
        static int const MOUSE_BUTTON_SIDE = 3;
        static int const MOUSE_BUTTON_EXTRA = 4;
        static int const MOUSE_BUTTON_FORWARD = 5;
        static int const MOUSE_BUTTON_BACK = 6;
    };

    [MoonSharp::Interpreter::MoonSharpUserDataAttribute]
    [Engine::Attributes::LuaAPIAttribute("KeyCode")]
    public ref class KeyCodes
    {
    public:
        static int const KEY_NULL = 0;      // Key: NULL; used for no key pressed
        // Alphanumeric keys
        static int const KEY_APOSTROPHE = 39;       // Key: '
        static int const KEY_COMMA = 44;       // Key: ;
        static int const KEY_MINUS = 45;       // Key: -
        static int const KEY_PERIOD = 46;       // Key: .
        static int const KEY_SLASH = 47;       // Key: /
        static int const KEY_ZERO = 48;       // Key: 0
        static int const KEY_ONE = 49;       // Key: 1
        static int const KEY_TWO = 50;       // Key: 2
        static int const KEY_THREE = 51;       // Key: 3
        static int const KEY_FOUR = 52;       // Key: 4
        static int const KEY_FIVE = 53;       // Key: 5
        static int const KEY_SIX = 54;       // Key: 6
        static int const KEY_SEVEN = 55;       // Key: 7
        static int const KEY_EIGHT = 56;       // Key: 8
        static int const KEY_NINE = 57;       // Key: 9
        static int const KEY_SEMICOLON = 59;       // Key: ;
        static int const KEY_EQUAL = 61;       // Key: =
        static int const KEY_A = 65;       // Key: A | a
        static int const KEY_B = 66;       // Key: B | b
        static int const KEY_C = 67;       // Key: C | c
        static int const KEY_D = 68;       // Key: D | d
        static int const KEY_E = 69;       // Key: E | e
        static int const KEY_F = 70;       // Key: F | f
        static int const KEY_G = 71;       // Key: G | g
        static int const KEY_H = 72;       // Key: H | h
        static int const KEY_I = 73;       // Key: I | i
        static int const KEY_J = 74;       // Key: J | j
        static int const KEY_K = 75;       // Key: K | k
        static int const KEY_L = 76;       // Key: L | l
        static int const KEY_M = 77;       // Key: M | m
        static int const KEY_N = 78;       // Key: N | n
        static int const KEY_O = 79;       // Key: O | o
        static int const KEY_P = 80;       // Key: P | p
        static int const KEY_Q = 81;       // Key: Q | q
        static int const KEY_R = 82;       // Key: R | r
        static int const KEY_S = 83;       // Key: S | s
        static int const KEY_T = 84;       // Key: T | t
        static int const KEY_U = 85;       // Key: U | u
        static int const KEY_V = 86;       // Key: V | v
        static int const KEY_W = 87;       // Key: W | w
        static int const KEY_X = 88;       // Key: X | x
        static int const KEY_Y = 89;       // Key: Y | y
        static int const KEY_Z = 90;       // Key: Z | z
        static int const KEY_LEFT_BRACKET = 91;       // Key: [
        static int const KEY_BACKSLASH = 92;       // Key: '\'
        static int const KEY_RIGHT_BRACKET = 93;       // Key: ]
        static int const KEY_GRAVE = 96;       // Key: `
        // Function keys
        static int const KEY_SPACE = 32;       // Key: Space
        static int const KEY_ESCAPE = 256;      // Key: Esc
        static int const KEY_ENTER = 257;      // Key: Enter
        static int const KEY_TAB = 258;      // Key: Tab
        static int const KEY_BACKSPACE = 259;      // Key: Backspace
        static int const KEY_INSERT = 260;      // Key: Ins
        static int const KEY_DELETE = 261;      // Key: Del
        static int const KEY_RIGHT = 262;      // Key: Cursor right
        static int const KEY_LEFT = 263;      // Key: Cursor left
        static int const KEY_DOWN = 264;      // Key: Cursor down
        static int const KEY_UP = 265;      // Key: Cursor up
        static int const KEY_PAGE_UP = 266;      // Key: Page up
        static int const KEY_PAGE_DOWN = 267;      // Key: Page down
        static int const KEY_HOME = 268;      // Key: Home
        static int const KEY_END = 269;      // Key: End
        static int const KEY_CAPS_LOCK = 280;      // Key: Caps lock
        static int const KEY_SCROLL_LOCK = 281;      // Key: Scroll down
        static int const KEY_NUM_LOCK = 282;      // Key: Num lock
        static int const KEY_PRINT_SCREEN = 283;      // Key: Print screen
        static int const KEY_PAUSE = 284;      // Key: Pause
        static int const KEY_F1 = 290;      // Key: F1
        static int const KEY_F2 = 291;      // Key: F2
        static int const KEY_F3 = 292;      // Key: F3
        static int const KEY_F4 = 293;      // Key: F4
        static int const KEY_F5 = 294;      // Key: F5
        static int const KEY_F6 = 295;      // Key: F6
        static int const KEY_F7 = 296;      // Key: F7
        static int const KEY_F8 = 297;      // Key: F8
        static int const KEY_F9 = 298;      // Key: F9
        static int const KEY_F10 = 299;      // Key: F10
        static int const KEY_F11 = 300;      // Key: F11
        static int const KEY_F12 = 301;      // Key: F12
        static int const KEY_LEFT_SHIFT = 340;      // Key: Shift left
        static int const KEY_LEFT_CONTROL = 341;      // Key: Control left
        static int const KEY_LEFT_ALT = 342;      // Key: Alt left
        static int const KEY_LEFT_SUPER = 343;      // Key: Super left
        static int const KEY_RIGHT_SHIFT = 344;      // Key: Shift right
        static int const KEY_RIGHT_CONTROL = 345;      // Key: Control right
        static int const KEY_RIGHT_ALT = 346;      // Key: Alt right
        static int const KEY_RIGHT_SUPER = 347;      // Key: Super right
        static int const KEY_KB_MENU = 348;      // Key: KB menu
        // Keypad keys
        static int const KEY_KP_0 = 320;      // Key: Keypad 0
        static int const KEY_KP_1 = 321;      // Key: Keypad 1
        static int const KEY_KP_2 = 322;      // Key: Keypad 2
        static int const KEY_KP_3 = 323;      // Key: Keypad 3
        static int const KEY_KP_4 = 324;      // Key: Keypad 4
        static int const KEY_KP_5 = 325;      // Key: Keypad 5
        static int const KEY_KP_6 = 326;      // Key: Keypad 6
        static int const KEY_KP_7 = 327;      // Key: Keypad 7
        static int const KEY_KP_8 = 328;      // Key: Keypad 8
        static int const KEY_KP_9 = 329;      // Key: Keypad 9
        static int const KEY_KP_DECIMAL = 330;      // Key: Keypad .
        static int const KEY_KP_DIVIDE = 331;      // Key: Keypad /
        static int const KEY_KP_MULTIPLY = 332;      // Key: Keypad *
        static int const KEY_KP_SUBTRACT = 333;      // Key: Keypad -
        static int const KEY_KP_ADD = 334;      // Key: Keypad +
        static int const KEY_KP_ENTER = 335;      // Key: Keypad Enter
        static int const KEY_KP_EQUAL = 336;      // Key: Keypad =
        // Android key buttons
        static int const KEY_BACK = 4;        // Key: Android back button
        static int const KEY_MENU = 82;       // Key: Android menu button
        static int const KEY_VOLUME_UP = 24;       // Key: Android volume up button
        static int const KEY_VOLUME_DOWN = 25;        // Key: Android volume down button
    };

    [MoonSharp::Interpreter::MoonSharpUserDataAttribute]
        [Engine::Attributes::LuaAPIAttribute("CursorStatus")]
    public ref class CursorStatus
    {
    public:
        static int Locked = 0;
        static int Unlocked = 1;
    };

    [MoonSharp::Interpreter::MoonSharpUserDataAttribute]
        [Engine::Attributes::LuaAPIAttribute("CursorVisibility")]
    public ref class CursorVisibility
    {
    public:
        static int Visible = 0;
        static int Hidden = 1;
    };

    [MoonSharp::Interpreter::MoonSharpUserDataAttribute]
        [Engine::Attributes::LuaAPIAttribute("Input")]
    public ref class InputManager
    {
    public:
        static bool IsKeyPressed(int key_id)
        {
            return RAYLIB::IsKeyPressed(key_id);
        }

        static bool IsKeyDown(int key_id)
        {

            return RAYLIB::IsKeyDown(key_id);
        }

        static bool IsKeyReleased(int key_id)
        {
            return RAYLIB::IsKeyReleased(key_id);
        }

        static bool IsKeyPressedRepeat(int key_id)
        {
            return RAYLIB::IsKeyPressedRepeat(key_id);
        }

        static bool IsMouseButtonUp(int button_press)
        {
            return RAYLIB::IsMouseButtonUp(button_press);
        }

        static bool IsMouseButtonDown(int button_press)
        {
            return RAYLIB::IsMouseButtonDown(button_press);
        }

        static bool IsMouseButtonPressed(int button_press)
        {
            return RAYLIB::IsMouseButtonPressed(button_press);
        }

        static Engine::Components::Vector2^ GetMouseDelta()
        {
            auto mdelta = RAYLIB::GetMouseDelta();
            return gcnew Engine::Components::Vector2(mdelta.x, mdelta.y);
        }

        static Engine::Components::Vector2^ GetMousePosition()
        {
#if !PRODUCTION_BUILD
            if (!EngineState::PlayMode)
            {
                ImVec2 mousePos = ImGui::GetMousePos();
                ImVec2 viewportPos = ImVec2(Screen::getX(), Screen::getY());
                ImVec2 viewportSize = ImVec2(Screen::Width, Screen::Height);

                ImGuiStyle style = ImGui::GetStyle();
                float paddingX = style.WindowPadding.x;
                float paddingY = style.WindowPadding.y;

                ImVec2 localMousePos;
                localMousePos.x = mousePos.x - viewportPos.x - paddingX;
                localMousePos.y = mousePos.y - viewportPos.y - paddingY;

                localMousePos.x = std::clamp(localMousePos.x, 0.0f, viewportSize.x);
                localMousePos.y = std::clamp(localMousePos.y, 0.0f, viewportSize.y);

                return gcnew Engine::Components::Vector2(localMousePos.x, localMousePos.y);
            }
            else
                return gcnew Engine::Components::Vector2(RAYLIB::GetMousePosition().x, RAYLIB::GetMousePosition().y);
#else
            return gcnew Engine::Components::Vector2(RAYLIB::GetMousePosition().x, RAYLIB::GetMousePosition().y);
#endif
        }

        static bool IsMouseButton1Up()
        {
            return RAYLIB::IsMouseButtonUp(MOUSE_BUTTON_LEFT);
        }
        static bool IsMouseButton2Up()
        {
            return RAYLIB::IsMouseButtonUp(MOUSE_BUTTON_RIGHT);
        }
        static bool IsMouseButton1Down()
        {
            return RAYLIB::IsMouseButtonDown(MOUSE_BUTTON_LEFT);
        }
        static bool IsMouseButton2Down()
        {
            return RAYLIB::IsMouseButtonDown(MOUSE_BUTTON_RIGHT);
        }
        static bool IsMouseButton1Pressed()
        {
            return RAYLIB::IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
        }
        static bool IsMouseButton2Pressed()
        {
            return RAYLIB::IsMouseButtonPressed(MOUSE_BUTTON_RIGHT);
        }
        static float GetMouseScroll()
        {
            return RAYLIB::GetMouseWheelMove();
        }
        static bool IsCursorOnScreen() { return RAYLIB::IsCursorOnScreen(); }
        static bool IsCursorOnHidden() { return RAYLIB::IsCursorHidden(); }

        static void SetCursorStatus(int status)
        {
            if (status == CursorStatus::Locked)
            {
                RAYLIB::DisableCursor();
                return;
            }

            if (status == CursorStatus::Unlocked)
            {
                RAYLIB::EnableCursor();
                return;
            }
        }

        static void SetCursorVisible(int visible)
        {
            if (visible == CursorVisibility::Visible)
            {
                RAYLIB::ShowCursor();
                return;
            }

            if (visible == CursorVisibility::Hidden)
            {
                RAYLIB::HideCursor();
                return;
            }
        }
    };
}