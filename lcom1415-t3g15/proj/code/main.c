#include <minix/sysutil.h>
#include <minix/syslib.h>
#include <minix/drivers.h>

#include "MainMenu.h"
#include "OptionsMenu.h"
#include "Whack.h"
#include "Game.h"
#include "Mouse.h"
#include "i8042.h"

#define BIT(n) (0x01<<(n))

int main(int argc, char **argv) {
	srand(time(NULL));
	sef_startup();

	Whack* whack = startWhack();
	vg_init(0x114);
	int done = 0; //whack->done;
	//Bitmap* board = loadBitmap("/home/lcom/lcom1415-t3g15/projeto/res/board.bmp");
	//Bitmap* bt = loadBitmap("/home/lcom/lcom1415-t3g15/projeto/res/exit-button.bmp");
	//drawBitmapWithoutBg(board, 0, 0, ALIGN_LEFT);
	//drawBitmapWithoutBg(bt, 200, 120, ALIGN_LEFT);

	//MainMenu* teste = createMainMenu();
	//drawMainMenu(teste);

	//deleteMainMenu(teste);

	/*Game* teste = createGame();
	//OptionsMenu* teste = createOptionsMenu(0, 0);
		drawGame(teste);
	flipMouse();
					drawMouse();
					flipDisplay();

		sleep(10);

	deleteGame(teste);*/

	while (!whack->done) {
		updateWhack(whack);
		if (get_counter(whack->timer) % 4 == 0){
			if (!whack->done && whack->draw) {
				drawWhack(whack);
			}

			if (getMouse()->draw) {
				flipMouse();
				drawMouse();
				flipDisplay();
			}
		}
	}
	stopWhack(whack);


	vg_exit();



	printf("\nTerminated\n");

	return 0;
}
