#pragma once
class Cell {
public:

	Cell();
	Cell(int value);
	~Cell();
	bool isMine();
	int getValue();
	void setValue(int newVal);
	bool getRevealed();
	void setRevealed(bool newValue);
	bool getFlagged();
	void setFlagged(bool newValue);
private:
	//the number of mines around the cell. 
	//Can be 0 to 8. a value of -1 implies that it is a mine
	int value;

	//whether the cell has been revealed
	bool revealed;

	//whether this cell has been flagged, i.e right-clicked
	bool flagged;
};

