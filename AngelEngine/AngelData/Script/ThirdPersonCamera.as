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
		const Actor @player = playerRef;
		if( player is null )
		{
			@player = actorFactory.GetActorByName("Player");
			
			// Keep a weak ref to the player so we don't keep the object alive unnecessarily
			@playerRef = player;
		}

		if(inputManager.IsRightMouseDown())
		{
			if(m_lastMousePosition.x!=inputManager.GetMousePosition().x
			|| m_lastMousePosition.y!=inputManager.GetMousePosition().y)
			{
				self.transform.rotation.y -=  (m_lastMousePosition.x-inputManager.GetMousePosition().x)*m_cameraRotationSpeed;
				float tmpX=self.transform.rotation.x;
				tmpX-=  (m_lastMousePosition.y-inputManager.GetMousePosition().y)*m_cameraRotationSpeed;
				if(tmpX < 0.2f && tmpX > -0.5f)
					self.transform.rotation.x =  tmpX;

				DebugString(tmpX);
			}
		}


		Vector3 camTarget=player.transform.position;
		mainCamera.SetCameraDirection(self.transform.rotation);

		Matrix rotate=GetRotationMatrix(
		self.transform.rotation.x,
		self.transform.rotation.y,
		0);

		Vector3 camPos=TransformCoord(
		mainCamera.GetDefaultForward(),rotate);
		camPos=camPos.Normalize();

		//distance to character.
		camPos.x*=-10;
		camPos.y*=25;
		camPos.z*=-10;
		camTarget.y+=5;

		camPos=camPos+camTarget;

		Vector3 camForward;
		camForward.x=camPos.x-camTarget.x;
		camForward.y=0;
		camForward.z=camPos.z-camTarget.z;

		camForward=camForward.Normalize();

		Vector3 camRight;
		camRight.x=-camForward.z;
		camRight.y=0;
		camRight.z=camForward.x;

		Vector3 camUp=Cross(camForward,camRight);
		camUp=camUp.Normalize();
		camUp.x=-camUp.x;
		camUp.y=-camUp.y;
		camUp.z=-camUp.z;

		mainCamera.SetCameraRight(camRight);
		mainCamera.SetCameraUp(camUp);
		mainCamera.SetCameraTarget(camTarget);
		mainCamera.SetCameraPosition(camPos);

		mainCamera.UpdateViewMatrix(mainCamera.GetCameraPosition()
		,mainCamera.GetCameraTarget()
		,mainCamera.GetCameraUp());
		
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
	//refrence to our player.
	const_weakref<Actor> playerRef;
}
