#include <iostream>
#include <string.h>
#include <SDL.h>
#include <SDL_image.h>
#include "..\include\SDL_helper.h"
#include <res_path.h>
#include <cleanup.h>
#include "Minefield.h"
#include "mineFieldIO.h"
#include "main.h"
#include <ctime>

const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 810;

const int BUTTON_WIDTH = 200;
const int BUTTON_HEIGHT = 100;
const int DIFFICULTY_AMOUNT = 3;//the amount of difficulty options
const int CELL_SIZE = 16;

const int SMILE_SIZE = 26;
const int SMILE_AMOUNT = 5;

const int DIGIT_WIDTH = 13;//width of each digit sprite
const int DIGIT_HEIGHT = 22;//height of each digit sprite
const int DIGIT_AMOUNT = 11;//10 digits + negative sign

/*
 * converts an int into each digit at the Hundreds, Tens, and Ones position
 * @param value: number to be converted. Cannot be larger than 999
 * @param digitArray: an array of size 3 containing the value at the Hundreds, Tens, and Ones position in that order
 * @return: returns and stores the new values in digitArray
*/
void intToDigit(int value, int* digitArray) {
	//if the value is negative, set the hundreds position to negative
	if (value < 0) {
		digitArray[0] = 10;//the last index contains the negative symbol
		value *= -1;//set the value to positive to prevent unexpected behaviour
	} else {
		digitArray[0] = (value % 1000) / 100;// need to turn it into a single digit values by dividing it by it's position

	}

	digitArray[1] = (value % 100) / 10;// need to turn it into a single digit values by dividing it by it's position

	//find the ones position
	digitArray[2] = value % 10;
}

int main(int, char**) {
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		logSDLError(std::cout, "Window not intialized.");
		return 1;
	}
	//loading the image subsystem
	if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG) {
		logSDLError(std::cout, "IMG_Init");
		SDL_Quit();
		return 1;
	}


	//creating window
	SDL_Window *window = SDL_CreateWindow("cSweeper++", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH,
		SCREEN_HEIGHT, (SDL_WINDOW_SHOWN ) );
	if (window == nullptr) {
		logSDLError(std::cout, "CreateWindow");
		SDL_Quit();
		return 1;
	}

	//creating renderer
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	if (renderer == nullptr) {
		logSDLError(std::cout, "CreateRenderer");
		cleanup(window);
		SDL_Quit();
		return 1;
	}

	//loading the background and foreground images
	const std::string resPath = getResourcePath("difficulty");
	SDL_Texture* spriteSheet = loadTexture(resPath + "difficultyButtons.png", renderer );
	
	//ensure that they load

	if (spriteSheet == nullptr ) {
        logSDLError(std::cout, "SpriteSheet-Difficulty");
		cleanup(spriteSheet, renderer, window);
		IMG_Quit();
		SDL_Quit();
	}
	
	//clipping the buttons out

	int iW = BUTTON_WIDTH ;
	int iH = BUTTON_HEIGHT;
	int x = SCREEN_WIDTH / 2 - iW / 2;
	int y = SCREEN_HEIGHT / 2 - iH / 2;

	SDL_Rect DifficultyButtons[DIFFICULTY_AMOUNT*2];//2 images per button
		
	//
	for (int i = 0; i < DIFFICULTY_AMOUNT*2; i++){
		DifficultyButtons[i].x = i / 2 * iW;
		DifficultyButtons[i].y = i % 2 * iH;
		DifficultyButtons[i].w = iW;
		DifficultyButtons[i].h = iH;
	}



	
	//create a slight separation between the buttons
	int buttonDifferenceW = BUTTON_WIDTH + 10;
	



	
	//a rectangle 1 pixel in size to detect where the mouse is
	SDL_Rect mouseDetect;
	mouseDetect.w = 1;
	mouseDetect.h = 1;

	//create a rectangle array to contain the position of each button
	SDL_Rect buttonPos[DIFFICULTY_AMOUNT];
	buttonPos[0].x = x - buttonDifferenceW;
	buttonPos[1].x = x;
	buttonPos[2].x = x + buttonDifferenceW;

	for (int i = 0; i < DIFFICULTY_AMOUNT; i++){
		buttonPos[i].h = BUTTON_HEIGHT;
		buttonPos[i].w = BUTTON_WIDTH;
		buttonPos[i].y = y;
	}
	//track which sprite to show for each button
	int buttonPress[DIFFICULTY_AMOUNT] = { 0, 2 ,4 };
	int whichPressed = -1;//track which button has been pressed

	
	//define quit here so the second loop can be skipped if user exits
	bool quit = false;
	bool diffChosen = false;
	
	SDL_Event eDiff;//storage variable for incoming inputs during the choosing of difficulty
	
	int difficultyChosen = 0;//contains the difficulty chosen by the player
	int windowScale = 2;	// scales the cell size for larger monitors

	//difficulty selection loop
	//exits once a difficulty has been chosen
	while (!diffChosen){
		

		//while there are events to be processed
		while (SDL_PollEvent(&eDiff)){
			if (eDiff.type == SDL_MOUSEBUTTONDOWN){
				//set the mousedetect to the mouse's current position
				mouseDetect.x = eDiff.button.x;
				mouseDetect.y = eDiff.button.y;
				
				//check if it intersects with any of the buttons
				for (int i = 0; i < DIFFICULTY_AMOUNT; i++){
					if (SDL_HasIntersection(&mouseDetect, &buttonPos[i]) == SDL_TRUE){
						buttonPress[i]++;
						difficultyChosen = i;
						whichPressed = i;
					}

				}
				
			}
			//unpress the pressed button
			if (eDiff.type == SDL_MOUSEBUTTONUP){
				if (whichPressed != -1){
					buttonPress[whichPressed]--;
					whichPressed = -1;
					diffChosen = true;
				}
			}
			//If user closes the window
			if (eDiff.type == SDL_QUIT) {
				quit = true;
				diffChosen = true;
			}
			if (eDiff.type == SDL_KEYDOWN){

				if (eDiff.key.keysym.sym == SDLK_ESCAPE){
					quit = true;
					diffChosen = true;
				}
			}

		}

		SDL_RenderClear(renderer);

		//render a background colour
		SDL_SetRenderDrawColor(renderer, 192, 192, 192, 255);
		//render each difficulty button
		for (int i = 0; i < DIFFICULTY_AMOUNT; i++){
			renderTexture(spriteSheet, renderer, buttonPos[i], &DifficultyButtons[buttonPress[i]]);
		}
		
		SDL_RenderPresent(renderer);
	}

	//defining an array containing settings for the various difficulty options
	int difficultyOptions[3][3] = { {7,7,4},//easy
									{10,10,15},//mediium
									{25,25,155} };//hard
	
	//set window size to the necessary size to fit all the cells
	
	int topBarOffset = 30 * windowScale;//an offset to make room for the clock, smily face, etc.
	int newWinWidth = difficultyOptions[difficultyChosen][0] * CELL_SIZE * windowScale;

	//add offset to height to make room for topbar. Scale it to match window window size
	int newWinHeight = (difficultyOptions[difficultyChosen][1] * CELL_SIZE * windowScale )  + (topBarOffset );
	
	SDL_SetWindowSize(window, newWinWidth, newWinHeight);//
	SDL_RenderSetScale(renderer, (float)windowScale, (float)windowScale);
	



	//clipping the sprites for the Smiley face
	const std::string smileyPath = getResourcePath("");
	SDL_Texture* smileSheet = loadTexture(smileyPath + "FaceSprites.png", renderer);
	
	if ( smileSheet== nullptr) {
		logSDLError(std::cout, "SpriteSheet-smile");
		cleanup(smileSheet, renderer, window);
		IMG_Quit();
		SDL_Quit();
	}

	//clipping the smily faces out
	
	int smileW = SMILE_SIZE;
	int smileH = SMILE_SIZE;
	int xSmile = (newWinWidth / 2) -(smileW);
	int ySmile = (topBarOffset / 2) - (smileH);

	SDL_Rect smilyButtons[SMILE_AMOUNT];
	int xOffset = 0;//include an offset to account for the gap between textures
	for (int i = 0; i < SMILE_AMOUNT ; i++) {
		smilyButtons[i].x = (i * smileW) + xOffset;
		smilyButtons[i].y = 0;
		smilyButtons[i].w = smileW;
		smilyButtons[i].h = smileH;
		xOffset++;//increment the offset to include previous gap as well
	}
	
	SDL_Rect SmilePos;
	SmilePos.x = xSmile / windowScale;
	SmilePos.y = ySmile / windowScale;
	SmilePos.h = smileH;
	SmilePos.w = smileW;

	/*clipping Digit sprites*/

	//getting sprite 
	const std::string digitpath = getResourcePath("");
	SDL_Texture* digitSheet = loadTexture(digitpath + "clock.png", renderer);

	if (digitSheet == nullptr) {
		logSDLError(std::cout, "SpriteSheet-digit");
		cleanup(smileSheet, renderer, window, digitSheet);
		IMG_Quit();
		SDL_Quit();
	}

	
	SDL_Rect digits[DIGIT_AMOUNT];
	xOffset = 0;//reusing previous offset variable
	for (int i = 0; i < DIGIT_AMOUNT; i++) {
		digits[i].x = (i*DIGIT_WIDTH) + xOffset;
		digits[i].y = 0;
		digits[i].h = DIGIT_HEIGHT;
		digits[i].w = DIGIT_WIDTH;
		xOffset++;
	}

	//create an individual rectangle for each position of the clock and mine counter
	//mine offset
	int mineXOffset = DIGIT_WIDTH / 2;
	int mineYOffset = DIGIT_HEIGHT / 4;
	//for mine counter
	SDL_Rect minesHundreds;
	minesHundreds.x = mineXOffset / windowScale;
	minesHundreds.y = mineYOffset / windowScale;
	minesHundreds.h = DIGIT_HEIGHT;
	minesHundreds.w = DIGIT_WIDTH;

	//set the position of the tens and ones relative to the hundreds to ensure consistency if it changes
	SDL_Rect minesTens;
	minesTens.x = minesHundreds.x + DIGIT_WIDTH;
	minesTens.y = minesHundreds.y;
	minesTens.h = DIGIT_HEIGHT;
	minesTens.w = DIGIT_WIDTH;

	
	SDL_Rect minesOnes;
	minesOnes.x = minesHundreds.x + DIGIT_WIDTH*2;//move it over horizontally by 2 digits
	minesOnes.y = minesHundreds.y;
	minesOnes.h = DIGIT_HEIGHT;
	minesOnes.w = DIGIT_WIDTH;

	//clock offset
	int clockXOffset = (int)(newWinWidth - DIGIT_WIDTH*2.25 );
	//the *2.5 comes from shifting 1 digitwidth over to ensure the entire text appear, then an offset
	
	int clockYOffset = mineYOffset;
	//for clock, working in reverse as it is reflected
	SDL_Rect clockOnes;
	clockOnes.x = (clockXOffset / windowScale)  ;
	clockOnes.y = clockYOffset / windowScale;
	clockOnes.h = DIGIT_HEIGHT;
	clockOnes.w = DIGIT_WIDTH;

	//set the position of the tens and ones relative to the hundreds to ensure consistency if it changes
	SDL_Rect clockTens;
	clockTens.x = clockOnes.x - DIGIT_WIDTH;
	clockTens.y = clockOnes.y;
	clockTens.h = DIGIT_HEIGHT;
	clockTens.w = DIGIT_WIDTH;

	SDL_Rect clockHundreds;
	clockHundreds.x = clockOnes.x - DIGIT_WIDTH*2;
	clockHundreds.y = clockOnes.y;
	clockHundreds.h = DIGIT_HEIGHT;
	clockHundreds.w = DIGIT_WIDTH;


	/*
	** MAIN LOOP
	*/
	//int centreWidth = SCREEN_WIDTH / 2 - CELL_SIZE / 2;
	//int centreHeight = SCREEN_HEIGHT / 2 - CELL_SIZE / 2;
	int useClip = 0;
	int xCoord = 0, yCoord = 0;
	int currentSmile = 0;//changes which smile sprite is displayed based on the players actions
	int highlightedCell[3] = { -1, -1, 0 };//stores the position of the cell that clicked. 
										   //3rd value is whether or not it has been middle clicked, 
										   //which higlights surrounding cells as well
	SDL_Event e;
	bool restart = true;//used to create a minefield intially and whenever the game is restarted
	int minesRemaining = 0;
	Minefield* playfield = nullptr;
	int* digitPos = new int [3];
	unsigned int startTime = 0;
	int timeDiff = 0;
	bool gameOver = false;//tracks whether the game is over, win or lose
	while (!quit){
		//resets the variables to their defualt
		if (restart == true) {
			//delete prievously allocated minefield if it exists
			if (playfield != nullptr) {
				delete playfield;
			}
			//create the minefield with parameters determined by the difficulty chosen
			playfield = new Minefield(difficultyOptions[difficultyChosen][0], 
									  difficultyOptions[difficultyChosen][1], 
									  difficultyOptions[difficultyChosen][2]);
			//ensure that it isn't recreated in the next iteration
			minesRemaining = playfield->getMines();
			restart = false;
			startTime = SDL_GetTicks();
			currentSmile = 0;
			gameOver = false;
			//debug
			//playfield->printRawField();

		}
		
		//e is an SDL_Event variable we've declared before entering the main loop
		while (SDL_PollEvent(&e)) {

			//If user closes the window
			if (e.type == SDL_QUIT) {
				quit = true;
			}
			
			
			if (e.type == SDL_MOUSEBUTTONDOWN){
				
				//if it was a left click
				getCell(e, xCoord, yCoord, windowScale, topBarOffset);
				if (e.button.button == SDL_BUTTON_LEFT) {
					//std::cout << "received left click down" << std::endl;
					//use mouseDetect rect declared early to determine selection
					mouseDetect.x = e.button.x/windowScale;
					mouseDetect.y = e.button.y/windowScale;

					if (SDL_HasIntersection(&mouseDetect, &SmilePos) == SDL_TRUE){
						//if the smile itself gets clicked, highlight 
						currentSmile = 1;

					}
					//use mouseDetect rect declared early to determine selection
					mouseDetect.x = e.button.x/windowScale;
					mouseDetect.y = e.button.y/windowScale;
				
					if (SDL_HasIntersection(&mouseDetect, &SmilePos) == SDL_TRUE){
						//if the smile itself gets clicked, highlight 
						currentSmile = 1;
					
					}
					//only highlight cells if the game is still ongoing
					if (!gameOver) {
						//only highlight the given cell if the cell is not flagged
						if (playfield->isFlagged(xCoord,yCoord) == false) {
							//set the highlighted cell to be the one that was clicked
							highlightedCell[0] = xCoord;
							highlightedCell[1] = yCoord;

						}
						//only change the smile if the highlight cell is valid
						if (playfield->validatePoint(xCoord,yCoord)) {
							currentSmile = 2;
						}

					}
					
				}
				
				//if it was a right-click and the game is ongoing
				if (e.button.button == SDL_BUTTON_RIGHT && !gameOver) {
					//only toggle if this is a valid cell and not revealed 
					if (playfield->validatePoint(xCoord,yCoord)
						&& !playfield->isRevealed(xCoord,yCoord)) {
						//then flag or unflag the given cell
						playfield->toggleFlagged(xCoord, yCoord);
					
						if (playfield->isFlagged(xCoord, yCoord) == true) {
							//if a cell is flagged, decrement the mine counter as it is assumed that a mine is found
							minesRemaining--;
						} else if (playfield->isFlagged(xCoord, yCoord) == false) {
							//and vice versa
							minesRemaining++;
						}

					}
				}

				//if we receive a middleclick, than reveal surrounding cells as well
				if (e.button.button == SDL_BUTTON_MIDDLE && !gameOver) {
					highlightedCell[0] = xCoord;
					highlightedCell[1] = yCoord;
					highlightedCell[2] = 1;//highlight all surronding cells as well
				}



			}
			if (e.type == SDL_MOUSEBUTTONUP) {
				currentSmile = 0;
				//std::cout << "x = " << e.button.x<<", y =" << e.button.y << std::endl;
				//getCell(e, xCoord, yCoord, windowScale, topBarOffset);
				if (e.button.button == SDL_BUTTON_LEFT) {
					//use mouseDetect rect declared early to determine selection
					mouseDetect.x = e.button.x/windowScale;
					mouseDetect.y = e.button.y/windowScale;
					if (SDL_HasIntersection(&mouseDetect, &SmilePos) == SDL_TRUE){
						//if the smile itself was clicked, restart the game
						restart = true;

					}
					//only do so if the game is still ongoing
					if (!gameOver) {
						//unhighlight the cell after the mouse button is released
						highlightedCell[0] = -1;
						highlightedCell[1] = -1;
						if (xCoord == -1 || yCoord == -1){
							//TODO: remember why this is here
							//quit = true;
						}

						//only reveal the point if it is within the minefield
						if (playfield->validatePoint(xCoord,yCoord)){
							//add only if it has not been flagged
							if (playfield->isFlagged(xCoord,yCoord) == false) {
								//reveal the chosen cell
								playfield->revealPoint(xCoord, yCoord);

							}

						}
						
					}
				

				}
				if (e.button.button == SDL_BUTTON_MIDDLE && !gameOver) {
					//we want to reveal the highlighted cell and all cells around it
					playfield->revealSurrounding(xCoord, yCoord);
					highlightedCell[0] = -1;
					highlightedCell[1] = -1;
					highlightedCell[2] = 0;
				}


			}
			//If user presses any key
			if (e.type == SDL_KEYDOWN) {
				break;//break because this is for debugging only
				//check which key it is

				switch (e.key.keysym.sym){
					case SDLK_1:
						
						playfield->revealAll();
						break;

					case SDLK_2:
						useClip = 1;
						break;

					case SDLK_3:
						useClip = 2;
						break;

					case SDLK_4:
						useClip = 3;
						break;

					case SDLK_5:
						useClip = 4;
						break;
					case SDLK_6:
						useClip = 5;
						break;

					case SDLK_LEFT:
						useClip--;
						break;

					case SDLK_RIGHT:
						useClip++;
						break;

					case SDLK_ESCAPE:
						quit = true;
						break;
					default:
						break;
				}
				
			}
			
		}
		//check if the game is lost
		if (playfield->getGameLost() ) {
			playfield->revealAll();
			gameOver = true;
			//only change the smile if the current smile is the default
			if (currentSmile == 0) {
				//as this becomes the default smile when the game is lost
				currentSmile = 4;
			}
			
		}
		//check if the game has been won
		if (playfield->getRemainingCells() == 0){
			gameOver = true;
			//only change the smile if the current smile is the default
			if (currentSmile == 0) {
				//as this becomes the default smile when the game is won
				currentSmile = 3;
			}
		}

		//only count the time if the game is still on going
		if (!gameOver) {
			//counting the seconds elapsed since starting
			timeDiff = (int)(SDL_GetTicks() - startTime);//the time elapsed since the game was started
			timeDiff = timeDiff / 1000; //convert milliseconds to seconds

		}
	
		/*RENDERING THE GAME*/


		SDL_RenderClear(renderer);
		
		//render a background colour
		SDL_SetRenderDrawColor(renderer, 192, 192, 192, 255);

		//get the values of each counter position
		intToDigit(minesRemaining, digitPos);
		
		//render the mine counter
		renderTexture(digitSheet, renderer, minesHundreds, &digits[digitPos[0]]);
		renderTexture(digitSheet, renderer, minesTens    , &digits[digitPos[1]]);
		renderTexture(digitSheet, renderer, minesOnes	 , &digits[digitPos[2]]);

		//get the values for each clock position
		intToDigit(timeDiff, digitPos);
		//render the clock
		renderTexture(digitSheet, renderer, clockHundreds, &digits[digitPos[0]]);
		renderTexture(digitSheet, renderer, clockTens	 , &digits[digitPos[1]]);
		renderTexture(digitSheet, renderer, clockOnes	 , &digits[digitPos[2]]);

		//render the smiley face
		renderTexture(smileSheet, renderer, SmilePos, &smilyButtons[currentSmile]);
		//draw the minefield
		displayField(renderer, playfield, topBarOffset/windowScale, highlightedCell);

		SDL_RenderPresent(renderer);
		
	}
	
	//clean up all allocated memory
	delete playfield;
	
	delete[] digitPos;
	cleanup(spriteSheet, window, renderer,smileSheet, digitSheet);
	IMG_Quit();
	SDL_Quit();
	return 0;
}