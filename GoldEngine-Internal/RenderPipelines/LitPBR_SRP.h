/// <summary>
/// Default Gold Engine RenderPipeline
/// LitPBR, only contains Lit (Direct Illumination) & PBR Rendering
/// </summary>

namespace Engine::Render::Pipelines
{
    public ref class LitPBR_SRP : Engine::Render::ScriptableRenderPipeline
    {
    public:
        LitPBR_SRP() : Engine::Render::ScriptableRenderPipeline()
        {

        }

    public:
        void OnRenderBegin() override
        {

        }

        void OnRenderEnd() override
        {

        }

        void PreRenderFrame() override
        {

        }

        void PostRenderFrame() override
        {

        }

        void PreRenderObjects() override
        {

        }

        void PostRenderObjects() override
        {

        }

        void PreRenderObject(Engine::Internal::Components::GameObject^ engineObject) override
        {
            engineObject->GameDraw();
        }

        void PostRenderObject() override
        {

        }

        void PreFirstPassRender(Engine::Management::Scene^ handle) override
        {

        }

        void PostFirstPassRender() override
        {

        }

        void OnUnloadPipeline() override
        {

        }

        void PreRenderStack() override
        {

        }

        void PostRenderStack() override
        {

        }
    };
}