#include "Header\iGraphics.h"
#include "Header\Defination.h"

//Constant Variable

#define ScreenWidth 1280
#define ScreenHeight 600
#define BulletMaxSize 50
#define AsteroidMaxSize 10
#define MaxScreenAsteroid 6
#define ExplosionMax 6

#define GameStateMainMenu 0
#define GameStateUserName 1
#define GameStateCurrentGame 2
#define GameStateHighScore 3
#define GameStateStory 4
#define GameStateGameOver 5
#define GameStateCredit 6

//Variable

int Gamestate = 0;
int SpawnSide = 0;
int CurrentAsteroid = 0;
int HighScore = 0;
char HighScoreStr[100];
char PlayerName[100];
char PlayerNameTemp[100];
int PlayerNameLen = 0;
bool Pause = false;
bool EnterGame = false;
bool ChangePlayerModel = false;
Explosion final;
FILE *fp;

//Array Index

int BulletIndx = 0;
int AstIndx = 0;
int AstTextureIndx = 0;
int ExIndx = 0;

//Texture Index

int SpaceShipTexture1;
int SpaceShipTexture2;
int HomePageTexture;
int FAFHTexture;
int PlayTexture;
int HighScoreTexture;
int CreditTexture;
int QuitTexture;
int *AsteroidTexture = new int[5];

//Array

Player p1 (ScreenWidth / 2 - 50 / 2, ScreenHeight / 2 - 50 / 2, 50, 50);
Bullet *BulletArray = new Bullet[BulletMaxSize];
Asteroid *AsteroidArray = new Asteroid[AsteroidMaxSize];
Explosion *ExplosionArray = new Explosion[ExplosionMax];
int Hover[5];

//Reset

void ResetGame (void) {
	p1.x = ScreenWidth / 2 - 50 / 2;
	p1.y = ScreenHeight / 2 - 50 / 2;
	p1.currentAngle = 0;
	p1.movingAngle = 0;
	p1.velocity = 0.0;
	p1.firstMove = true;
	p1.IsThrust = false;

	for (int i = 0; i < BulletMaxSize; i++) {
		ResetBullet (BulletArray[i]);
	}

	for (int i = 0; i < AsteroidMaxSize; i++) {
		ResetAsteroid (AsteroidArray[i]);
	}

	CurrentAsteroid = 0;
	HighScore = 0;
	EnterGame = false;
	ChangePlayerModel = false;
	final.NoFlip = false;
	final.radius = 0.0;

	return;
}

//Spawn

void AsetroidSpawn (void) {
	if (CurrentAsteroid < MaxScreenAsteroid) {
		CurrentAsteroid++;

		while (AsteroidArray[AstIndx].IsSpawned) {
			AstIndx++;

			if (AstIndx == AsteroidMaxSize) {
				AstIndx = 0;
			}
		}

		switch (SpawnSide) {
			case 0 :
				AsteroidArray[AstIndx].x = rand () % 1280;
				AsteroidArray[AstIndx].y = 600;
				break;
			case 1 :
				AsteroidArray[AstIndx].x = rand () % 1280;
				AsteroidArray[AstIndx].y = -AsteroidArray[AstIndx].height;
				break;
			case 2 :
				AsteroidArray[AstIndx].x = -AsteroidArray[AstIndx].width;
				AsteroidArray[AstIndx].y = rand () % 600;
				break;
			case 3 :
				AsteroidArray[AstIndx].x = 1280;
				AsteroidArray[AstIndx].y = rand () % 600;
				break;
		}

		AsteroidArray[AstIndx].angle = AsteroidArray[AstIndx].AngleCalc (p1);
		AsteroidArray[AstIndx].IsSpawned = true;

		SpawnSide = SpawnSide == 3 ? 0 : SpawnSide + 1;
	}

	return;
}

//Texture Initialization

void TextureInit (void)
{
	//Player Model
	SpaceShipTexture1 = iLoadImage ("SpaceShip\\SpaceShip1.png");
	SpaceShipTexture2 = iLoadImage ("SpaceShip\\SpaceShip2.png");
	AsteroidTexture[0] =  iLoadImage ("Astroids\\Astroid1.png");
	AsteroidTexture[1] =  iLoadImage ("Astroids\\Astroid2.png");
	AsteroidTexture[2] =  iLoadImage ("Astroids\\Astroid3.png");
	AsteroidTexture[3] =  iLoadImage ("Astroids\\Astroid4.png");
	AsteroidTexture[4] =  iLoadImage ("Astroids\\Astroid5.png");

	//Main Menu

	HomePageTexture = iLoadImage ("bg\\wp1.png");
	PlayTexture = iLoadImage ("Icon\\play.png");
	HighScoreTexture = iLoadImage ("Icon\\highscore.png");
	QuitTexture = iLoadImage ("Icon\\quit.png");
	FAFHTexture = iLoadImage ("Icon\\FAFH.png");
	CreditTexture = iLoadImage ("Icon\\credit.png");

	return;
}

//Collsion

void CollsionCheck (void) {
	for (int i = 0; i < BulletMaxSize; i++) {
		if (BulletArray[i].IsSpawned) {
			for (int j = 0; j < AsteroidMaxSize; j++) {
				if (AsteroidArray[j].IsSpawned && CollisionBetweenBulletAndAstroids (BulletArray[i], AsteroidArray[j])) {
					Explode (ExplosionArray[ExIndx], BulletArray[i].x, BulletArray[i].y);
					ExIndx++;
					if (ExIndx == 6) {
						ExIndx = 0;
					}
					ResetBullet (BulletArray[i]);
					ResetAsteroid (AsteroidArray[j]);
					HighScore += 5;
					CurrentAsteroid--;
					PlaySound("Music\\shoot.wav", NULL,SND_ASYNC);
				}
			}
		}
	}

	for (int i = 0; i < AsteroidMaxSize; i++) {
		if (AsteroidArray[i].IsSpawned) {
			if (CollisionBetweenPlayerAndAstroids (p1, AsteroidArray[i])) {
				if (Pause == false) {
					final.limit = 1e20;
					final.NoFlip = true;
					Explode (final, AsteroidArray[i].x, AsteroidArray[i].y);
					ChangeBool (Pause);
					Gamestate = GameStateGameOver;

					fp = fopen ("score", "a");
                    fprintf (fp, "%s %d\n", PlayerName, HighScore);
                    fclose (fp);
				}
			}
		}
	}

	return;
}

//Rendering

void ExplosionRender (void) {
	for (int i = 0; i < ExplosionMax; i++) {
		if (ExplosionArray[i].IsSpawned) {
			iFilledCircle (ExplosionArray[i].x, ExplosionArray[i].y, ExplosionArray[i].radius);
		}
	}

	if (final.NoFlip) {
		iFilledCircle (final.x, final.y, final.radius);
	}

	return;
}

void CreditUI (void) {
	iShowImage(0,0,1280,600, HomePageTexture); 
	iText (1060, 10, "Press BackSpace to go back.");

	iText (66, 469, "Player Movement :");
	iText (66, 457, "Ayon");
	iText (66, 445, "Poushi");
	iText (66, 433, "Shila");

	iText (66, 407, "Asteroid :");
	iText (66, 395, "Mumu");
	iText (66, 383, "Shila");
	iText (66, 371, "Oyshe");

	iText (66, 345, "Bullet :");
	iText (66, 333, "Mumu");
	iText (66, 321, "Shila");

	iText (66, 295, "Enemy Spawn :");
	iText (66, 283, "Ayon");
	iText (66, 271, "Poushi");

	iText (435, 469, "Collision :");
	iText (435, 457, "Ayon");
	iText (435, 445, "Oyshe");

	iText (435, 407, "GameBehaviourState :");
	iText (435, 395, "Ayon");

	iText (435, 345, "Menu :");
	iText (435, 333, "Mumu");
	iText (435, 321, "Oyshe");

	iText (435, 295, "Sound :");
	iText (435, 283, "Mumu");
	iText (435, 271, "Oyshe");

	iText (899, 469, "HighScore :");
	iText (899, 457, "Ayon");
	iText (899, 445, "Shila");

	iText (899, 407, "Credit :");
	iText (899, 395, "Ayon");
	iText (899, 383, "Shila");

	iText (899, 345, "Animation :");
	iText (899, 333, "Poushi");

	iText (899, 295, "ImageRendering :");
	iText (899, 283, "Poushi");

	return;
}

void GameOver (void) {
	iSetColor (0, 0, 0);
	iText (550, 377, "Game Over", GLUT_BITMAP_TIMES_ROMAN_24);
	iText (550, 353, "Press BackSpace to go back.", GLUT_BITMAP_TIMES_ROMAN_24);
	iSetColor (255, 255, 255);

	return;
}

void HighScoreUI (void) {
	iShowImage(0,0,1280,600, HomePageTexture);
	iText (1060, 10, "Press BackSpace to go back.");

	fp = fopen ("score", "r");

	int index = 0;

	PlayerData arr[100];
	char tempname[100];
	int tempscore;

	while (fscanf (fp, "%s %d", tempname, &tempscore) != EOF) {
		arr[index].highscore = tempscore;
		strcpy (arr[index].name, tempname);

		index++;
	}
	
	fclose (fp);
	
	SortArray (arr, index);

	iText (475, 440, "HighScore", GLUT_BITMAP_TIMES_ROMAN_24);

	char temp[100];
	IntToString (arr[0].highscore, temp);

	iText (370, 370, arr[0].name, GLUT_BITMAP_TIMES_ROMAN_24);
	iText (670, 370, temp, GLUT_BITMAP_TIMES_ROMAN_24);

	IntToString (arr[1].highscore, temp);

	iText (370, 300, arr[1].name, GLUT_BITMAP_TIMES_ROMAN_24);
	iText (670, 300, temp, GLUT_BITMAP_TIMES_ROMAN_24);

	IntToString (arr[2].highscore, temp);

	iText (370, 230, arr[2].name, GLUT_BITMAP_TIMES_ROMAN_24);
	iText (670, 230, temp, GLUT_BITMAP_TIMES_ROMAN_24);

	return;
}

void MainMenuUI (void) {
	iShowImage(0,0,1280,600, HomePageTexture);
	iShowImage(290,420,743,46, FAFHTexture);
	iShowImage(565,194,109,39, PlayTexture);
	iShowImage(490,154,255,40, HighScoreTexture);
	iShowImage(551,120,140,30, CreditTexture);
	iShowImage(564,74,110,38, QuitTexture);

	if(Hover[0]==1){
		iRectangle(565,194,113,39);
	}
	if(Hover[1]==1){
		iRectangle(490,154,255,40);
	}
	if(Hover[2]==1){
		iRectangle(548,115,148,38);
	}
	if(Hover[3]==1){
		iRectangle(564,74,112,38);
	}

	return;
}

void UserNameUI (void) {
	iShowImage(0,0,1280,600, HomePageTexture);

	iSetColor (150, 150, 150);
	iRectangle (515, 250, 250, 20);

	iSetColor (255, 255, 255);

	iText (520, 280, "Enter Your Name: ");

	iText (520, 257, PlayerNameTemp);

	return;
}

void UIRenderInGame (void) {
	if (Gamestate == GameStateGameOver) {
		iSetColor (0, 0, 0);
	}
	iText (1000, 10, "Pilot: ", GLUT_BITMAP_TIMES_ROMAN_24);
	iText (1060, 10, PlayerName, GLUT_BITMAP_TIMES_ROMAN_24);

	if (HighScore != 0) {
		IntToString (HighScore, HighScoreStr);
	}
	else {
		strcpy (HighScoreStr, "0");
	}

	iText (0, 577, "Score: ", GLUT_BITMAP_TIMES_ROMAN_24);
	iText (100, 577, HighScoreStr, GLUT_BITMAP_TIMES_ROMAN_24);

	int OldBulletIndx = BulletIndx - 1;
	if (OldBulletIndx < 0) OldBulletIndx = BulletMaxSize - 1;

	if (BulletArray[OldBulletIndx].IsSpawned == true) {
		iText (0, 10, "Loading!", GLUT_BITMAP_TIMES_ROMAN_24);
	} else {
		iText (0, 10, "Ready to Fire!", GLUT_BITMAP_TIMES_ROMAN_24);
	}

	if (Gamestate == GameStateGameOver)
		iText (550, 577, "You are gonna carry that weight?", GLUT_BITMAP_TIMES_ROMAN_24);
	else
		iText (550, 577, "Only One Life Remaining..", GLUT_BITMAP_TIMES_ROMAN_24);

	if (Pause == true && Gamestate != GameStateGameOver) {
		iText (900, 577, "Paused", GLUT_BITMAP_TIMES_ROMAN_24);
	}
	if (Gamestate == GameStateGameOver) {
		iSetColor (255, 255, 255);
	}

	return;
}

void PlayerRender (void)
{
	iRotate (p1.x + p1.width / 2, p1.y + p1.height / 2, p1.currentAngle);

	if (ChangePlayerModel == true)
		iShowImage (p1.x, p1.y, p1.width, p1.height, SpaceShipTexture2);
	else
		iShowImage (p1.x, p1.y, p1.width, p1.height, SpaceShipTexture1);

	iUnRotate ();
	
	p1.CheckRange ();

	if (p1.IsThrust == true) {
		p1.AddForce ();
		ChangePlayerModel = true;
	}
	else {
		p1.RemoveForce ();
		ChangePlayerModel = false;
	}

	return;
}

void BulletRender (void)
{
	for (int i = 0; i < BulletMaxSize; i++) {
		if (BulletArray[i].IsSpawned) {
			iFilledCircle (BulletArray[i].x, BulletArray[i].y, BulletArray[i].radius);
		}
	}

	for (int i = 0; i < BulletMaxSize; i++) {
		if (BulletArray[i].IsSpawned && CheckRangeBullet (BulletArray[i], ScreenWidth, ScreenHeight)) {
			ResetBullet (BulletArray[i]);
		}
	}

	return;
}

void AsteroidRender (void)
{   
	int j = 0;
	for (int i = 0; i < AsteroidMaxSize; i++) {
		
		if (AsteroidArray[i].IsSpawned) {
			iShowImage(AsteroidArray[i].x, AsteroidArray[i].y, AsteroidArray[i].width, AsteroidArray[i].height, AsteroidTexture[j]);
			
			j++;
			
			if (j == 5)
				j = 0;

			if (CheckRangeAsteroid (AsteroidArray[i], ScreenWidth, ScreenHeight)) {
				ChangeRangeAsteroid (AsteroidArray[i], ScreenWidth, ScreenHeight);
			}
		}
	}

	return;
}

//Shooting Mechanics

void Shoot (void)
{
	BulletArray[BulletIndx].x = p1.x + p1.width / 2;
	BulletArray[BulletIndx].y = p1.y + p1.height / 2;
	BulletArray[BulletIndx].angle = p1.currentAngle;
	BulletArray[BulletIndx].IsSpawned = true;

	BulletIndx++;
	

	if (BulletIndx == BulletMaxSize) 
		BulletIndx = 0;

	return;
}

//SetTimer Methods

void PlayerPhysics (void)
{
	if (Pause == false) {
		p1.UpdateCoordinates ();
	}

	return;
}

void BulletPhysics (void)
{
	if (Pause == false) {
		for (int i = 0; i < 50; i++) {
			if (BulletArray[i].IsSpawned) {
				BulletArray[i].AddVelocity ();
			}
		}
	}

	return;
}

void AsteroidPhysics (void) {
	if (Pause == false) {
		for (int i = 0; i < AsteroidMaxSize; i++) {
			if (AsteroidArray[i].IsSpawned) {
				AsteroidArray[i].AddVelocity ();
			}
		}
	}

	return;
}

void ChangeExplosion (void) {
	if (Pause == false) {
		for (int i = 0; i < ExplosionMax; i++) {
			if (ExplosionArray[i].IsSpawned) {
				ExplosionArray[i].RadiusChange ();
			}
		}
	}

	if (final.NoFlip) {
		final.RadiusChange ();
	}
}

//iGraphics Methods

void iDraw (void)
{
	iClear ();

	switch (Gamestate) {
		case GameStateMainMenu:
				MainMenuUI ();
				break;
		case GameStateUserName:
				UserNameUI ();
				break;
		case GameStateGameOver:
				AsetroidSpawn ();
				CollsionCheck ();
				PlayerRender ();
				BulletRender ();
				AsteroidRender ();
				ExplosionRender ();
				GameOver ();
				UIRenderInGame ();
				break;
		case GameStateCurrentGame: 
				if (EnterGame == false) {
					PlaySound (0,0,0);
					EnterGame = true;
				}
				UIRenderInGame ();
				AsetroidSpawn ();
				CollsionCheck ();
				PlayerRender ();
				BulletRender ();
				AsteroidRender ();
				ExplosionRender ();
				break;
		case GameStateHighScore:
				HighScoreUI ();
				break;
		case GameStateCredit:
				CreditUI ();
				break;
	}
	
	return;
}

void iMouse (int button, int state, int mx, int my)
{
	if (Gamestate == GameStateMainMenu && button == GLUT_LEFT_BUTTON)
	{
		if (mx >= 552 && mx <= 665 && my >= 211 && my <= 235)
		{
			Gamestate = GameStateUserName;
		}
		if (mx >= 481 && mx <= 728 && my >= 171 && my <= 193)
		{
			Gamestate = GameStateHighScore;
		}
		if(mx >= 556 && mx <= 688 && my >= 123 && my <= 150)
		{
			Gamestate = GameStateCredit;
		}
		if(mx >= 552 && mx <= 663 && my >= 81 && my <= 105)
		{
			exit(0);
		}
	}

	else if (Gamestate == GameStateCurrentGame) {
		if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && Pause == false)
		{
			int OldBulletIndx = BulletIndx - 1;
			if (OldBulletIndx < 0) OldBulletIndx = BulletMaxSize - 1;

			if (BulletArray[OldBulletIndx].IsSpawned == false) Shoot ();
		}
	}

	return;
}

void iMouseMove (int mx, int my)
{
	
	return;
}

void iPassiveMouseMove (int mx, int my)
{
	if(Gamestate == GameStateMainMenu)
	{
		if(mx >= 552 && mx <= 665 && my >= 211 && my <= 235)
		{
			Hover[0]=1;
		}
		else
		{
			Hover[0]=0;
		}
		if(mx >= 481 && mx <= 728 && my >= 171 && my <= 193)
		{
			Hover[1]=1;
		}
		else
		{
			Hover[1]=0;
		}
		if(mx >= 556 && mx <= 688 && my >= 123 && my <= 150)
		{
			Hover[2]=1;
		}
		else
		{
			Hover[2]=0;
		}
		if(mx >= 552 && mx <= 663 && my >= 81 && my <= 105)
		{
			Hover[3]=1;
		}
		else
		{
			Hover[3]=0;
		}
	}

	return;
}

void iKeyboard (unsigned char key)
{
	if (Gamestate == GameStateUserName) {
		if (key == '\r') {
			Gamestate = GameStateCurrentGame;
			PlayerNameTemp[PlayerNameLen] = '\0';
			strcpy (PlayerName, PlayerNameTemp);
			
			for (int i = 0; i < PlayerNameLen; i++) {
				PlayerNameTemp[i] = '\0';
			}

			PlayerNameLen = 0;
		} else if (key == '\b') {
			PlayerNameLen--;

			if (PlayerNameLen < 0) {
				PlayerNameLen = 0;
			}

			PlayerNameTemp[PlayerNameLen] = '\0';
		} else {
			PlayerNameTemp[PlayerNameLen] = key;
			PlayerNameLen++;
		}
	}
	else if (Gamestate == GameStateCurrentGame) {
		if (key == '\r') {
			ChangeBool (Pause);
		}
		if (Pause == false) {
			if (key == 'w') {
				p1.ThrustSwitch ();
			}
			if (key == 'd') {
				p1.RotateClockWise ();
			}
			if (key == 'a') {
				p1.RotateAntiCloakWise ();
			}
		}
	} else if (Gamestate == GameStateHighScore) {
		if (key == '\b') {
			Gamestate = GameStateMainMenu;
		}
	} else if (Gamestate == GameStateGameOver) {
		if (key == '\b') {
			ChangeBool (Pause);
			ResetGame ();
			Gamestate = GameStateMainMenu;
			PlaySound("Music\\menu.wav",NULL,SND_LOOP | SND_ASYNC);
		}
	} else if (Gamestate == GameStateCredit) {
		//Play Credit Music
		if (key == '\b') {
			Gamestate = GameStateMainMenu;
			//Play Menu Music
		}
	}

    return;
}

void iSpecialKeyboard (unsigned char key)
{
	
	return;
}

int main (void)
{
	srand (time (NULL));

	iSetTimer (5, PlayerPhysics);
	iSetTimer (5, BulletPhysics);
	iSetTimer (5, AsteroidPhysics);
	iSetTimer (5, ChangeExplosion);

	iInitialize (ScreenWidth, ScreenHeight, "Far Away From Home");

	TextureInit ();

	PlaySound("Music\\menu.wav",NULL,SND_LOOP | SND_ASYNC);

	iStart ();

	return 0;
}