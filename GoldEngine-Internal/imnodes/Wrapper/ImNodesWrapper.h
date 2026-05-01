#pragma once

namespace ImGuiNET::Nodes
{
    public delegate void MiniMapNodeHoveringCallback(int nodeId);

	public enum class ImNodesCol
	{
        NodeBackground = 0,
        NodeBackgroundHovered,
        NodeBackgroundSelected,
        NodeOutline,
        TitleBar,
        TitleBarHovered,
        TitleBarSelected,
        Link,
        LinkHovered,
        LinkSelected,
        Pin,
        PinHovered,
        BoxSelector,
        BoxSelectorOutline,
        GridBackground,
        GridLine,
        GridLinePrimary,
        MiniMapBackground,
        MiniMapBackgroundHovered,
        MiniMapOutline,
        MiniMapOutlineHovered,
        MiniMapNodeBackground,
        MiniMapNodeBackgroundHovered,
        MiniMapNodeBackgroundSelected,
        MiniMapNodeOutline,
        MiniMapLink,
        MiniMapLinkSelected,
        MiniMapCanvas,
        MiniMapCanvasOutline,
        COUNT
	};

    public enum class ImNodesStyleVar 
    {
        GridSpacing = 0,
        NodeCornerRounding,
        NodePadding,
        NodeBorderThickness,
        LinkThickness,
        LinkLineSegmentsPerLength,
        LinkHoverDistance,
        PinCircleRadius,
        PinQuadSideLength,
        PinTriangleSideLength,
        PinLineThickness,
        PinHoverRadius,
        PinOffset,
        MiniMapPadding,
        MiniMapOffset,
        COUNT
    };

    public enum class ImNodesStyleFlags
    {
        None = 0,
        NodeOutline = 1 << 0,
        GridLines = 1 << 2,
        GridLinesPrimary = 1 << 3,
        GridSnapping = 1 << 4
    };

    public enum class ImNodesPinShape
    {
        Circle,
        CircleFilled,
        Triangle,
        TriangleFilled,
        Quad,
        QuadFilled
    };

    public enum class ImNodesAttributeFlags
    {
        None = 0,
        EnableLinkDetachWithDragClick = 1 << 0,
        EnableLinkCreationOnSnap = 1 << 1
    };

    public enum class ImNodesMiniMapLocation
    {
        BottomLeft,
        BottomRight,
        TopLeft,
        TopRight,
    };

    public ref class ImNodesStyle
    {
    internal:
        ::ImNodesStyle* StylePointer;

    public:
        ImNodesStyle(::ImNodesStyle* pointer);

        // === General Style ===
        property float GridSpacing
        {
            float get();
            void set(float value);
        }

        property float NodeCornerRounding
        {
            float get();
            void set(float value);
        }

        property System::Numerics::Vector2 NodePadding
        {
            System::Numerics::Vector2 get();
            void set(System::Numerics::Vector2 value);
        }

        property float NodeBorderThickness
        {
            float get();
            void set(float value);
        }

        property float LinkThickness
        {
            float get();
            void set(float value);
        }

        property float LinkLineSegmentsPerLength
        {
            float get();
            void set(float value);
        }

        property float LinkHoverDistance
        {
            float get();
            void set(float value);
        }

        // === Pin Properties ===
        property float PinCircleRadius
        {
            float get();
            void set(float value);
        }

        property float PinQuadSideLength
        {
            float get();
            void set(float value);
        }

        property float PinTriangleSideLength
        {
            float get();
            void set(float value);
        }

        property float PinLineThickness
        {
            float get();
            void set(float value);
        }

        property float PinHoverRadius
        {
            float get();
            void set(float value);
        }

        property float PinOffset
        {
            float get();
            void set(float value);
        }

        // === Mini-map ===
        property System::Numerics::Vector2 MiniMapPadding
        {
            System::Numerics::Vector2 get();
            void set(System::Numerics::Vector2 value);
        }

        property System::Numerics::Vector2 MiniMapOffset
        {
            System::Numerics::Vector2 get();
            void set(System::Numerics::Vector2 value);
        }

        // === Flags ===
        property ImNodesStyleFlags Flags
        {
            ImNodesStyleFlags get();
            void set(ImNodesStyleFlags value);
        }

        // === Colors ===
        property cli::array<unsigned int>^ Colors
        {
            cli::array<unsigned int>^ get();
            void set(cli::array<unsigned int>^ value);
        }
    };

	public ref class ImNodes
	{
    public:
        static void SetImGuiContext(System::IntPtr ctx);

        static System::IntPtr CreateContext();
        static void DestroyContext(System::IntPtr ctx);
        static System::IntPtr GetCurrentContext();
        static void SetCurrentContext(System::IntPtr ctx);

        static System::IntPtr EditorContextCreate();
        static void EditorContextFree(System::IntPtr ctx);
        static void EditorContextSet(System::IntPtr ctx);
        static System::Numerics::Vector2 EditorContextGetPanning();
        static void EditorContextResetPanning(System::Numerics::Vector2 pos);
        static void EditorContextMoveToNode(const int node_id);

        static System::IntPtr GetIO();
        static ImNodesStyle^ GetStyle();

        static void StyleColorsDark();
        static void StyleColorsClassic();
        static void StyleColorsLight();

        static void StyleColorsDark(ImNodesStyle^ dest);
        static void StyleColorsClassic(ImNodesStyle^ dest);
        static void StyleColorsLight(ImNodesStyle^ dest);

        static void BeginNodeEditor();
        static void EndNodeEditor();

        static void MiniMap(
            float minimapSizeFraction,
            ImNodesMiniMapLocation location,
            MiniMapNodeHoveringCallback^ callback
        );

        static void PushColorStyle(ImNodesCol item, unsigned int color);
        static void PopColorStyle();
        static void PushStyleVar(ImNodesStyleVar style_item, float value);
        static void PushStyleVar(ImNodesStyleVar style_item, System::Numerics::Vector2 value);
        static void PopStyleVar() { PopStyleVar(1); }
        static void PopStyleVar(int count);

        static void BeginNode(int id);
        static void EndNode();

        static System::Numerics::Vector2 GetNodeDimensions(int id);
        static void BeginNodeTitleBar();
        static void EndNodeTitleBar();

        static void BeginInputAttribute(int id) { BeginInputAttribute(id, ImNodesPinShape::CircleFilled); }
        static void BeginInputAttribute(int id, ImNodesPinShape shape);
        static void EndInputAttribute();

        static void BeginOutputAttribute(int id) { BeginOutputAttribute(id, ImNodesPinShape::CircleFilled); }
        static void BeginOutputAttribute(int id, ImNodesPinShape shape);
        static void EndOutputAttribute();

        static void BeginStaticAttribute(int id);
        static void EndStaticAttribute();

        static void PushAttributeFlag(ImNodesAttributeFlags flag);
        static void PopAttributeFlag();

        static void Link(int id, int start_attribute_id, int end_attribute_id);
        static void SetNodeDraggable(int node_id, const bool draggable);
        static void SetNodeScreenSpacePos(int node_id, System::Numerics::Vector2 screen_space_pos);
        static void SetNodeEditorSpacePos(int node_id, System::Numerics::Vector2 editor_space_pos);
        static void SetNodeGridSpacePos(int node_id, System::Numerics::Vector2 grid_pos);
	
        static System::Numerics::Vector2 GetNodeScreenSpacePos(const int node_id);
        static System::Numerics::Vector2 GetNodeEditorSpacePos(const int node_id);
        static System::Numerics::Vector2 GetNodeGridSpacePos(const int node_id);

        static void SnapNodeToGrid(int node_id);
        static bool IsEditorHovered();

        static bool IsNodeHovered(int% node_id);
        static bool IsLinkHovered(int% link_id);
        static bool IsPinHovered(int% attribute_id);

        static int NumSelectedNodes();
        static int NumSelectedLinks();

        static void GetSelectedNodes(cli::array<int>^ node_ids);
        static void GetSelectedLinks(cli::array<int>^ link_ids);

        static void ClearNodeSelection();
        static void ClearLinkSelection();

        static void SelectNode(int node_id);
        static void ClearNodeSelection(int node_id);
        static bool IsNodeSelected(int node_id);
        static void SelectLink(int link_id);
        static void ClearLinkSelection(int link_id);
        static bool IsLinkSelected(int link_id);

        static bool IsAttributeActive();
        static bool IsAnyAttributeActive() { int v = 0; return IsAnyAttributeActive(v); }
        static bool IsAnyAttributeActive(int% attribute_id);

        static bool IsLinkStarted(int% started_at_attribute_id);

        static bool IsLinkDropped() { int v = 0; return IsLinkDropped(v); }
        static bool IsLinkDropped(int% started_at_attribute_id) { return IsLinkDropped(started_at_attribute_id, true); }
        static bool IsLinkDropped(int% started_at_attribute_id, bool including_detached_links);


        static bool IsLinkCreated(
            int% started_at_attribute_id,
            int% ended_at_attribute_id);

        static bool IsLinkCreated(
            int% started_at_attribute_id,
            int% ended_at_attribute_id,
            bool% created_from_snap);

        static bool IsLinkCreated(
            int% started_at_node_id,
            int% started_at_attribute_id,
            int% ended_at_node_id,
            int% ended_at_attribute_id,
            bool% created_from_snap);

        static bool IsLinkDestroyed(int% link_id);


        static System::String^ SaveCurrentEditorStateToString();
        static void LoadCurrentEditorStateFromString(System::String^ data);

        static void SaveCurrentEditorStateToFile(System::String^ filename);
        static void LoadCurrentEditorStateFromFile(System::String^ filename);

        static System::String^ SaveEditorStateToString(System::IntPtr editorContext);
        static void LoadEditorStateFromString(System::IntPtr editorContext, System::String^ data);

        static void SaveEditorStateToFile(System::IntPtr editorContext, System::String^ filename);
        static void LoadEditorStateFromFile(System::IntPtr editorContext, System::String^ filename);
    };
}