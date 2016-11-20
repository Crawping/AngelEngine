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
		if(m_started && !m_paused)
		{
			if(m_countDown>0.0f)
			m_countDown-=timeManager.DeltaTime();
			string text="\t        Time :  ";
			text+=ToString(m_countDown);
			text+="\n\n\t\t Score: ";
			text+=ToString(m_score);
			scoreBoard.SetText(text);
		}
		else if(!m_paused && !m_started)
		{
			string text="Press Start To Play";
			scoreBoard.SetText(text);
		}
		else if(m_started && m_paused)
		{
			string text="\t Paused";
			scoreBoard.SetText(text);
		}

		if(m_countDown<0.0f)
		{
			string text="Times Up!";
			text+="\nYour Score : ";
			text+=ToString(m_score);
			FinalScore.SetText(text);
			FinalScore.SetActive(true);
			StartGame.SetActive(true);
			End.SetActive(true);
		}
	}

	void Start()
	{
		CALLBACK @func = CALLBACK(StartGameCallBack);
		CALLBACK @funcEnd = CALLBACK(EndGameCallBack);
		CALLBACK @funcPause = CALLBACK(PauseGameCallBack);
		
		@StartGame = CreateButton();
		StartGame.RegisterFunction(func);
		StartGame.SetPosition(400,50);
		StartGame.SetText("Start");
		StartGame.SetRounded(true);
		StartGame.SetRoundedRadius(10,10);
		StartGame.SetRectangle(0,0,200,50);
		StartGame.SetDefaultColor(0.5,0.5,0.6,1.0f);
		StartGame.SetHoverColor(0.8,0.5,0.6,1.0f);
		StartGame.SetClickColor(0.5,0.8,0.6,1.0f);
		StartGame.SetTextColor(1,1,1,1);
		StartGame.SetPosition(200,100);
		
		@End = CreateButton();
		End.SetPosition(400,300);
		End.SetText("End");
		End.SetRounded(true);
		End.SetRoundedRadius(10,10);
		End.SetRectangle(0,0,200,50);
		End.SetDefaultColor(0.5,0.5,0.6,1.0f);
		End.SetHoverColor(0.8,0.5,0.6,1.0f);
		End.SetClickColor(0.5,0.8,0.6,1.0f);
		End.SetTextColor(1,1,1,1);
		End.SetPosition(WindowHeight()/2,200);
		End.RegisterFunction(funcEnd);
		
		@Title=CreateText();
		Title.SetText("Mini Soccer");
		Title.SetRectangle(0,0,200,200);
		Title.SetEditable(false);
		Title.SetTextColor(0.5,1,0.5,1);
		Title.SetAlignmentDirection(1);
		Title.SetFittingMode(1);
		Title.SetPosition(WindowHeight()/2-100,50);
		
		@scoreBoard = Create3DText();
		scoreBoard.SetText("Score : ");
		scoreBoard.SetBackgroundColor(0.3,0.3,0.3,0.3);
		scoreBoard.SetTextColor(0,1,0,1);
		scoreBoard.SetPosition(-80,20,-10);
		scoreBoard.SetRotation(0,ToRadian(-90),0);
		scoreBoard.SetScale(10,10,1);
		scoreBoard.SetFontSize(45);
		scoreBoard.SetEditable(false);
		
		@Angel = Create3DText();
		Angel.SetText("Powered By AngelEngine");
		Angel.SetBackgroundColor(0.3,0.3,0.3,0.0);
		Angel.SetTextColor(0.8,1,0.3,1);
		Angel.SetPosition(-80,40,-5);
		Angel.SetRotation(0,ToRadian(-90),0);
		Angel.SetScale(10,5,1);
		Angel.SetFontSize(85);
		Angel.SetEditable(false);
		Angel.SetBillboard(true);
		
		
		
		@FinalScore = CreateText();
		FinalScore.SetRectangle(0,0,200,200);
		FinalScore.SetEditable(false);
		FinalScore.SetTextColor(1,1,0,1);
		FinalScore.SetAlignmentDirection(0);
		FinalScore.SetFittingMode(1);
		FinalScore.SetPosition(WindowHeight()/2,50);
		FinalScore.SetActive(false);
		
		@Tutorial=CreateText();
		Tutorial.SetText("Press Arrow Keys To Move The Ball \n Press WASD To Adjust Camera Position Behind Ball \nUse LShift To Shoot \nScore As Many As You Can");
		Tutorial.SetRectangle(0,0,200,200);
		Tutorial.SetEditable(false);
		Tutorial.SetTextColor(0.8,1,0.8,1);
		Tutorial.SetAlignmentDirection(1);
		Tutorial.SetFittingMode(1);
		Tutorial.SetPosition(WindowHeight()/2+20,300);
		
		@PauseButton = CreateButton();
		PauseButton.SetPosition(0,0);
		PauseButton.SetBitmapPath("PauseIcon.gif");
		PauseButton.SetRoundedRadius(10,10);
		PauseButton.SetRectangle(0,0,50,50);
		PauseButton.SetDefaultColor(0.0,0.0,0.0,0.0f);
		PauseButton.SetHoverColor(0.1,0.1,0.1,0.1f);
		PauseButton.SetClickColor(0.1,0.1,0.1,0.0f);
		PauseButton.RegisterFunction(funcPause);
		
		m_started=false;
		m_paused=false;
	}

	void StartGameCallBack()
	{
		StartGame.SetActive(false);
		End.SetActive(false);
		Title.SetActive(false);
		self.Send("StartGame","MainBall");
		m_started=true;
		m_countDown = 60.0f;
		FinalScore.SetActive(false);
		Tutorial.SetActive(false);
		m_score=0;
	}

	void EndGameCallBack()
	{
		Quit();
	}

	void PauseGameCallBack()
	{
		if(m_started)
		{
			m_paused=!m_paused;
			timeManager.SetPause(m_paused);
		}
	}

	void OnCollide(Actor &sender)
	{
		
	}

	void OnMessage(string msg, const Actor &sender)
	{
		if(msg=="Score" && (m_countDown>0.0f))
			m_score++;
	}

	Actor @self;
	GUIButton @StartGame;
	GUIButton @End;
	GUIText @Title;
	GUIText @Tutorial;
	GUIText @FinalScore;
	GUI3DText @scoreBoard;
	GUI3DText @Angel;
	GUIButton @PauseButton;
	int m_score;
	bool m_started;
	float m_countDown;
	bool m_paused;
}
