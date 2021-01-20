class pt
{
public:
	int x;
	int y;
};

class mypoints : public pt  //상속
{
	//public:   
private:
	pt p[1000]; //pt 클래스의 (instance, object) 배열
	int iCount;
public:
	void Add(int x, int y)
	{
		if (iCount < 1000)
		{
			p[iCount].x = x;
			p[iCount].y = y;
			iCount++;
		}

	}
	void Draw(HDC hdc)
	{
		HBRUSH hBrush;
		hBrush = CreateSolidBrush(RGB(255, 0, 0));
		SelectObject(hdc, hBrush);
		for (int i = 0; i < iCount; i++)
		{
			Rectangle(hdc, p[i].x - 10, p[i].y - 10,
				p[i].x + 10, p[i].y + 10);
		}
	}
};