#include <minix/drivers.h>
#include <minix/sysutil.h>
#include <minix/com.h>
#include <stdio.h>
#include "Numbers.h"
#include "i8042.h"
#include "i8254.h"
#include "Graphics.h"
#include "Utilities.h"

static int HOOK_ID;
Numbers* numbers = NULL;

Numbers* createNumbers() {
	Numbers* numbers = (Numbers*) malloc(sizeof(Numbers));

	numbers->numbs[0] = loadBitmap("home/lcom/lcom1415-t3g15/projeto/res/zeroBMP.bmp");
	numbers->numbs[1] = loadBitmap("home/lcom/lcom1415-t3g15/projeto/res/umBMP.bmp");
	numbers->numbs[2] = loadBitmap("home/lcom/lcom1415-t3g15/projeto/res/doisBMP.bmp");
	numbers->numbs[3] = loadBitmap("home/lcom/lcom1415-t3g15/projeto/res/tresBMP.bmp");
	numbers->numbs[4] = loadBitmap("home/lcom/lcom1415-t3g15/projeto/res/quatroBMP.bmp");
	numbers->numbs[5] = loadBitmap("home/lcom/lcom1415-t3g15/projeto/res/cincoBMP.bmp");
	numbers->numbs[6] = loadBitmap("home/lcom/lcom1415-t3g15/projeto/res/seisBMP.bmp");
	numbers->numbs[7] = loadBitmap("home/lcom/lcom1415-t3g15/projeto/res/seteBMP.bmp");
	numbers->numbs[8] = loadBitmap("home/lcom/lcom1415-t3g15/projeto/res/oitoBMP.bmp");
	numbers->numbs[9] = loadBitmap("home/lcom/lcom1415-t3g15/projeto/res/noveBMP.bmp");
	return numbers;
}

void drawNumber(int n, int x, int y) {

	if (numbers == NULL)
			numbers = createNumbers();
	if(n < 10)
		drawBitmapWithoutBg(numbers->numbs[n], x, y, ALIGN_LEFT);
	else
	{
		int temp = n %10;
		drawBitmapWithoutBg(numbers->numbs[temp], x, y, ALIGN_LEFT);
		n /=10;
		int dx= x-40;
		drawNumber(n,dx,y);
	}
}

void deleteNumbers() {
}
