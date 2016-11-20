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
		if(inputManager.isKeyDown("left") && m_gameStarted)
		{
			self.rigidBody.ApplyForce(0,0,-2500);
			self.rigidBody.ApplyTorque(0,0,-250);
		}
		if(inputManager.isKeyDown("right") && m_gameStarted)
		{
			self.rigidBody.ApplyForce(0,0,2500);
			self.rigidBody.ApplyTorque(0,0,250);
		}
		if(inputManager.isKeyDown("Down") && m_gameStarted)
		{
			self.rigidBody.ApplyForce(2500,0,0);
			self.rigidBody.ApplyTorque(250,0,0);
		}
		if(inputManager.isKeyDown("Up") && m_gameStarted)
		{
			self.rigidBody.ApplyForce(-2500,0,0);
			self.rigidBody.ApplyTorque(-250,0,0);
		}

		if(inputManager.isKeyPressed("LShift") && m_gameStarted)
		{
			self.rigidBody.ApplyTorque(-250,0,0);
			self.rigidBody.ApplyImpluse(-1900,0,0);
			self.audioHandler.SetVolume(2.1,1);
			self.audioHandler.Play(false,1);
		}

		if(self.transform.position.y<-20.0f)
		{
			self.rigidBody.SetPosition(m_startingPosition);
			self.rigidBody.SetLinearVelocity(0,0,0);
		}

		//DebugString(m_score);

		//SetCallback(MyCallback);

	}

	void RestartGame()
	{
			self.rigidBody.SetPosition(m_startingPosition);
			self.rigidBody.SetLinearVelocity(0,0,0);
			Actor @a=actorFactory.GetActorByName("GoalBall1");
			a.rigidBody.SetPosition(5,0,0);
			a.rigidBody.SetLinearVelocity(0,0,0);
			a.rigidBody.SetAngularVelocity(0,0,0);
			@a=actorFactory.GetActorByName("GoalBall2");
			a.rigidBody.SetPosition(5,0,5);
			a.rigidBody.SetLinearVelocity(0,0,0);
			a.rigidBody.SetAngularVelocity(0,0,0);
			@a=actorFactory.GetActorByName("GoalBall3");
			a.rigidBody.SetPosition(5,0,13);
			a.rigidBody.SetLinearVelocity(0,0,0);
			a.rigidBody.SetAngularVelocity(0,0,0);
			@a=actorFactory.GetActorByName("GoalBall4");
			a.rigidBody.SetPosition(5,0,-9);
			a.rigidBody.SetLinearVelocity(0,0,0);
			a.rigidBody.SetAngularVelocity(0,0,0);
			@a=actorFactory.GetActorByName("GoalBall5");
			a.rigidBody.SetPosition(5,0,-5);
			a.rigidBody.SetLinearVelocity(0,0,0);
			a.rigidBody.SetAngularVelocity(0,0,0);
			@a=actorFactory.GetActorByName("GoalBall6");
			a.rigidBody.SetPosition(5,0,-13);
			a.rigidBody.SetLinearVelocity(0,0,0);
			a.rigidBody.SetAngularVelocity(0,0,0);
			@a=actorFactory.GetActorByName("GoalBall7");
			a.rigidBody.SetPosition(5,0,-17);
			a.rigidBody.SetLinearVelocity(0,0,0);
			a.rigidBody.SetAngularVelocity(0,0,0);
			@a=actorFactory.GetActorByName("GoalBall8");
			a.rigidBody.SetPosition(5,0,17);
			a.rigidBody.SetLinearVelocity(0,0,0);
			a.rigidBody.SetAngularVelocity(0,0,0);
			@a=actorFactory.GetActorByName("GoalBall9");
			a.rigidBody.SetPosition(5,0,-21);
			a.rigidBody.SetLinearVelocity(0,0,0);
			a.rigidBody.SetAngularVelocity(0,0,0);
			@a=actorFactory.GetActorByName("GoalBall10");
			a.rigidBody.SetPosition(5,0,15);
			a.rigidBody.SetLinearVelocity(0,0,0);
			a.rigidBody.SetAngularVelocity(0,0,0);
			self.Send("Reset","GoalBall1");
			self.Send("Reset","GoalBall2");
			self.Send("Reset","GoalBall3");
			self.Send("Reset","GoalBall4");
			self.Send("Reset","GoalBall5");
			self.Send("Reset","GoalBall6");
			self.Send("Reset","GoalBall7");
			self.Send("Reset","GoalBall8");
			self.Send("Reset","GoalBall9");
			self.Send("Reset","GoalBall10");
	}

	
	void Start()
	{
		m_startingPosition.x=45;
		m_startingPosition.y=0;
		m_startingPosition.z=-5;
		self.transform.position = m_startingPosition;
		m_score=0;
		self.audioHandler.Load("Music.wav");
		self.audioHandler.Load("BallBounceOther.wav");
		self.audioHandler.Load("BallBounceOther.wav");
		self.audioHandler.SetVolume(1,0);
		self.audioHandler.Play(true,0);
		self.audioHandler.SetVolume(0.9,2);
		self.meshRenderer.SetAmbientColor(0.8,0.8,0.8,0.8);
		self.meshRenderer.SetDiffuseColor(0.8,0.8,0.8,0.8);
		m_gameStarted=false;
		
	}

	void OnCollide(const Actor &sender)
	{
		if(sender.GetTag()=="GoalBall" || sender.GetTag()=="GoaledBall")
		{
			self.audioHandler.Play(false,2);
		}
	}

	void OnMessage(string msg, const Actor &sender)
	{
		if(msg=="Score")
		{
			m_score++;
		}	
		if(msg=="StartGame")
		{
			m_gameStarted=true;
			RestartGame();
		}
			
	}


	Actor @self;
	//for not letting player getting out of sight.
	Vector4 m_bounds;
	Vector3 m_startingPosition;
	int m_score;
	bool m_gameStarted;
}
