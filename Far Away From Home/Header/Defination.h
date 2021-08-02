#include <math.h>

//typedef defination
typedef struct Player Player;
typedef struct Bullet Bullet;
typedef struct Asteroid Asteroid;
typedef struct PlayerData PlayerData;
typedef struct Explosion Explosion;

//Function Defination
void ChangeSign (int x, double &a);
void ResetBullet (Bullet &a);
void ChangeRangeAsteroid (Asteroid &a, int mxx, int mxy);
bool CollisionBetweenPlayerAndAstroids (Player &p1, Asteroid &a1);
bool CollisionBetweenBulletAndAstroids (Bullet &p1, Asteroid &a1);
bool CheckRangeBullet (Bullet a, int mxx, int mxy);
bool CheckRangeAsteroid (Asteroid a, int mxx, int mxy);
void IntToString (int n, char *arr);
void StringReverse (char *arr);
void ChangeBool (bool &a);
void SortArray (PlayerData *arr, int n);
void Explode (Explosion &a, int x, int y);

//Structure
struct Player
{
	double x;
	double y;
	double velocity;
	int width;
	int height;
	bool IsHit;
	bool IsThrust;
	bool changeDirection;
	bool firstMove;
	double currentAngle;
	double movingAngle;

	Player (double _x, double _y, int _width, int _height)
	{
		x = _x;
		y = _y;
		width = _width;
		height = _height;
		IsHit = false;
		IsThrust = false;
		velocity = 0.0;
		currentAngle = 0.0;
		changeDirection = false;
		firstMove = true;

		return;
	}

	void ThrustSwitch (void)
	{
		if (IsThrust == true)
			IsThrust = false;
		else
			IsThrust = true;

		return;
	}

	void AddForce (void)
	{
		if (changeDirection)
		{
			velocity -= 0.1;

			if (velocity < 0.0)
			{
				velocity = 0.0;

				movingAngle = currentAngle;

				changeDirection = false;
			}

			return;
		}

		if (firstMove)
		{
			firstMove = false;
			movingAngle = currentAngle;
		}

		velocity += 0.1;

		if (velocity > 3.0)
			velocity = 3.0;

		return;
	}

	void RemoveForce (void)
	{
		velocity -= 0.01;

		if (velocity < 0.0)
			velocity = 0.0;

		if (!firstMove)
			changeDirection = true;

		return;
	}

	void UpdateCoordinates (void)
	{
		x += cos (movingAngle * (acos (-1.0) / 180)) * velocity;
		y += sin (movingAngle * (acos (-1.0) / 180)) * velocity;

		return;
	}

	void CheckRange (void)
	{
		if (x > 1280)
			x = 0 - width;
		else if (x < (-1) * (width))
			x = 1280;
		else if (y > 600)
			y = 0 - height;
		else if (y < (-1) * height)
			y = 600;

		return;
	}

	void RotateAntiCloakWise (void)
	{
		currentAngle += 5;

		if (currentAngle >= 360) currentAngle = 0;

		return;
	}

	void RotateClockWise (void)
	{
		currentAngle -= 5;

		if (currentAngle <= 0) currentAngle = 360;

		return;
	}
};

struct Bullet
{
	int x;
	int y;
	int radius;
	int velocity;
	bool IsSpawned;
	double angle;

	Bullet (void)
	{
		x = 0;
		y = 0;
		velocity = 14;
		angle = 0;
		radius = 2;
		IsSpawned = false;

		return;
	}

	void AddVelocity (void)
	{
		x += cos ( angle * (acos (-1.0) / 180)) * velocity;
		y += sin ( angle * (acos (-1.0) / 180)) * velocity;

		return;
	}
};

struct Asteroid 
{
	double x;
	double y;
	double velocityX;
	double velocityY;
	int width;
	int height;
	bool IsSpawned;
	double angle;

	Asteroid (void)
    {
        x = 0;
        y = 0;
		width = 40;
		height = 40;
		velocityX = 1.5;
		velocityY = 1.5;
		IsSpawned = false;
		angle = 0;

        return;
	}

	void AddVelocity (void)
	{
		x += cos ( angle * (acos (-1.0) / 180)) * velocityX;
		y += sin ( angle * (acos (-1.0) / 180)) * velocityY;

		return;
	}
	 
	void AstCheckRange (void)
	{
		if (x > 1280)
			x = -width;
		else if (x < -width)
			x = 1280;
		else if (y > 600)
			y = -height;
		else if (y < -height)
			y = 600;

		return;
	}

	double AngleCalc (Player p1)
	{
		int diffx = x - p1.x;
		int diffy = y - p1.y;

		if (diffx == 0) {
			if (diffy < 0)
			{
				ChangeSign (0, velocityY);
				return 270;
			}
			if (diffy >= 0)
			{
				ChangeSign (1, velocityY);
				return 90;
			}
		}

		else if (diffy == 0) {
			if (diffx >= 0) {
				ChangeSign (1, velocityY);
				return 180;
			}
			if (diffx < 0)
			{
				ChangeSign (0, velocityY);
				return 0;
			}
		}

		else  {
			if (diffx >= 0) {
				ChangeSign (1, velocityX);
			} else {
				ChangeSign (0, velocityX);
			}

			if (diffy >= 0) {
				ChangeSign (1, velocityY);
			} else {
				ChangeSign (0, velocityY);
			}
		}

		return atan ((double) diffy / diffx) * (180 / acos (-1.0));
	}
};

struct Explosion {
	int x;
	int y;
	double radius;
	bool flip;
	bool IsSpawned;
	double limit;
	bool NoFlip;

	Explosion (void) {
		x = 0;
		y = 0;
		radius = 0.0;
		flip = false;
		IsSpawned = false;
		limit = 20.0;
		NoFlip = false;
	}

	void RadiusChange (void) {
		if (flip == false) {
			if (NoFlip == false)
				radius += 1.0;
			else
				radius += 10.0;

			if (radius == limit && NoFlip == false) {
				flip = true;
			}
		}
		else {
			radius -= 1.0;

			if (radius == 0.0) {
				flip = false;
				IsSpawned = false;
			}
		}
	}
};

struct PlayerData {
	char name[100];
	int highscore;
};

//Custom Methods

bool CollisionBetweenPlayerAndAstroids (Player &p1, Asteroid &a1)
{
	if(p1.x + p1.width > a1.x && p1.x < a1.x + a1.width && p1.y + p1.height > a1.y && p1.y < a1.y + a1.height) {
		return true;
	}

	return false;
}

bool CollisionBetweenBulletAndAstroids (Bullet &p1, Asteroid &a1)
{
	if (p1.x + p1.radius > a1.x && p1.x - p1.radius < a1.x + a1.width && p1.y + p1.radius > a1.y && p1.y - p1.radius < a1.y + a1.height)
	{
		return true;
	}

	return false;
}

void ChangeSign (int x, double &a) {
	if (x == 0) {
		if (a >= 0) {
			return;
		}
		else {
			a *= -1.0;
			return;
		}
	}
	else {
		if (a < 0) {
			return;
		}
		else {
			a *= -1.0;
			return;
		}
	}
}

bool CheckRangeBullet (Bullet a, int mxx, int mxy) {
	if (a.x + a.radius > mxx || a.x - a.radius < 0 || a.y + a.radius > mxy || a.y - a.radius < 0) {
		return true;
	}

	return false;
}

bool CheckRangeAsteroid (Asteroid a, int mxx, int mxy) {
	if (a.x > mxx || a.x + a.width < 0 || a.y > mxy || a.y + a.height < 0) {
		return true;
	}

	return false;
}

void ChangeRangeAsteroid (Asteroid &a, int mxx, int mxy) {
	if (a.x > mxx) {
		a.x = -a.width;
	}
	if (a.x + a.width < 0) {
		a.x = mxx;
	}
	if (a.y > mxy) {
		a.y = -a.height;
	}
	if (a.y + a.height < 0) {
		a.y = 600;
	}

	return;
}

void ResetBullet (Bullet &a) {
	a.x = 0;
	a.y = 0;
	a.angle = 0;
	a.IsSpawned = false;

	return;
}

void ResetAsteroid (Asteroid &a) {
	a.x = 0;
	a.y = 0;
	a.angle = 0;
	a.IsSpawned = false;

	return;
}

void StringReverse (char *arr) {
	int len = strlen (arr);
	char temp[100];
	strcpy (temp, arr);

	for (int i = len - 1; i >= 0; i--) {
		arr[len - i - 1] = temp[i];
	}

	return;
}

void IntToString (int n, char *arr) {
	char *str = (char *) malloc (100);
	int indx = 0;

	while (n) {
		str[indx] = (n % 10) + '0';
		indx++;
		n /= 10;
	}

	str[indx] = '\0';

	StringReverse (str);

	strcpy (arr, str);

	free (str);

	return;
}

void ChangeBool (bool &a) {
	if (a == true) {
		a = false;
	}
	else {
		a = true;
	}

	return;
}

void SortArray (PlayerData *arr, int n) {

    for (int i = 0; i <= n; i++)
    {
        for (int j = i + 1; j <= n; j++)
        {
			if (arr[i].highscore < arr[j].highscore)
            {
                PlayerData temp = arr[i];
                arr[i] = arr[j];
                arr[j] = temp;
            }
        }
    }

    return;
}

void Explode (Explosion &a, int x, int y) {
	a.IsSpawned = true;
	a.x = x;
	a.y = y;
}