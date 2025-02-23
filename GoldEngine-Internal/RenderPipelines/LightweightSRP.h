/// <summary>
/// RenderPipeline used for less powerful devices
/// Does not contain any kind of illumination
/// </summary>

namespace Engine::Render::Pipelines
{
    public ref class LightweightSRP : Engine::Render::ScriptableRenderPipeline
    {
    private:
        System::Reflection::MethodInfo^ patch;

    public:
        LightweightSRP() : Engine::Render::ScriptableRenderPipeline()
        {
            HarmonyLib::Harmony^ harmonyInst = Singleton<HarmonyLib::Harmony^>::Instance;

            System::Reflection::MethodInfo^ originalMethod = Engine::EngineObjects::LightManager::typeid->GetMethod("LightUpdate");

            System::Reflection::MethodInfo^ prefixMethod = LightweightSRP::typeid->GetMethod("LightPrefix");

            patch = harmonyInst->Patch(originalMethod, prefixMethod, nullptr, nullptr, nullptr);
        }

    public:
        static bool LightPrefix()
        {
            return false;
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
            HarmonyLib::Harmony^ harmonyInst = Singleton<HarmonyLib::Harmony^>::Instance;
            System::Reflection::MethodInfo^ originalMethod = Engine::EngineObjects::LightManager::typeid->GetMethod("LightUpdate");

            harmonyInst->Unpatch(originalMethod, patch);
        }

        void PreRenderStack() override
        {

        }

        void PostRenderStack() override
        {

        }
    };
}