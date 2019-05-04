#include "Cell.h"



Cell::Cell(){
	this->value = 0;
	this->revealed = false;
	this->flagged = false;
}

Cell::Cell(int value){
	this->value = value;
	this->revealed = false;
	this->flagged = false;
}


Cell::~Cell(){
	this->revealed = false;
	this->value = 0;
	this->flagged = false;
}

bool Cell::isMine(){

	if (this->value == -1){
		return true;
	}
	else {
		return false;
	}
	
}

int Cell::getValue()
{
	return this->value;
}

void Cell::setValue(int newVal){
	this->value = newVal;
}

bool Cell::getRevealed()
{
	return this->revealed;
}

void Cell::setRevealed(bool newValue) {
	//can only reveal a cell if it has not been flagged
	if (this->flagged == false) {
		this->revealed = newValue;
	}
}

bool Cell::getFlagged() {
	return this->flagged;
}

void Cell::setFlagged(bool newValue) {
	this->flagged = newValue;
}

