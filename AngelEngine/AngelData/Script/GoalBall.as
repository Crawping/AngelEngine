// This script implements the logic for the player character
class GoalBall : IController
{
	// The constructor must take a CGameObj handle as input,
	// this is the object that the script will control
	GoalBall(Actor @obj)
	{
		// Keep the owner for later reference
		@self = obj;
	}

	void Update()
	{
		if(m_goaled)
		{
			m_timer+=timeManager.DeltaTime();
		}
		if(m_timer>2.0f)
			self.SetActive(false);
		
	}

	void Start()
	{
		m_goaled=false;
		self.meshRenderer.SetAmbientColor(0.8,0.8,0.8,0.8);
		self.meshRenderer.SetDiffuseColor(0.8,0.8,0.8,0.8);
		m_timer=0.0f;
	}

	void OnCollide(const Actor &sender)
	{
		
	}

	void OnMessage(string msg, const Actor &sender)
	{
		if(msg=="Goaled" && !m_goaled)
		{
			m_goaled=true;
			self.Send("Score","GUIManager");
		}
		if(msg=="Reset")
		{
			m_goaled=false;
			self.SetActive(true);
			m_timer=0.0f;
		}
			
	}


	Actor @self;
	bool m_goaled;
	float m_timer;
}
