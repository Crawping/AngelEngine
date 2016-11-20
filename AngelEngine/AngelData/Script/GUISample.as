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
		if(textAlign1.GetChecked())
		{
			scrollBar.SetAlignmentDirection(0);
		}
		else
		{
			scrollBar.SetAlignmentDirection(1);
		}
		
		if(textAlign2.GetChecked())
		{
			scaleDown.SetAlignmentDirection(0);
		}
		else
		{
			scaleDown.SetAlignmentDirection(1);
		}
	}

	void Start()
	{
		CALLBACK @func = CALLBACK(AddNewTab);
		CALLBACK @funcPause = CALLBACK(PauseGameCallBack);
		
		@AddTab = CreateButton();
		AddTab.RegisterFunction(func);
		AddTab.SetPosition(400,50);
		AddTab.SetText("Add New Tab");
		AddTab.SetRounded(true);
		AddTab.SetRoundedRadius(10,10);
		AddTab.SetRectangle(0,0,100,50);
		AddTab.SetDefaultColor(0.5,0.5,0.6,1.0f);
		AddTab.SetHoverColor(0.8,0.5,0.6,1.0f);
		AddTab.SetClickColor(0.5,0.8,0.6,1.0f);
		AddTab.SetTextColor(1,1,1,1);
		AddTab.SetPosition(0,400);
		
		@scrollBar=CreateText();
		scrollBar.SetText("Editable text with Scrollbar");
		scrollBar.SetRectangle(0,0,200,200);
		scrollBar.SetEditable(true);
		scrollBar.SetTextColor(0.7,0,5.7,1);
		scrollBar.SetAlignmentDirection(1);
		scrollBar.SetFittingMode(1);
		scrollBar.SetPosition(10,50);
		scrollBar.SetBackgroundColor(0,0.5,0,1);
		
		@scaleDown=CreateText();
		scaleDown.SetText("Editable text with Auto ScaleDown To Fit Text");
		scaleDown.SetRectangle(0,0,200,200);
		scaleDown.SetEditable(true);
		scaleDown.SetTextColor(0.7,0,0.7,1);
		scaleDown.SetAlignmentDirection(1);
		scaleDown.SetFittingMode(0);
		scaleDown.SetPosition(230,50);
		scaleDown.SetBackgroundColor(0,0.5,0,1);
		
		@Hint = CreateText();
		Hint.SetRectangle(0,0,200,200);
		Hint.SetEditable(false);
		Hint.SetText("This Sample Shows AngelGUI2D Features,AngelGUI Writen Using Direct2D. It Has Some Basic GUI Items Like Button,TexBox,CheckBox,Tab And TabGroups. All Features Are Shown In This Sample.");
		Hint.SetTextColor(1,1,0,1);
		Hint.SetAlignmentDirection(0);
		Hint.SetFittingMode(1);
		Hint.SetPosition(435,50);
		Hint.SetActive(true);
		Hint.SetFontSize(15);
		
		@textAlign1 = CreateCheckBox();
		textAlign1.SetPosition(100,250);
		textAlign1.SetText("Right To Left");
		textAlign1.SetFilledColor(1,0.5,1,1);
		
		@textAlign2 = CreateCheckBox();
		textAlign2.SetPosition(300,250);
		textAlign2.SetText("Right To Left");
		textAlign2.SetFilledColor(1,1,1,1);
		
		@PauseButton = CreateButton();
		PauseButton.SetPosition(0,0);
		PauseButton.SetBitmapPath("Back.png");
		PauseButton.SetRoundedRadius(10,10);
		PauseButton.SetRectangle(0,0,50,50);
		PauseButton.SetDefaultColor(0.0,0.0,0.0,0.0f);
		PauseButton.SetHoverColor(0.1,0.1,0.1,0.1f);
		PauseButton.SetClickColor(0.1,0.1,0.1,0.0f);
		PauseButton.RegisterFunction(funcPause);

		@tabGroup=CreateTabGroup();
		tabGroup.SetHeaderText("Tabs");
		tabGroup.SetPosition(400,400);
		tabGroup.SetRectangle(0,0,200,200);
		//tab.SetBackgroundColor(1,0,0,1);
		tabGroup.SetTabHeaderTextColor(1,1,1,1);

		m_currentTabIndex=0;
	}

	void PauseGameCallBack()
	{
		levelManager.LoadLevel("SampleMain");
	}

	void AddNewTab()
	{
		GUITab @t=CreateTab();
		string text="Tab"+ToString(m_currentTabIndex);
		m_currentTabIndex++;
		t.SetHeaderText(text);
		GUIText @newtext=CreateText();
		newtext.SetText("I'm In"+text);
		newtext.SetEditable(true);
		newtext.SetRectangle(0,0,100,50);
		newtext.SetPosition(10,10);
		t.AddItem(newtext);
		tabGroup.AddTab(t);
	}

	void OnCollide(Actor &sender)
	{
		
	}

	void OnMessage(string msg, const Actor &sender)
	{
		
	}

	Actor @self;
	GUIButton @AddTab;
	GUIText @scrollBar;
	GUIText @scaleDown;
	GUIText @Hint;
	GUICheckBox@ textAlign1;
	GUICheckBox@ textAlign2;
	GUITabGroup @tabGroup;
	GUIButton @PauseButton;
	int m_currentTabIndex;
}
