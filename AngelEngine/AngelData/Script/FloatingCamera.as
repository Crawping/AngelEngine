// This script implements the logic for the player character
class camera : IController
{
	// The constructor must take a CGameObj handle as input,
	// this is the object that the script will control
	camera(Actor @obj)
	{
		// Keep the owner for later reference
		@self = obj;
	}

	void Update()
	{
		if(inputManager.isKeyDown("D"))
		{
			self.transform.position.x+=m_cameraMovmentSpeed;
		}
		if(inputManager.isKeyDown("A"))
		{
			self.transform.position.x-=m_cameraMovmentSpeed;
		}
		if(inputManager.isKeyDown("W"))
		{
			self.transform.position.z+=m_cameraMovmentSpeed;
		}
		if(inputManager.isKeyDown("S"))
		{
			self.transform.position.z-=m_cameraMovmentSpeed;
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
		

		Matrix rotate=GetRotationMatrix(
		self.transform.rotation.x,
		self.transform.rotation.y,
		0);
		
		Vector3 transformedTarget=TransformCoord(
		mainCamera.GetDefaultForward(),rotate);
		
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

		Vector3 tmpPosition=mainCamera.GetCameraPosition();

		if(m_Lposition.x!=self.transform.position.x)
		{
			Vector3 newPosition=transformedRight.Multiply
			((self.transform.position.x-m_Lposition.x));
			tmpPosition = mainCamera.GetCameraPosition()+newPosition;
		}

		mainCamera.SetCameraPosition(tmpPosition);

		if(m_Lposition.z!=self.transform.position.z)
		{
			Vector3 newPosition=transformedForward.Multiply
			((self.transform.position.z-m_Lposition.z));
			tmpPosition = mainCamera.GetCameraPosition()+newPosition;
		}

		mainCamera.SetCameraPosition(tmpPosition);

		if(m_Lposition.y!=self.transform.position.y)
		{
			Vector3 newPosition=transformedUp.Multiply
			((self.transform.position.y-m_Lposition.y));
			tmpPosition=mainCamera.GetCameraPosition()+newPosition;
		}

		mainCamera.SetCameraPosition(tmpPosition);

		Vector3 FinalTarget=mainCamera.GetCameraTarget()+mainCamera.GetCameraPosition();
		mainCamera.SetCameraTarget(FinalTarget);

		mainCamera.UpdateViewMatrix(mainCamera.GetCameraPosition()
		,mainCamera.GetCameraTarget()
		,mainCamera.GetCameraUp());
		
		m_Lposition=self.transform.position;

		m_lastMousePosition=inputManager.GetMousePosition();
		//self.Send("Attack","MainBall");
	}

	void Start()
	{
		m_cameraMovmentSpeed=0.5f;
		m_cameraRotationSpeed=0.01f;
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
