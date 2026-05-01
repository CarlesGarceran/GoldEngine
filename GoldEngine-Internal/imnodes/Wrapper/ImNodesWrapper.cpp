#include "../Includes.h"
#include "ImNodesWrapper.h"
#include <msclr/marshal.h>
#include <msclr/marshal_cppstd.h>


struct MiniMapCallbackTrampoline
{
    msclr::gcroot<ImGuiNET::Nodes::MiniMapNodeHoveringCallback^> managedCallback;

    static void NativeCallback(int node_id, void* user_data)
    {
        MiniMapCallbackTrampoline* trampoline = (MiniMapCallbackTrampoline*)user_data;
        ImGuiNET::Nodes::MiniMapNodeHoveringCallback^ cb = trampoline->managedCallback;
        if (trampoline && cb != nullptr)
            cb->Invoke(node_id);
    }
};

using namespace msclr::interop;
using namespace System;
using namespace System::Text;

void ImGuiNET::Nodes::ImNodes::SetImGuiContext(System::IntPtr ctx)
{
	::ImNodes::SetImGuiContext((ImGuiContext*)ctx.ToPointer());
}

System::IntPtr ImGuiNET::Nodes::ImNodes::CreateContext()
{
	return IntPtr(::ImNodes::CreateContext());
}

void ImGuiNET::Nodes::ImNodes::DestroyContext(System::IntPtr ctx)
{
	::ImNodes::DestroyContext((ImNodesContext*)ctx.ToPointer());
}

System::IntPtr ImGuiNET::Nodes::ImNodes::GetCurrentContext()
{
	return IntPtr(::ImNodes::GetCurrentContext());
}

void ImGuiNET::Nodes::ImNodes::SetCurrentContext(System::IntPtr ctx)
{
	::ImNodes::SetCurrentContext((ImNodesContext*)ctx.ToPointer());
}

System::IntPtr ImGuiNET::Nodes::ImNodes::EditorContextCreate()
{
	return IntPtr(::ImNodes::EditorContextCreate());
}

void ImGuiNET::Nodes::ImNodes::EditorContextFree(System::IntPtr ctx)
{
	::ImNodes::EditorContextFree((ImNodesEditorContext*)ctx.ToPointer());
}

void ImGuiNET::Nodes::ImNodes::EditorContextSet(System::IntPtr ctx)
{
	::ImNodes::EditorContextSet((ImNodesEditorContext*)ctx.ToPointer());
}

System::Numerics::Vector2 ImGuiNET::Nodes::ImNodes::EditorContextGetPanning()
{
	const ImVec2& vec2 = ::ImNodes::EditorContextGetPanning();
	return System::Numerics::Vector2(vec2.x, vec2.y);
}

void ImGuiNET::Nodes::ImNodes::EditorContextResetPanning(System::Numerics::Vector2 pos)
{
	::ImNodes::EditorContextResetPanning(ImVec2(pos.X, pos.Y));
}

void ImGuiNET::Nodes::ImNodes::EditorContextMoveToNode(const int node_id)
{
	::ImNodes::EditorContextMoveToNode(node_id);
}

System::IntPtr ImGuiNET::Nodes::ImNodes::GetIO()
{
	return IntPtr(&::ImNodes::GetIO());
}

ImGuiNET::Nodes::ImNodesStyle^ ImGuiNET::Nodes::ImNodes::GetStyle()
{
	return gcnew ImGuiNET::Nodes::ImNodesStyle(&::ImNodes::GetStyle());
}

void ImGuiNET::Nodes::ImNodes::StyleColorsDark()
{
    ::ImNodes::StyleColorsDark();
}

void ImGuiNET::Nodes::ImNodes::StyleColorsClassic()
{
    ::ImNodes::StyleColorsClassic();
}

void ImGuiNET::Nodes::ImNodes::StyleColorsLight()
{
    ::ImNodes::StyleColorsLight();
}

void ImGuiNET::Nodes::ImNodes::StyleColorsDark(ImNodesStyle^ dest)
{
    ::ImNodes::StyleColorsDark(dest->StylePointer);
}

void ImGuiNET::Nodes::ImNodes::StyleColorsClassic(ImNodesStyle^ dest)
{
    ::ImNodes::StyleColorsClassic(dest->StylePointer);
}

void ImGuiNET::Nodes::ImNodes::StyleColorsLight(ImNodesStyle^ dest)
{
    ::ImNodes::StyleColorsLight(dest->StylePointer);
}

void ImGuiNET::Nodes::ImNodes::BeginNodeEditor()
{
    ::ImNodes::BeginNodeEditor();
}

void ImGuiNET::Nodes::ImNodes::EndNodeEditor()
{
    ::ImNodes::EndNodeEditor();
}

void ImGuiNET::Nodes::ImNodes::MiniMap(float minimapSizeFraction, ImNodesMiniMapLocation location, MiniMapNodeHoveringCallback^ callback)
{
    MiniMapCallbackTrampoline trampoline;
    trampoline.managedCallback = callback;

    ::ImNodes::MiniMap(
        minimapSizeFraction,
        (::ImNodesMiniMapLocation)location,
        callback ? MiniMapCallbackTrampoline::NativeCallback : nullptr,
        callback ? &trampoline : nullptr
    );
}

void ImGuiNET::Nodes::ImNodes::PushColorStyle(ImNodesCol item, unsigned int color)
{
    ::ImNodes::PushColorStyle((int)item, color);
}

void ImGuiNET::Nodes::ImNodes::PopColorStyle()
{
    ::ImNodes::PopColorStyle();
}

void ImGuiNET::Nodes::ImNodes::PushStyleVar(ImNodesStyleVar style_item, float value)
{
    ::ImNodes::PushStyleVar((int)style_item, value);
}

void ImGuiNET::Nodes::ImNodes::PushStyleVar(ImNodesStyleVar style_item, System::Numerics::Vector2 value)
{
    ::ImNodes::PushStyleVar((int)style_item, ImVec2(value.X, value.Y));
}

void ImGuiNET::Nodes::ImNodes::PopStyleVar(int count)
{
    ::ImNodes::PopStyleVar(count);
}

void ImGuiNET::Nodes::ImNodes::BeginNode(int id)
{
    ::ImNodes::BeginNode(id);
}

void ImGuiNET::Nodes::ImNodes::EndNode()
{
    ::ImNodes::EndNode();
}

System::Numerics::Vector2 ImGuiNET::Nodes::ImNodes::GetNodeDimensions(int id)
{
    const ImVec2& vec = ::ImNodes::GetNodeDimensions(id);
    return System::Numerics::Vector2(vec.x, vec.y);
}

void ImGuiNET::Nodes::ImNodes::BeginNodeTitleBar()
{
    ::ImNodes::BeginNodeTitleBar();
}

void ImGuiNET::Nodes::ImNodes::EndNodeTitleBar()
{
    ::ImNodes::EndNodeTitleBar();
}

void ImGuiNET::Nodes::ImNodes::BeginInputAttribute(int id, ImNodesPinShape shape)
{
    ::ImNodes::BeginInputAttribute(id, (int)shape);
}

void ImGuiNET::Nodes::ImNodes::EndInputAttribute()
{
    ::ImNodes::EndInputAttribute();
}

void ImGuiNET::Nodes::ImNodes::BeginOutputAttribute(int id, ImNodesPinShape shape)
{
    ::ImNodes::BeginOutputAttribute(id, (int)shape);
}

void ImGuiNET::Nodes::ImNodes::EndOutputAttribute()
{
    ::ImNodes::EndOutputAttribute();
}

void ImGuiNET::Nodes::ImNodes::BeginStaticAttribute(int id)
{
    ::ImNodes::BeginStaticAttribute(id);
}

void ImGuiNET::Nodes::ImNodes::EndStaticAttribute()
{
    ::ImNodes::EndStaticAttribute();
}

void ImGuiNET::Nodes::ImNodes::PushAttributeFlag(ImNodesAttributeFlags flag)
{
    ::ImNodes::PushAttributeFlag((int)flag);
}

void ImGuiNET::Nodes::ImNodes::PopAttributeFlag()
{
    ::ImNodes::PopAttributeFlag();
}

void ImGuiNET::Nodes::ImNodes::Link(int id, int start_attribute_id, int end_attribute_id)
{
    ::ImNodes::Link(id, start_attribute_id, end_attribute_id);
}

void ImGuiNET::Nodes::ImNodes::SetNodeDraggable(int node_id, const bool draggable)
{
    ::ImNodes::SetNodeDraggable(node_id, draggable);
}

void ImGuiNET::Nodes::ImNodes::SetNodeScreenSpacePos(int node_id, System::Numerics::Vector2 screen_space_pos)
{
    ::ImNodes::SetNodeScreenSpacePos(node_id, ImVec2(screen_space_pos.X, screen_space_pos.Y));
}

void ImGuiNET::Nodes::ImNodes::SetNodeEditorSpacePos(int node_id, System::Numerics::Vector2 editor_space_pos)
{;
    ::ImNodes::SetNodeEditorSpacePos(node_id, ImVec2(editor_space_pos.X, editor_space_pos.Y));
}

void ImGuiNET::Nodes::ImNodes::SetNodeGridSpacePos(int node_id, System::Numerics::Vector2 grid_pos)
{
    ::ImNodes::SetNodeGridSpacePos(node_id, ImVec2(grid_pos.X, grid_pos.Y));
}

System::Numerics::Vector2 ImGuiNET::Nodes::ImNodes::GetNodeScreenSpacePos(const int node_id)
{
    ImVec2 v2 = ::ImNodes::GetNodeScreenSpacePos(node_id);
    return System::Numerics::Vector2(v2.x, v2.y);
}

System::Numerics::Vector2 ImGuiNET::Nodes::ImNodes::GetNodeEditorSpacePos(const int node_id)
{
    ImVec2 v2 = ::ImNodes::GetNodeEditorSpacePos(node_id);
    return System::Numerics::Vector2(v2.x, v2.y);
}

System::Numerics::Vector2 ImGuiNET::Nodes::ImNodes::GetNodeGridSpacePos(const int node_id)
{
    ImVec2 v2 = ::ImNodes::GetNodeGridSpacePos(node_id);
    return System::Numerics::Vector2(v2.x, v2.y);
}

void ImGuiNET::Nodes::ImNodes::SnapNodeToGrid(int node_id)
{
    ::ImNodes::SnapNodeToGrid(node_id);
}

bool ImGuiNET::Nodes::ImNodes::IsEditorHovered()
{
    return ::ImNodes::IsEditorHovered();
}

bool ImGuiNET::Nodes::ImNodes::IsNodeHovered(int% node_id)
{
    int localId = node_id;
    bool hovered = ::ImNodes::IsNodeHovered(&localId);

    node_id = localId;

    return hovered;
}

bool ImGuiNET::Nodes::ImNodes::IsLinkHovered(int% link_id)
{
    int localId = link_id;
    bool hovered = ::ImNodes::IsLinkHovered(&localId);

    link_id = localId;

    return hovered;
}

bool ImGuiNET::Nodes::ImNodes::IsPinHovered(int% attribute_id)
{
    int localId = attribute_id;
    bool hovered = ::ImNodes::IsPinHovered(&localId);

    attribute_id = localId;

    return hovered;
}

int ImGuiNET::Nodes::ImNodes::NumSelectedNodes()
{
    return ::ImNodes::NumSelectedNodes();
}

int ImGuiNET::Nodes::ImNodes::NumSelectedLinks()
{
    return ::ImNodes::NumSelectedLinks();
}

void ImGuiNET::Nodes::ImNodes::GetSelectedNodes(cli::array<int>^ node_ids)
{
    if (node_ids == nullptr || node_ids->Length == 0)
        return;

    pin_ptr<int> pinnedArray = &node_ids[0];
    ::ImNodes::GetSelectedNodes(pinnedArray);
}

void ImGuiNET::Nodes::ImNodes::GetSelectedLinks(cli::array<int>^ link_ids)
{
    if (link_ids == nullptr || link_ids->Length == 0)
        return;

    pin_ptr<int> pinnedArray = &link_ids[0];
    ::ImNodes::GetSelectedLinks(pinnedArray);
}

void ImGuiNET::Nodes::ImNodes::ClearNodeSelection()
{
    ::ImNodes::ClearNodeSelection();
}

void ImGuiNET::Nodes::ImNodes::ClearLinkSelection()
{
    ::ImNodes::ClearLinkSelection();
}

void ImGuiNET::Nodes::ImNodes::SelectNode(int node_id)
{
    ::ImNodes::SelectNode(node_id);
}

void ImGuiNET::Nodes::ImNodes::ClearNodeSelection(int node_id)
{
    ::ImNodes::ClearNodeSelection(node_id);
}

bool ImGuiNET::Nodes::ImNodes::IsNodeSelected(int node_id)
{
    return ::ImNodes::IsNodeSelected(node_id);
}

void ImGuiNET::Nodes::ImNodes::SelectLink(int link_id)
{
    ::ImNodes::SelectLink(link_id);
}

void ImGuiNET::Nodes::ImNodes::ClearLinkSelection(int link_id)
{
    ::ImNodes::ClearLinkSelection(link_id);
}

bool ImGuiNET::Nodes::ImNodes::IsLinkSelected(int link_id)
{
    return ::ImNodes::IsLinkSelected(link_id);;
}

bool ImGuiNET::Nodes::ImNodes::IsAttributeActive()
{
    return ::ImNodes::IsAttributeActive();
}

bool ImGuiNET::Nodes::ImNodes::IsAnyAttributeActive(int% attribute_id)
{
    int attrib = 0;
    bool result = ::ImNodes::IsAnyAttributeActive(&attrib);
    attribute_id = attrib;
    return result;
}

bool ImGuiNET::Nodes::ImNodes::IsLinkStarted(int% started_at_attribute_id)
{
    int inner_value = started_at_attribute_id;
    bool result = ::ImNodes::IsAnyAttributeActive(&inner_value);
    started_at_attribute_id = inner_value;
    return result;
}

bool ImGuiNET::Nodes::ImNodes::IsLinkDropped(int% started_at_attribute_id, bool including_detached_links)
{
    int local_num = started_at_attribute_id;
    bool result = ::ImNodes::IsLinkDropped(&local_num, including_detached_links);
    started_at_attribute_id = local_num;
    return result;
}

bool ImGuiNET::Nodes::ImNodes::IsLinkCreated(int% started_at_attribute_id, int% ended_at_attribute_id)
{
    int stAttr = started_at_attribute_id, enAttr = ended_at_attribute_id;
    bool result = ::ImNodes::IsLinkCreated(&stAttr, &enAttr, (bool*)0);
    started_at_attribute_id = stAttr;
    ended_at_attribute_id = enAttr;
    return result;
}

bool ImGuiNET::Nodes::ImNodes::IsLinkCreated(int% started_at_attribute_id, int% ended_at_attribute_id, bool% created_from_snap)
{
    int stAttr = started_at_attribute_id, enAttr = ended_at_attribute_id;
    bool createdSnap = created_from_snap;
    bool result = ::ImNodes::IsLinkCreated(&stAttr, &enAttr, &createdSnap);
    started_at_attribute_id = stAttr;
    ended_at_attribute_id = enAttr;
    created_from_snap = createdSnap;
    return result;
}

bool ImGuiNET::Nodes::ImNodes::IsLinkCreated(int% started_at_node_id, int% started_at_attribute_id, int% ended_at_node_id, int% ended_at_attribute_id, bool% created_from_snap)
{
    int stNode = started_at_node_id, stAttr = started_at_attribute_id, endNode = ended_at_node_id, endAttr = ended_at_attribute_id;
    bool createdSnap = created_from_snap;
    bool result = ::ImNodes::IsLinkCreated(&stNode, &stAttr, &endNode, &endAttr, &createdSnap);
    started_at_node_id = stNode;
    started_at_attribute_id = stAttr;
    ended_at_node_id = endNode;
    ended_at_attribute_id = endAttr;
    return result;
}

bool ImGuiNET::Nodes::ImNodes::IsLinkDestroyed(int% link_id)
{
    int inner = link_id;
    bool result = ::ImNodes::IsLinkDestroyed(&inner);
    link_id = inner;
    return result;
}

String^ ImGuiNET::Nodes::ImNodes::SaveCurrentEditorStateToString()
{
    size_t size = 0;
    const char* str = ::ImNodes::SaveCurrentEditorStateToIniString(&size);
    if (!str || size == 0) return nullptr;

    // length-aware string
    return gcnew String(str, 0, (int)size, Encoding::UTF8);
}

void ImGuiNET::Nodes::ImNodes::LoadCurrentEditorStateFromString(String^ data)
{
    if (data == nullptr) return;

    array<unsigned char>^ bytes = Encoding::UTF8->GetBytes(data);
    pin_ptr<unsigned char> pinned = &bytes[0];

    ::ImNodes::LoadCurrentEditorStateFromIniString((const char*)pinned, bytes->Length);
}

void ImGuiNET::Nodes::ImNodes::SaveCurrentEditorStateToFile(String^ filename)
{
    if (filename == nullptr) return;

    marshal_context context;
    ::ImNodes::SaveCurrentEditorStateToIniFile(context.marshal_as<const char*>(filename));
}

void ImGuiNET::Nodes::ImNodes::LoadCurrentEditorStateFromFile(String^ filename)
{
    if (filename == nullptr) return;

    marshal_context context;
    ::ImNodes::LoadCurrentEditorStateFromIniFile(context.marshal_as<const char*>(filename));
}

// ------------------- Specific Editor Context -------------------

String^ ImGuiNET::Nodes::ImNodes::SaveEditorStateToString(IntPtr editorContext)
{
    if (editorContext == IntPtr::Zero) return nullptr;

    size_t size = 0;
    const char* str = ::ImNodes::SaveEditorStateToIniString(
        (const ImNodesEditorContext*)editorContext.ToPointer(),
        &size
    );

    if (!str || size == 0) return nullptr;
    return gcnew String(str, 0, (int)size, Encoding::UTF8);
}

void ImGuiNET::Nodes::ImNodes::LoadEditorStateFromString(IntPtr editorContext, System::String^ data)
{
    if (editorContext == IntPtr::Zero || data == nullptr) return;

    array<unsigned char>^ bytes = Encoding::UTF8->GetBytes(data);
    pin_ptr<unsigned char> pinned = &bytes[0];

    ::ImNodes::LoadEditorStateFromIniString(
        (ImNodesEditorContext*)editorContext.ToPointer(),
        (const char*)pinned,
        bytes->Length
    );
}

void ImGuiNET::Nodes::ImNodes::SaveEditorStateToFile(IntPtr editorContext, System::String^ filename)
{
    if (editorContext == IntPtr::Zero || filename == nullptr) return;

    marshal_context context;
    ::ImNodes::SaveEditorStateToIniFile(
        (const ImNodesEditorContext*)editorContext.ToPointer(),
        context.marshal_as<const char*>(filename)
    );
}

void ImGuiNET::Nodes::ImNodes::LoadEditorStateFromFile(IntPtr editorContext, String^ filename)
{
    if (editorContext == IntPtr::Zero || filename == nullptr) return;

    marshal_context context;
    ::ImNodes::LoadEditorStateFromIniFile(
        (ImNodesEditorContext*)editorContext.ToPointer(),
        context.marshal_as<const char*>(filename)
    );
}





// PROPERTIES GETTERS & SETTERS \\

namespace ImGuiNET::Nodes {

    // Constructor
    ImNodesStyle::ImNodesStyle(::ImNodesStyle* pointer)
    {
        StylePointer = pointer;
    }

    // === General Style Properties ===
    float ImNodesStyle::GridSpacing::get() { return StylePointer->GridSpacing; }
    void ImNodesStyle::GridSpacing::set(float value) { StylePointer->GridSpacing = value; }

    float ImNodesStyle::NodeCornerRounding::get() { return StylePointer->NodeCornerRounding; }
    void ImNodesStyle::NodeCornerRounding::set(float value) { StylePointer->NodeCornerRounding = value; }

    System::Numerics::Vector2 ImNodesStyle::NodePadding::get() { return System::Numerics::Vector2(StylePointer->NodePadding.x, StylePointer->NodePadding.y); }
    void ImNodesStyle::NodePadding::set(System::Numerics::Vector2 value) { StylePointer->NodePadding = ImVec2(value.X, value.Y); }

    float ImNodesStyle::NodeBorderThickness::get() { return StylePointer->NodeBorderThickness; }
    void ImNodesStyle::NodeBorderThickness::set(float value) { StylePointer->NodeBorderThickness = value; }

    float ImNodesStyle::LinkThickness::get() { return StylePointer->LinkThickness; }
    void ImNodesStyle::LinkThickness::set(float value) { StylePointer->LinkThickness = value; }

    float ImNodesStyle::LinkLineSegmentsPerLength::get() { return StylePointer->LinkLineSegmentsPerLength; }
    void ImNodesStyle::LinkLineSegmentsPerLength::set(float value) { StylePointer->LinkLineSegmentsPerLength = value; }

    float ImNodesStyle::LinkHoverDistance::get() { return StylePointer->LinkHoverDistance; }
    void ImNodesStyle::LinkHoverDistance::set(float value) { StylePointer->LinkHoverDistance = value; }

    // === Pin Properties ===
    float ImNodesStyle::PinCircleRadius::get() { return StylePointer->PinCircleRadius; }
    void ImNodesStyle::PinCircleRadius::set(float value) { StylePointer->PinCircleRadius = value; }

    float ImNodesStyle::PinQuadSideLength::get() { return StylePointer->PinQuadSideLength; }
    void ImNodesStyle::PinQuadSideLength::set(float value) { StylePointer->PinQuadSideLength = value; }

    float ImNodesStyle::PinTriangleSideLength::get() { return StylePointer->PinTriangleSideLength; }
    void ImNodesStyle::PinTriangleSideLength::set(float value) { StylePointer->PinTriangleSideLength = value; }

    float ImNodesStyle::PinLineThickness::get() { return StylePointer->PinLineThickness; }
    void ImNodesStyle::PinLineThickness::set(float value) { StylePointer->PinLineThickness = value; }

    float ImNodesStyle::PinHoverRadius::get() { return StylePointer->PinHoverRadius; }
    void ImNodesStyle::PinHoverRadius::set(float value) { StylePointer->PinHoverRadius = value; }

    float ImNodesStyle::PinOffset::get() { return StylePointer->PinOffset; }
    void ImNodesStyle::PinOffset::set(float value) { StylePointer->PinOffset = value; }

    // === Mini-map ===
    System::Numerics::Vector2 ImNodesStyle::MiniMapPadding::get() { return System::Numerics::Vector2(StylePointer->MiniMapPadding.x, StylePointer->MiniMapPadding.y); }
    void ImNodesStyle::MiniMapPadding::set(System::Numerics::Vector2 value) { StylePointer->MiniMapPadding = ImVec2(value.X, value.Y); }

    System::Numerics::Vector2 ImNodesStyle::MiniMapOffset::get() { return System::Numerics::Vector2(StylePointer->MiniMapOffset.x, StylePointer->MiniMapOffset.y); }
    void ImNodesStyle::MiniMapOffset::set(System::Numerics::Vector2 value) { StylePointer->MiniMapOffset = ImVec2(value.X, value.Y); }

    // === Flags ===
    ImNodesStyleFlags ImNodesStyle::Flags::get() {

        return (ImNodesStyleFlags)((int)StylePointer->Flags);
    }
    void ImNodesStyle::Flags::set(ImNodesStyleFlags value) { StylePointer->Flags = (int)value; }

    // === Colors ===
    cli::array<unsigned int>^ ImNodesStyle::Colors::get()
    {
        cli::array<unsigned int>^ arr = gcnew cli::array<unsigned int>(ImNodesCol_COUNT);
        for (int i = 0; i < ImNodesCol_COUNT; i++)
            arr[i] = StylePointer->Colors[i];
        return arr;
    }

    void ImNodesStyle::Colors::set(cli::array<unsigned int>^ value)
    {
        if (value == nullptr) return;
        int count = value->Length < ImNodesCol_COUNT ? value->Length : ImNodesCol_COUNT;
        for (int i = 0; i < count; i++)
            StylePointer->Colors[i] = value[i];
    }
}