// This script implements the logic for the player character
funcdef void CALLBACK();
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
		
	}

	
	
	void Start()
	{
		CALLBACK @funcPause = CALLBACK(PauseGameCallBack);

		@scoreBoard = Create3DText();
		scoreBoard.SetText("Sample Name : HelloWorld\nDescription: First Sample Just Showing Basic Featrues Like:\n3D Text Rendering\nBillBoarding\nShadow Mapping\nCharacter Controller Using Bullet Physics And Avoid Passing Throw Walls And Climbing Stairs.\nSelect Back Button To Return To Samples Menu.");
		scoreBoard.SetBackgroundColor(0.3,0.3,0.3,0.0);
		scoreBoard.SetTextColor(0,1,0,1);
		scoreBoard.SetPosition(0,10,40);
		scoreBoard.SetRotation(0,ToRadian(-90),0);
		scoreBoard.SetScale(10,10,1);
		scoreBoard.SetFontSize(45);
		scoreBoard.SetEditable(false);
		scoreBoard.SetBillboard(true);

		@PauseButton = CreateButton();
		PauseButton.SetPosition(0,0);
		PauseButton.SetBitmapPath("Back.png");
		PauseButton.SetRoundedRadius(10,10);
		PauseButton.SetRectangle(0,0,50,50);
		PauseButton.SetDefaultColor(0.0,0.0,0.0,0.0f);
		PauseButton.SetHoverColor(0.1,0.1,0.1,0.1f);
		PauseButton.SetClickColor(0.1,0.1,0.1,0.0f);
		PauseButton.RegisterFunction(funcPause);
	}

	void PauseGameCallBack()
	{
		levelManager.LoadLevel("SampleMain");
	}

	void OnCollide(const Actor &sender)
	{
		
	}

	void OnMessage(string msg, const Actor &sender)
	{
		
			
	}

	Actor @self;
	GUI3DText @scoreBoard;
	GUIButton @PauseButton;
	
}