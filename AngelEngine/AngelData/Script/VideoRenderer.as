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
		scoreBoard.SetText("Sample Name : MediaRendering\nDescription : This Sample Uses Component 'Video Renderer' To Renderer Any Type Of Video Files Using IMFMediaEngine Both 2D And 3D.");
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

		self.videoRenderer.Load("tottenham_1_1_liverpool.mp4");
		self.videoRenderer.SetRectangle(0,200,200,400);
		self.videoRenderer.Set3DRendering(true);
		self.videoRenderer.SetBillboard(true);
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