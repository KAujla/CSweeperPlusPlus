#include "mineFieldIO.h"
#include "../include/SDL_helper.h"
#include <res_path.h>
#include <cleanup.h>
#include <iostream>

const int CELL_SIZE = 16;//each cell sprite is 16p by 16p

void getCell(SDL_Event mouseClick, int & x, int & y, int scale, int offset){
	//convert the "raw" x and y pixel values into coordinates for the mine field

	x = (int)floor(mouseClick.button.x / (float)(CELL_SIZE * scale));//need to explicitly convert demoninator to float to ensure no truncation
	y = (int) floor( ( mouseClick.button.y - offset) / (float)(CELL_SIZE * scale) );
	
}

void displayField(SDL_Renderer * ren, Minefield* playField, int offset, int highlightedCell[3]){

	//an enum of the different kinds of cells
	//used since the value of the cell may not be mappable to it's position in the array
	enum cellType{
		DEFAULT = 0,
		REVEALED,
		FLAG,
		QUESTION,
		QUESTIONREVEALED,
		MINE,
		MINERED,
		MINEX,
		ONE		= 8,
		TWO		= 9,
		THREE	= 10,
		FOUR	= 11,
		FIVE	= 12,
		SIX		= 13,
		SEVEN	= 14,
		EIGHT	= 15

	};
	const int cellSpriteNum = 16;//16 different cell sprites
	const int iW = CELL_SIZE ;
	const int iH = CELL_SIZE ;
	//int width, height;
	/*SDL_GetRendererOutputSize(ren, &width, &height);
	int centreWidth = width/ 2 - iW/ 2;
	int centreHeight = height/ 2 - iH/ 2;
	*/
	//figure out what the offset for the topbar is

	
	const std::string cellResPath = getResourcePath("");
	SDL_Texture* cellSheet = loadTexture(cellResPath + "CellSprites.png", ren);

	//ensure that they load

	if (cellSheet == nullptr) {
		cleanup(cellSheet);
		return;
	}

	SDL_Rect cellSprites[cellSpriteNum];//16 images total

	int xOffset = 0;//offset of the cell sprites x coord as there is a 1 pixel gap between sprites
					//there is 8 sprites per row, so once it reaches the second row, it will add a single pixel,
					//which is the desired offset
	int yOffset = 0;

	for (int i = 0; i < cellSpriteNum; i++) {
		cellSprites[i].x = (i % 8 * iW) + (xOffset % 8);
		cellSprites[i].y = (i / 8 * iH) + (yOffset / 8);
		cellSprites[i].w = iW;
		cellSprites[i].h = iH;
		xOffset++;
		yOffset++;
	}
	//get the height and width of the minefield
	int numRows;
	int numCols;
	playField->getSize(numCols, numRows);
	
	
	int useClip = 0;
	int x, y;
	for (int i = 0; i < numCols; i++){
		for (int j = 0; j < numRows; j++){

			x = j * iW;
			y = (i * iH) + offset;
			if (playField->isRevealed(j,i)){
				int value = playField->getValue(j, i);
				switch (value){
					case -2:
						//used a special case, if the chosen cell contains a mine
						useClip = MINERED;
						break;
					case -1:
						useClip = MINE;
						break;
					case 0:
						useClip = REVEALED;
						break;
					case 1:
						useClip = ONE;
						break;
					case 2:
						useClip = TWO;
						break;
					case 3:
						useClip = THREE;
						break;
					case 4:
						useClip = FOUR;

						break;
					case 5:
						useClip = FIVE;
						break;
					case 6:
						useClip = SIX;
						break;
					case 7:
						useClip = SEVEN;
						break;
					case 8:
						useClip = EIGHT;
						break;

					default:
						break;
				}
			}
			else {
				
				useClip = DEFAULT;
				//if the cell is flagged, then set it to flag texture
				if (playField->isFlagged(j,i)) {
					useClip = FLAG;
				}
			}

			//in the case of a game over, reveal incorrect flags
			if (playField->getGameLost()== true) {
				//if the current cell is flagged and isn't a mine
				if (playField->isFlagged(j,i) == true && playField->getValue(j,i) != -1) {
					//then the player flagged a cell incorrect and should be shown as such
					useClip = MINEX;
				}
			}
			

			renderTexture(cellSheet, ren, x, y, &cellSprites[useClip]);
		}
	}
	//special case for the highlighted cell
	//if the highlighted cell is within the minefield and not revealed already, set it to the blank sprite, aka "revealed"
	if (playField->validatePoint(highlightedCell[0],highlightedCell[1]) && 
		playField->isRevealed(highlightedCell[0],highlightedCell[1]) == false) {
		x = highlightedCell[0] * iW;
		y = highlightedCell[1] * iH + offset;
		useClip = REVEALED;
		renderTexture(cellSheet, ren, x, y, &cellSprites[useClip]);
		
	}
	
	//if it was highlighted with a middle-click and is valid, highlight all surrounding cells as well
	if (highlightedCell[2] == 1  &&
		playField->validatePoint(highlightedCell[0],highlightedCell[1])) {
		useClip = REVEALED;
		int adjX, adjY = 0;//placeholder for the x-y position of the surronding cells

		for (int i = -1; i < 2; i++) {
			for (int j = -1; j < 2; j++) {
				adjX = highlightedCell[0] + i;
				adjY = highlightedCell[1] + j;
				//if the cell has not been revealed already and is within the board
				if (playField->isRevealed(adjX, adjY) == false 
					&& playField->validatePoint(adjX, adjY) == true ) {
					x = adjX * iW;
					y = adjY * iH + offset;
					renderTexture(cellSheet, ren, x, y, &cellSprites[useClip]);
				}
			}
		}
	}
	cleanup(cellSheet);
}
