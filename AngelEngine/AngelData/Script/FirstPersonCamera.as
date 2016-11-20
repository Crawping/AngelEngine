// This script implements the logic for the player character we use character controller for this player..
class camera : IController
{
	camera(Actor @obj)
	{
		// Keep the owner for later reference
		@self = obj;
	}

	void Update()
	{
		//simulate running.
		if(inputManager.isKeyDown("LShift"))
		{
			self.characterController.SetMoveSpeed(20*timeManager.DeltaTime());
		}
		if(inputManager.isKeyUp("LShift"))
		{
			self.characterController.SetMoveSpeed(5*timeManager.DeltaTime());
		}
		if(inputManager.isKeyDown("A"))
		{
			self.characterController.MoveLeft();
		}
		if(inputManager.isKeyDown("d"))
		{
			self.characterController.MoveRight();
		}
		if(inputManager.isKeyDown("W"))
		{
			self.characterController.MoveForward();
		}
		if(inputManager.isKeyDown("S"))
		{
			self.characterController.MoveBackward();
		}

		if(inputManager.IsRightMouseDown())
		{
			if(m_lastMousePosition.x!=inputManager.GetMousePosition().x
			|| m_lastMousePosition.y!=inputManager.GetMousePosition().y)
			{
				self.transform.rotation.y -=  (m_lastMousePosition.x-inputManager.GetMousePosition().x)*m_cameraRotationSpeed;
				self.transform.rotation.x -=  (m_lastMousePosition.y-inputManager.GetMousePosition().y)*m_cameraRotationSpeed;
			}
		}

		mainCamera.SetCameraDirection(self.transform.rotation);
		
		Matrix rotate=GetRotationMatrix(
		self.transform.rotation.x,
		self.transform.rotation.y,
		0);
		
		Vector3 transformedTarget=TransformCoord(
		mainCamera.GetDefaultForward(),rotate);

		rotate=GetRotationMatrix(
		0,
		self.transform.rotation.y,
		0);
		
		Vector3 transformedForward=TransformCoord(
		mainCamera.GetDefaultForward(),rotate);

		Vector3 transformedRight=TransformCoord(
		mainCamera.GetDefaultRight(),rotate);

		transformedTarget=transformedTarget.Normalize();
		mainCamera.SetCameraTarget(transformedTarget);
		mainCamera.SetCameraRight(transformedRight);
		mainCamera.SetCameraForward(transformedForward);
		Vector3 transformedUp=Cross(transformedForward
		,transformedRight);

		mainCamera.SetCameraUp(transformedUp);

		mainCamera.SetCameraPosition(self.transform.position);

		Vector3 FinalTarget=mainCamera.GetCameraTarget()+mainCamera.GetCameraPosition();
		mainCamera.SetCameraTarget(FinalTarget);

		mainCamera.UpdateViewMatrix(mainCamera.GetCameraPosition()
		,mainCamera.GetCameraTarget()
		,mainCamera.GetCameraUp());
		
		m_Lposition=self.transform.position;

		m_lastMousePosition=inputManager.GetMousePosition();
	}

	void Start()
	{
		m_cameraMovmentSpeed=0.5f;
		m_cameraRotationSpeed=0.01f;
		mainCamera.SetCameraPosition(self.transform.position);
		m_Lposition=self.transform.position;
	}

	void OnCollide(Actor &sender)
	{
		
	}

	void OnMessage(string msg, const Actor &sender)
	{
		
	}

	Actor @self;
	Vector3 m_Position;
	Vector3 m_Rotation;
	Vector3 m_Lposition;
	Vector2 m_lastMousePosition;
	float	m_cameraMovmentSpeed;
	float	m_cameraRotationSpeed;
}
