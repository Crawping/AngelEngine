// This script implements the logic for the player character
class GoalLine : IController
{
	// The constructor must take a CGameObj handle as input,
	// this is the object that the script will control
	GoalLine(Actor @obj)
	{
		// Keep the owner for later reference
		@self = obj;
	}

	void Update()
	{
		
		
	}

	void Start()
	{
		self.SetHidden(true);
		self.rigidBody.SetCollisionFlag(4);
	}

	void OnCollide(const Actor &sender)
	{
		if(sender.GetTag()=="GoalBall")
		{
			self.Send("Goaled",sender);
		}
			
	}

	void OnMessage(string msg, const Actor &sender)
	{
	
	}


	Actor @self;
}
