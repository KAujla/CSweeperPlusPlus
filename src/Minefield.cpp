#include "Minefield.h"
#include <time.h>
#include <cstdlib>
#include <iostream>
//
//int height;
//int width;
//int NumOfMines;
//int RemainingCells;
//bool isGameLost;
//Cell** field;


/*
* creates a minefield with the specified paramters
* @param width  = how many columns wide the field is
* @param height = how many rows long the field is
* @param mines  = the number of mines in the field
*/
Minefield::Minefield(int width, int height, int mines){
	this->width = width;
	this->height = height;
	this->NumOfMines = mines;
	
	isGameLost = false;

	//remaining cells is the number of unrevealed cells that are not mines
	RemainingCells = (this->width*this->height) - this->NumOfMines;


	//allocate the memory for the field
	field = new Cell*[this->height];
	for (int i = 0; i < this->height; i++ ) {
		field[i] = new Cell[this->width];
	}

	
	unsigned int seed = (unsigned) time(NULL);
	srand(seed);
	

	//randomly scatter mines throughout the field
	int randCol = 0, randRow = 0;
	for (int i = 0; i < this->NumOfMines; i++){
		randCol = rand() % this->width;
		randRow = rand() % this->height;

		//only set it as a mine if the cell does not currently contain a mine
		if (field[randRow][randCol].getValue() != -1) {
			// '-1' is interpeted as a mine 
			this->field[randRow][randCol].setValue(-1);

		} else {
			//the mine is not placed therefore decrement i so it can try again
			i--;
		}
	}


	//TODO: potential improvement my keeping track of mine positions in an array

	//now calculate the values of each cell by counting adjacent mines
	for (int i = 0; i < this->height; i++){
		for (int j= 0; j < this->width; j++){
			int surroundingMines = 0;
			
			//if the current cell is not a mine
			if ( !(this->field[i][j].isMine()) ){

				//then look at the adjacent cells to see if they contain mines
				for (int x = -1; x < 2; x++) {
					for (int y = -1; y < 2; y++) {


						//check if the cell is out of bounds in any direction
						if (i + x >= 0 && j + y >= 0
							&& j + y <= this->width - 1
							&& i + x <= this->height - 1) {

							//check if it is a mine
							if (this->field[i + x][j + y].getValue() == -1) {

								surroundingMines++;
							}
						}

					}

				}
				//set the value of the cell to the number of adjacent mines
				this->field[i][j].setValue(surroundingMines);
			}

		}
	}
}


Minefield::~Minefield(){
	for (int i = 0; i < this->height; i++){
		delete[] this->field[i];
		
	}
	delete[] this->field;
	
	this->NumOfMines = 0;
	this->height = 0;
	this->width = 0;
	this->NumOfMines = 0;
	this->RemainingCells = 0;
	
}

/*
* @return the remaining number of unrevealed, non-mine, cells
*/
int Minefield::getRemainingCells(){
	return this->RemainingCells;
}

int Minefield::getMines() {
	return this->NumOfMines;
}

/*
* @return the value of 'isGameLost'
*/
bool Minefield::getGameLost(){
	return this->isGameLost;
}

void Minefield::getSize(int& width, int& height){
	width = this->width;
	height = this->height;
}

bool Minefield::validatePoint(int x, int y){
	
	return ((0 <= x && x < this->width) && 
		(0 <= y && y < this->height));
	
	
}

bool Minefield::revealPoint(int x, int y){
	//assuming that the point is valid
	
	//check if the cell is not reveal and not flagged
	if ( !( this->field[y][x].getRevealed()) 
		 && !(this->isFlagged(x, y)) ){
		
	
		//if the cell contains a zero
		if (this->field[y][x].getValue() == 0){
			//then run revealzeros to unhide any adjacent 0-cells
			this->RevealZeroes(x, y);
		
			//only reveal if not zero and has not been flagged, as revealZero reveals this cell as well 
		} else if (!(this->field[y][x].getFlagged())) {

			//then reveal the cell 
			this->field[y][x].setRevealed(true);

			//and decrement the number of hidden cells
			this->RemainingCells--;
		}
	
		//if this cell containsa mine
		if (this->field[y][x].isMine()){
			//then the player has lost
			//if the game is lost after choosing this cell, indicate that is should be red
			this->field[y][x].setValue(-2);//use -2 to indicate a special mine
			this->isGameLost = true;
		}

	}//otherwise just return
	return this->getGameLost();
}

bool Minefield::revealSurrounding(int x, int y) {
	//first ensure that the centre cell is within the field and that it has been revealed already
	if (this->validatePoint(x,y) == true && 
		this->isRevealed(x, y) == true) {
		//then check if the surrounding cells contain the correct number of flags
		int surroundingFlags = 0;
		for (int xOff = -1; xOff < 2; xOff++) {
			for (int yOff = -1; yOff < 2; yOff++) {
				if (this->isFlagged(x + xOff, y + yOff) == true) {
					surroundingFlags++;
				}
			}
		}

		if (surroundingFlags == this->getValue(x, y)){
			// we want to reveal all the surronding cells
			for (int xOff = -1; xOff < 2; xOff++) {
				for (int yOff = -1; yOff < 2; yOff++) {
					//ensure that it is within the board
					if (this->validatePoint(x + xOff, y + yOff)) {
						this->revealPoint(x + xOff, y + yOff);

					}
					
				}
			}
		}
	}

	return this->getGameLost();
}

bool Minefield::isRevealed(int x, int y){
	if (validatePoint(x,y)){
		return field[y][x].getRevealed();
	}
	else {
		return false;

	}
}

int Minefield::getValue(int x, int y){
	if (validatePoint(x, y)) {
		return field[y][x].getValue();
	}
	else {
		return -2;

	}
}

void Minefield::revealAll(){
	//if the game has been lost, reveal all mines
	if (this->getGameLost()){
		
		for (int i = 0; i < this->height; i++){
			for (int j = 0; j < this->width; j++){
				//only reveal the mines
				if (this->field[i][j].isMine()) {
					this->field[i][j].setRevealed(true);

				}
			}
		}

	}
}

bool Minefield::isFlagged(int x, int y) {
	if (validatePoint(x,y)){
		return field[y][x].getFlagged();
	}
	else {
		return false;

	}
}

void Minefield::toggleFlagged(int x, int y) {
	if (validatePoint(x,y)){
		//set flagged to the opposite of what it currently is
		field[y][x].setFlagged( !( field[y][x].getFlagged() ) );
	}
}

void Minefield::printRawField(){
	
	/*printf("height is     %d\n", inputField->height);
	printf("width is      %d\n", inputField->width);
	*///printf("numOfMines is %d\n", inputField->numOfMines);
	std::cout << "height is \t"<<height << std::endl;
	std::cout << "width is \t" <<width << std::endl;
	std::cout << "number of mines is "<<NumOfMines << std::endl;

	if (getGameLost()) {
		std::cout << "gameLost is true\n" << std::endl;
	}
	else {
		std::cout << "gameLost is true\n" << std::endl;
	}
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {

			printf("(%d,%d) = %d, checked =  %d \n", i, j,
				field[i][j].getValue(), field[i][j].getRevealed() );
			/* code */
		}
	}
	//print the field in a grid format for easier visualization
	std::cout<<"field\n"<<std::endl;
	std::cout << "   ";
	for (int i = 0; i < this->width; i++) {
		std::cout << i << " ";
		if (i <10) {
			std::cout << " ";
		}
	}
	std::cout << "\n";
	for (int i = 0; i < this->width; i++) {
		std::cout << "-- ";
	}
	std::cout << "" << std::endl;

	//board
	for (int i = 0; i < this->height; i++) {
		if (i < 10) {
			std::cout << i << " |";
		} else {
			std::cout << i << "|";
		}
		for (int j = 0; j < this->width; j++) {
			if (field[i][j].getValue() == -1) {
				std::cout << "*  ";
			} else {
				std::cout << this->field[i][j].getValue() << "  ";
			}

		}
		std::cout << "" << std::endl;
	}
	

}

void Minefield::RevealZeroes(int x, int y){
	int cellValue = 0;

	if (this->validatePoint(x,y) ){
		
		cellValue = this->field[y][x].getValue();

		//if the cell has not been revealed yet and isn't flagged
		if (!(this->field[y][x].getRevealed()) && !(this->field[y][x].getFlagged()) ) {
			//reveal it
			this->field[y][x].setRevealed(true);
			this->RemainingCells--;

		}else {//if the cell has already been revealed return as there is nothing to be done
			return;
		}

		if (cellValue == 0){
			//if the cell is a zero, than run on all adjacent cells to reveal any further 0-cells

			for (int xOff = -1; xOff < 2; xOff++){
				for (int yOff = -1; yOff < 2; yOff++){
					this->RevealZeroes(x + xOff, y + yOff);
				}
			}
		}
	}
}

