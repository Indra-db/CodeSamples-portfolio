Entity* CreatePlayerEntity(const EntityManager* const pEntityManager , const std::string& entityName , int posX , int posY)
{
	// - Texture -
	const GPU_Image* const pPlayerTexture = ResourceManager::GetInstance().GetTexture("allSprites");

	// - Transform -
	const TransformComponent transformC{glm::vec3 { posX,posY,0 } , (float) pPlayerTexture->texture_w , (float) pPlayerTexture->texture_h};

	// ---- Entity ----
	Entity* const pPlayerEntity = pEntityManager->CreateEntity(entityName , LayerType::PLAYER_LAYER , transformC);

		// - input - 
	InputComponent* const pInputCompPlayer = pPlayerEntity->AddComponent<InputComponent>();

	float moveSpeed = 32.0f;
	pInputCompPlayer->AddInputs("Move_Up" , new MoveCommand(0.f , -moveSpeed));

// Continued...

// - Physics -
	pPlayerEntity->AddComponent<PhysicsComponent>(glm::vec3{5.f, 5.f, 0.f} , glm::vec3{0.f, 0.f, 0.f});

	// - State -
	StateComponent* const pStateComp = pPlayerEntity->AddComponent<StateComponent>("Player" , new PlayerState());
	pStateComp->AddState("VertMov" , new PlayerVerMoveState());

	// Continued...

	// - Animation -
	AnimatedSpriteComponent* const pAnimationComp = pPlayerEntity->AddComponent<AnimatedSpriteComponent>("allSprites" , false , true);
	pAnimationComp->AddAnimation(new Animation(AnimationType::WALKHOR , "allSprites" , 0 , 3 , 1 , 3 , 5 , 32 , 32 , 0 , 0)); //animation alignment, frames, positions, size, ect

	// Continued...

	pPlayerEntity->AddComponent<RectCollisionComponent>(CollsionMask::PLAYER , true , SDL_Color{255,0,255} , 0.9f , 0.9f);

	// - Subject -

	SubjectComponent<EventArg>* const pSubjComp = pPlayerEntity->AddComponent<SubjectComponent<EventArg>>();


	// - collision check -

	CollisionCommand<EventArg>* const pEmeraldColCom = new CollisionCommand<EventArg>({EventType::COLEMERALD,pPlayerEntity} , pSubjComp);

	// Continued...

	CollisionCheckComponent* const pColCheckComp = pPlayerEntity->AddComponent<CollisionCheckComponent>();

	pColCheckComp->AddCollisionAction(CollisionType::PLAYER_EMERALD_COLLISION , emeraldColCom);

	// Continued...

	pPlayerEntity->AddComponent<AudioEffectComponent>("DeadPlayerAudio");

	// Continued...

	int lives{3};
	int posXWindow{114};
	int posYWindow{0};
	pPlayerEntity->AddComponent<LivesComponent>(lives , posXWindow , posYWindow , "Heart");

	return pPlayerEntity;
}