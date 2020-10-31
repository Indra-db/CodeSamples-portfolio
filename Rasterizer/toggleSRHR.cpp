//in main
//std::bind is inefficient
std::function<void()> hardwareRender = [&pRenderer] { pRenderer->RenderHardware(); };
std::function<void()> softwareRender = [&pRenderer] { pRenderer->RenderSoftware(); };
GameManager::GetInstance()->Initialize(hardwareRender , softwareRender);

GameManager::GameManager()
    : m_RenderMode{RenderMode::hardware}
    , m_FrustumCullingMode{FrustumCullingMode::OneVertexMode}
    , m_Gammavalue{0.9f}
{}

/// @brief the general game update
/// @param deltaTime the elapsed time since the previous frame
void GameManager::Update(const float)
{}

/// @brief renders the game with the given render function
void GameManager::Render()
{
    m_pCurrentRenderFunction();
}

/// @brief switch to the next render state
void GameManager::SwitchRenderFunction()
{
    if(m_RenderMode == RenderMode::software)
    {
        m_pCurrentRenderFunction = m_pHardwareRenderFunction;
        m_RenderMode = RenderMode::hardware;
        std::cout << "RenderMode Changed to hardware\n";
    }
    else
    {
        m_pCurrentRenderFunction = m_pSoftwareRenderFunction;
        m_RenderMode = RenderMode::software;
        std::cout << "RenderMode Changed to software\n";
    }
    CameraManager::GetInstance()->GetCamera()->SwitchRenderMode();

}

/// @brief initialize the function pointers to the different render functions
/// @param hardwareRender function pointer to the hardware renderer
/// @param softwareRender function pointer to the software renderer
void GameManager::Initialize(const std::function<void()>& hardwareRender , const std::function<void()>& softwareRender)
{
    m_pHardwareRenderFunction = hardwareRender;
    m_pSoftwareRenderFunction = softwareRender;
    m_pCurrentRenderFunction = hardwareRender;
}