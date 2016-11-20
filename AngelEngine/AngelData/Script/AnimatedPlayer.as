// This script implements the logic for the player character

class Player : IController
{
	// The constructor must take a CGameObj handle as input,
	// this is the object that the script will control
	Player(Actor @obj)
	{
		// Keep the owner for later reference
		@self = obj;
	}

	void Update()
	{
		//reset speed every frame.
		m_speed=4.0f;
		Matrix rotate=GetRotationMatrix(
		0,
		self.transform.rotation.y,
		0);

		Vector3 forward=TransformCoord(
		mainCamera.GetDefaultForward(),rotate);
		forward=forward.Normalize();

		Vector3 right=TransformCoord(
		mainCamera.GetDefaultRight(),rotate);
		right=right.Normalize();

		if(inputManager.isKeyDown("W"))
		{
			if(inputManager.isKeyDown("LShift"))
			{
				m_speed*=2;
				self.meshRenderer.SetAnimationClipIndex(9);
			}
			else
				self.meshRenderer.SetAnimationClipIndex(16);
			self.characterController.SetMoveSpeed(m_speed*timeManager.DeltaTime());
			self.characterController.MoveForward();
		}
		else if(inputManager.isKeyDown("S"))
		{
			if(inputManager.isKeyDown("LShift"))
			{
				m_speed*=2;
				self.meshRenderer.SetAnimationClipIndex(10);
			}
			else
				self.meshRenderer.SetAnimationClipIndex(18);
			self.characterController.SetMoveSpeed(m_speed*timeManager.DeltaTime());
			self.characterController.MoveBackward();
		}
		else if(inputManager.isKeyDown("A"))
		{
			if(inputManager.isKeyDown("LShift"))
			{
				m_speed*=2;
				self.meshRenderer.SetAnimationClipTimeScale(15,2);
				self.meshRenderer.SetAnimationClipIndex(15);
			}
			else
			{
				self.meshRenderer.SetAnimationClipTimeScale(15,1);
				self.meshRenderer.SetAnimationClipIndex(15);
			}
			
			self.characterController.SetMoveSpeed(m_speed*timeManager.DeltaTime());
			self.characterController.MoveLeft();
		}
		else if(inputManager.isKeyDown("D"))
		{
			if(inputManager.isKeyDown("LShift"))
			{
				m_speed*=2;
				self.meshRenderer.SetAnimationClipTimeScale(14,2);
				
				self.meshRenderer.SetAnimationClipIndex(14);
			}
			else
			{
				self.meshRenderer.SetAnimationClipTimeScale(14,1);
				self.meshRenderer.SetAnimationClipIndex(14);
			}
			self.characterController.SetMoveSpeed(m_speed*timeManager.DeltaTime());
			self.characterController.MoveRight();
		}
		else
		{
			self.meshRenderer.SetAnimationClipIndex(1);
		}
		self.transform.rotation.y=mainCamera.GetCameraDirection().y;
	}

	void RestartGame()
	{
			
	}

	
	void Start()
	{
		
		self.meshRenderer.SetAnimationClipIndex(1);
		self.meshRenderer.SetAnimationClipEndTime(16,1.4);
		self.meshRenderer.SetAnimationClipEndTime(18,1.4);
		self.meshRenderer.SetAnimationClipEndTime(14,1.4);
		self.meshRenderer.SetAnimationClipEndTime(15,1.4);
		//faster playing of animations.
		self.meshRenderer.SetAnimationClipTimeScale(9,2);
		self.meshRenderer.SetAnimationClipTimeScale(10,2);
		self.meshRenderer.SetAnimationClipTimeScale(14,2);
		self.meshRenderer.SetAnimationClipTimeScale(15,2);
		//for minimum distance to ground.
		self.characterController.SetLocalScale(1,0,0);
		//for adjust player height on ground.
		self.characterController.SetHeightOffset(-2.2);
	}

	void OnCollide(const Actor &sender)
	{
	}

	void OnMessage(string msg, const Actor &sender)
	{
		
	}


	Actor @self;
	float m_speed;

}
