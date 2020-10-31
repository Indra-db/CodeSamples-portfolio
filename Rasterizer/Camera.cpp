Camera::Camera(const Elite::FPoint3& position , float fov , float aspectRatio , float m_NearPlane , float m_FarPlane)
    : m_Position{position}
    , m_WorldUpVector{0.f , 1.f , 0.f}
    , m_RightVector{0.0f , 0.0f , 0.0f}
    , m_ForwardVector{0.f , 0.f , 1.f}
    , m_RotSpeed{0.5f}
    , m_Speed{100.0f}
    , m_FOV{tanf(fov / 2)}
    , m_AspectRatio{aspectRatio}
    , m_NearPlane{m_NearPlane}
    , m_FarPlane{m_FarPlane}
    , m_UpdateMovement{false}
{
    m_RightVector = GetNormalized(Cross(m_WorldUpVector , m_ForwardVector.xyz));

    int rendermode = static_cast<int>(GameManager::GetInstance()->GetRenderMode());

    m_UpVector = GetNormalized(Cross({rendermode * m_ForwardVector.x, rendermode * m_ForwardVector.y,m_ForwardVector.z} , Elite::FVector3{m_RightVector.x,  rendermode * m_RightVector.y, rendermode * m_RightVector.z}));

    CalculateProjectionMatrix();

    m_ONBMatrix = Elite::FMatrix4
    {
          { m_RightVector.x,rendermode * m_RightVector.y,rendermode * m_RightVector.z, m_RightVector.w},
          m_UpVector,
          { rendermode * m_ForwardVector.x,rendermode * m_ForwardVector.y,m_ForwardVector.z, m_ForwardVector.w},
          Elite::FVector4(Elite::FVector3(m_Position.x,m_Position.y,rendermode * m_Position.z),1.0f)
    };

    CalculateViewMatrix();
}

/// @brief update the rotation and movement according to the input + do the necessary calculations
/// @param deltaTime elapsed seconds since last frame
void Camera::Update(const float deltaTime)
{
    MouseMovement(deltaTime);
    CameraTranslation(deltaTime);

    if(m_UpdateMovement)
    {
        m_UpdateMovement = false;
        ONBMovementUpdate();
    }
}

/// @brief When this function gets called, the matrices get modified to fit the light/right handed coordinate system
void Camera::SwitchRenderMode()
{
    m_ProjectionMatrix(2 , 2) = -m_ProjectionMatrix(2 , 2);
    m_ProjectionMatrix(3 , 2) = -m_ProjectionMatrix(3 , 2);

    //get the render mode -- hardware values negative 1 ---- software values positive 1
    int rendermode = static_cast<int>(GameManager::GetInstance()->GetRenderMode());

    m_ONBMatrix(2 , 3) = -m_ONBMatrix(2 , 3); //pos z

    m_UpVector = GetNormalized(Cross({rendermode * m_ForwardVector.x,rendermode * m_ForwardVector.y,m_ForwardVector.z}
    , Elite::FVector3{m_RightVector.x, rendermode * m_RightVector.y, rendermode * m_RightVector.z}));
    m_ONBMatrix[0] = {m_RightVector.x , rendermode * m_RightVector.y , rendermode * m_RightVector.z , m_RightVector.w};
    m_ONBMatrix[1] = m_UpVector;
    m_ONBMatrix[2] = {rendermode * m_ForwardVector.x,rendermode * m_ForwardVector.y,m_ForwardVector.z, m_ForwardVector.w};

    CalculateViewMatrix();
}

/// @brief Called at initialize to init the pojection matrix and 
/// @brief gets adapted according to the start render mode (software or hardware rasterizer)
void Camera::CalculateProjectionMatrix()
{
    m_ProjectionMatrix =
    {
          Elite::FVector4(1 / (m_AspectRatio * m_FOV), 0, 0, 0),
          Elite::FVector4(0, 1 / m_FOV, 0, 0),
          Elite::FVector4(0, 0, (m_FarPlane / (m_FarPlane - m_NearPlane)), 1.0f),
          Elite::FVector4(0, 0, ((-(m_NearPlane * m_FarPlane) / (m_FarPlane - m_NearPlane))), 0)
    };

    if(GameManager::GetInstance()->GetRenderMode() == RenderMode::software)
    {
        m_ProjectionMatrix(2 , 2) = -m_ProjectionMatrix(2 , 2);
        m_ProjectionMatrix(3 , 2) = -m_ProjectionMatrix(3 , 2);
    }
}

/// @brief calculate and store the inverse of the ONB matrix -> view matrix
void Camera::CalculateViewMatrix()
{
    m_ViewMatrix = Elite::Inverse(m_ONBMatrix);
}

/// @brief move, change the position according to the key press input of the user
/// @param deltaTime elapsed time between frames
void Camera::CameraTranslation(const float deltaTime)
{
    const Uint8* state = SDL_GetKeyboardState(NULL);

    //forward backward translation
    if(state[SDL_SCANCODE_UP] || state[SDL_SCANCODE_KP_8])
    {
        m_Position -= m_ForwardVector.xyz * m_Speed * deltaTime;
        m_UpdateMovement = true;
    }
    else if(state[SDL_SCANCODE_DOWN] || state[SDL_SCANCODE_KP_2])
    {
        m_Position += m_ForwardVector.xyz * m_Speed * deltaTime;
        m_UpdateMovement = true;
    }

    //left right translation
    if(state[SDL_SCANCODE_LEFT] || state[SDL_SCANCODE_KP_4])
    {
        m_Position -= m_RightVector.xyz * m_Speed * deltaTime;
        m_UpdateMovement = true;
    }
    else if(state[SDL_SCANCODE_RIGHT] || state[SDL_SCANCODE_KP_6])
    {
        m_Position += m_RightVector.xyz * m_Speed * deltaTime;
        m_UpdateMovement = true;
    }

    if(state[SDL_SCANCODE_KP_7])
    {
        m_Position.y -= m_Speed * deltaTime;
        m_UpdateMovement = true;
    }
    else if(state[SDL_SCANCODE_KP_9])
    {
        m_Position.y += m_Speed * deltaTime;
        m_UpdateMovement = true;
    }
}

/// @brief update the required ONB values when the position of the camera changes
void Camera::ONBMovementUpdate()
{
      //get the render mode -- hardware values negative 1 ---- software values positive 1
    int rendermode = static_cast<int>(GameManager::GetInstance()->GetRenderMode());
    m_ONBMatrix[3] = {m_Position.x,m_Position.y, rendermode * m_Position.z,1.0f};
    CalculateViewMatrix();
}

/// @brief Updating of the right and up vector and the ONB matrix when rotation occures
void Camera::RotationUpdate()
{
      //get the render mode -- hardware values negative 1 ---- software values positive 1
    int rendermode = static_cast<int>(GameManager::GetInstance()->GetRenderMode());

    m_RightVector = GetNormalized(Cross(m_WorldUpVector , m_ForwardVector.xyz));
    m_UpVector = GetNormalized(Cross({rendermode * m_ForwardVector.x,rendermode * m_ForwardVector.y,m_ForwardVector.z}
    , Elite::FVector3{m_RightVector.x, rendermode * m_RightVector.y, rendermode * m_RightVector.z}));

    m_ONBMatrix[0] = {m_RightVector.x , rendermode * m_RightVector.y , rendermode * m_RightVector.z , m_RightVector.w};
    m_ONBMatrix[1] = m_UpVector;
    m_ONBMatrix[2] = {rendermode * m_ForwardVector.x,rendermode * m_ForwardVector.y,m_ForwardVector.z, m_ForwardVector.w};
    CalculateViewMatrix();
}