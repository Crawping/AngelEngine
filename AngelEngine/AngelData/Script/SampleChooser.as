// This script implements the logic for the player character
funcdef void CALLBACK();
class GUI : IController
{
	// The constructor must take a CGameObj handle as input,
	// this is the object that the script will control
	GUI(Actor @obj)
	{
		// Keep the owner for later reference
		@self = obj;
	}

	void Update()
	{
		
	}

	void Start()
	{
		CALLBACK @funcHelloWorld = CALLBACK(LoadHelloWorld);

		@HelloWorld = CreateButton();
		HelloWorld.RegisterFunction(funcHelloWorld);
		HelloWorld.SetPosition(400,50);
		HelloWorld.SetText("Hello World");
		HelloWorld.SetRounded(true);
		HelloWorld.SetRoundedRadius(10,10);
		HelloWorld.SetRectangle(0,0,200,50);
		HelloWorld.SetDefaultColor(0.5,0.5,0.6,1.0f);
		HelloWorld.SetHoverColor(0.8,0.5,0.6,1.0f);
		HelloWorld.SetClickColor(0.5,0.8,0.6,1.0f);
		HelloWorld.SetTextColor(1,1,1,1);
		HelloWorld.SetPosition(200,100);

		CALLBACK @funcMediaRendering = CALLBACK(LoadMediaRendering);

		@MediaRendering = CreateButton();
		MediaRendering.RegisterFunction(funcMediaRendering);
		MediaRendering.SetPosition(400,50);
		MediaRendering.SetText("Media Rendering");
		MediaRendering.SetRounded(true);
		MediaRendering.SetRoundedRadius(10,10);
		MediaRendering.SetRectangle(0,0,200,50);
		MediaRendering.SetDefaultColor(0.5,0.5,0.6,1.0f);
		MediaRendering.SetHoverColor(0.8,0.5,0.6,1.0f);
		MediaRendering.SetClickColor(0.5,0.8,0.6,1.0f);
		MediaRendering.SetTextColor(1,1,1,1);
		MediaRendering.SetPosition(200,160);

		CALLBACK @funcGUI2D = CALLBACK(LoadGUI2D);

		@GUI2D = CreateButton();
		GUI2D.RegisterFunction(funcGUI2D);
		GUI2D.SetPosition(400,50);
		GUI2D.SetText("AngelGUI2D");
		GUI2D.SetRounded(true);
		GUI2D.SetRoundedRadius(10,10);
		GUI2D.SetRectangle(0,0,200,50);
		GUI2D.SetDefaultColor(0.5,0.5,0.6,1.0f);
		GUI2D.SetHoverColor(0.8,0.5,0.6,1.0f);
		GUI2D.SetClickColor(0.5,0.8,0.6,1.0f);
		GUI2D.SetTextColor(1,1,1,1);
		GUI2D.SetPosition(200,220);

		CALLBACK @funcAnimation = CALLBACK(LoadAnimation);
		@Animation = CreateButton();
		Animation.RegisterFunction(funcAnimation);
		Animation.SetPosition(400,50);
		Animation.SetText("Animations(Vertex Skinning)");
		Animation.SetRounded(true);
		Animation.SetRoundedRadius(10,10);
		Animation.SetRectangle(0,0,200,50);
		Animation.SetDefaultColor(0.5,0.5,0.6,1.0f);
		Animation.SetHoverColor(0.8,0.5,0.6,1.0f);
		Animation.SetClickColor(0.5,0.8,0.6,1.0f);
		Animation.SetTextColor(1,1,1,1);
		Animation.SetPosition(200,280);
	}

	void OnCollide(Actor &sender)
	{
		
	}

	void OnMessage(string msg, const Actor &sender)
	{
		
	}

	void LoadHelloWorld()
	{
		levelManager.LoadLevel("HelloWorld");
	}

	void LoadMediaRendering()
	{
		levelManager.LoadLevel("MediaRendering");
	}

	void LoadGUI2D()
	{
		levelManager.LoadLevel("AngelGUI2D");
	}

	void LoadAnimation()
	{
		levelManager.LoadLevel("Animation");
	}

	Actor @self;
	GUIButton @HelloWorld;
	GUIButton @MediaRendering;
	GUIButton @GUI2D;
	GUIButton @Animation;
}
