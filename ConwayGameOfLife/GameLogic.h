#pragma once
#include <string>
#include "Constants.h"

class GameLogic
{
	bool fieldBufferFront[g_unitsX * g_unitsY];
	bool fieldBufferBack[g_unitsX * g_unitsY];
public:
	bool isPause = false;
	bool isFlame = false;
	bool isSpeedUp = false;

	int timeStamp = 0;
	int cursorPosX = 0,
		cursorPosY = 0;

	int getElementId(int x, int y)
	{
		if (x < 0)
			x += g_unitsX;
		else if (x >= g_unitsX)
			x -= g_unitsX;

		if (y < 0)
			y += g_unitsY;
		else if (y >= g_unitsY)
			y -= g_unitsY;

		return x + y * g_unitsX;
	}

	bool getElement(int x, int y)
	{
		return fieldBufferFront[getElementId(x, y)];
	}

	bool gameRule(bool isAlive, int alivesNear)
	{
		return ((alivesNear == 3) || (isAlive && alivesNear == 2));
	}

	void invertElement(int x, int y)
	{
		int realId = getElementId(x, y);
		fieldBufferBack[realId] = !fieldBufferFront[realId];
		fieldBufferFront[realId] = !fieldBufferFront[realId];
	}

	void cleanField()
	{
		memset(fieldBufferBack, false, g_unitsX * g_unitsY * sizeof(bool));
		memset(fieldBufferFront, false, g_unitsX * g_unitsY * sizeof(bool));
	}

	void tick()
	{
		if (isPause)
			return;

		for (int iy = 0; iy < g_unitsY; iy++)
		{
			for (int ix = 0; ix < g_unitsX; ix++)
			{
				int alives = 0;

				for (int jy = -1; jy <= 1; jy++)
					for (int jx = -1; jx <= 1; jx++)
						alives += getElement(ix + jx, iy + jy);

				alives -= getElement(ix, iy);

				fieldBufferBack[getElementId(ix, iy)] = gameRule(fieldBufferBack[getElementId(ix, iy)], alives);
			}
		}

		memcpy(fieldBufferFront, fieldBufferBack, g_unitsX * g_unitsY * sizeof(bool));
	}
};

