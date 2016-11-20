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

		const Actor @player = playerRef;
		if( player is null )
		{
			@player = actorFactory.GetActorByName("MainBall");
			
			// Keep a weak ref to the player so we don't keep the object alive unnecessarily
			@playerRef = player;
		}

		Vector3 position;
		position.x=player.transform.position.x+m_xOffset;
		position.y=player.transform.position.y+m_yOffset;
		position.z=player.transform.position.z;

		mainCamera.SetCameraPosition(position);

		Vector3 FinalTarget=mainCamera.GetCameraTarget()+mainCamera.GetCameraPosition();
		mainCamera.SetCameraTarget(FinalTarget);

		mainCamera.UpdateViewMatrix(mainCamera.GetCameraPosition()
		,mainCamera.GetCameraTarget()
		,mainCamera.GetCameraUp());
		
		m_Lposition=self.transform.position;

		m_lastMousePosition=inputManager.GetMousePosition();

		//Adjust position
		if(inputManager.isKeyDown("W"))
		{
			if(m_yOffset<m_yMaxOffset)
			{
				m_yOffset+=0.05f;
			}
		}

		if(inputManager.isKeyDown("S"))
		{
			if(m_yOffset>m_yMinOffset)
			{
				m_yOffset-=0.05f;
			}
		}

		if(inputManager.isKeyDown("D"))
		{
			if(m_xOffset<m_xMaxOffset)
			{
				m_xOffset+=0.05f;
			}
		}

		if(inputManager.isKeyDown("A"))
		{
			if(m_xOffset>m_xMinOffset)
			{
				m_xOffset-=0.05f;
			}
		}
	}

	void Start()
	{
		m_cameraMovmentSpeed=0.5f;
		m_cameraRotationSpeed=0.01f;
		m_yMinOffset=5;
		m_yMaxOffset=10;
		m_yOffset=5;
		m_xMinOffset=20;
		m_xMaxOffset=30;
		m_xOffset=20;

		self.transform.rotation.y=ToRadian(-90);
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
	float   m_yOffset;
	float	m_yMaxOffset;
	float	m_yMinOffset;
	float	m_xOffset;
	float	m_xMaxOffset;
	float	m_xMinOffset;
	const_weakref<Actor> playerRef;
}
