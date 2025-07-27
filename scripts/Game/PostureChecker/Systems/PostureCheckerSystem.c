class PostureCheckerSystem : GameSystem
{	
	[Attribute(uiwidget: UIWidgets.CheckBox, defvalue: "true", desc: "Enable Posture Warnings")];
	private bool m_bPostureWarningEnabled;
	
	[Attribute(uiwidget: UIWidgets.Slider, params: "0 120 1", defvalue: "15", desc: "Time between posture warnings (min)", precision: 0)]
	private float m_fPostureWarningTime;
	
	PlayerManager m_playerManager;
	protected float m_fUpdateTime = 1000;
	protected bool m_bInitDone = false;
	
	protected float m_fLastPostureUpdateTime;
	
	protected ref array<ENotification> m_postureCheckMessages;
	
	//------------------------------------------------------------------------------------------------
	static PostureCheckerSystem GetInstance(typename type)
	{
		ChimeraWorld world = GetGame().GetWorld();
		return PostureCheckerSystem.Cast(world.FindSystem(type));
	}
	
	//------------------------------------------------------------------------------------------------
	override static void InitInfo(WorldSystemInfo outInfo)
	{
		super.InitInfo(outInfo);
		outInfo.SetAbstract(true)
			.SetUnique(false)
			.SetLocation(ESystemLocation.Server)
			.AddPoint(ESystemPoint.Frame);
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void OnInit()
	{
		m_playerManager = GetGame().GetPlayerManager();
		
		m_fLastPostureUpdateTime = GetGame().GetWorld().GetWorldTime();
		
		m_fPostureWarningTime = m_fPostureWarningTime * 60 * 1000;
		
		m_postureCheckMessages = {};
		
		array<string> enumNames = {};
		SCR_Enum.GetEnumNames(ENotification, enumNames);
		
		foreach (string name : enumNames)
		{
			if (name.Contains("PostureChecker_"))
			{
				ENotification notification = typename.StringToEnum(ENotification, name);				
				m_postureCheckMessages.Insert(notification);
			}
		}
		
		m_bInitDone = true;
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void OnUpdate(ESystemPoint point)
	{		
		float currentTime = GetGame().GetWorld().GetWorldTime();
		
		if (IsPostureWarningEnabled())
		{
			if (m_fLastPostureUpdateTime + m_fPostureWarningTime < currentTime)
			{
				Print("PostureChecker | Sending posture warning notification...");
				SendNotification(m_postureCheckMessages.GetRandomElement());
				m_fLastPostureUpdateTime = GetGame().GetWorld().GetWorldTime();
			}
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void SendNotification(ENotification type)
	{
		auto playerManager = GetGame().GetPlayerManager();
		if (!playerManager)
			return;
		
		array<int> allPlayers = {};
		m_playerManager.GetAllPlayers(allPlayers);
		
		foreach (int playerId : allPlayers)
		{
			SCR_NotificationsComponent.SendToPlayer(playerId, type);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsPostureWarningEnabled() { return m_bPostureWarningEnabled; };
}
