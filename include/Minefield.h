#pragma once
#include "Cell.h"
class Minefield{
	public:
		/*
		* creates a minefield with the specified paramters
		* @param width  = how many columns wide the field is
		* @param height = how many rows long the field is
		* @param mines  = the number of mines in the field
		*/
		Minefield(int width, int height, int mines);
		


		~Minefield();
		
		/*
		 * @return the remaining number of unrevealed, non-mine, cells
		 */
		int getRemainingCells();
		
		/*
		 * returns the number of mines in the field
		*/
		int getMines();
		
		/*
		 * @return the value of 'isGameLost'
		 */
		bool getGameLost();
		

		/*
		 * gives the size of the field and stores them in the parameters
		 * @param width = address in which to place the width of the field
		 * @param height = address in which to palce the height of the field
		 */
		void getSize(int& width, int& height);
		
		/* 
		 * confirms whether a given point is within the minefield
		 * @param x = the x coordinate of the point
		 * @param y = the y coordinate of the point
		 * @return = true if it is within, false otherwise
		 */
		bool validatePoint(int x, int y);
		

		/*
		 * Reveals the cell at the given coordinate
		 * and sets 'isGameLost' if the given cell is a mine
		 * assumes that the given point is valid, i.e within the field
		 * @param x = the x value of the cell
		 * @param y = the y value of the cell
		 * @return the value of 'isGameLost' after the cell is revealed
		 */
		bool revealPoint(int x, int y);

		/*
		 * reveals the cells surrounding the given cell.
		 * Only does so if the the surrounding cells contains
		 * equal number of flags to the centre cell's value
		 * and the centre cell has already been reveal.
		 * E.g., if the cell given by (x,y) has value 1,
		 * then the surrounding cells will only be revealed if
		 * they contain exactly 1 flag.
		 * @param x = the x value of the cell
		 * @param y = the y value of the cell
		 * @return the value of 'isGameLost' after all cells are revealed
		 */
		bool revealSurrounding(int x, int y);


		/*
		 * Determines whether a cell is revealed
		 * @param x = the x value of the cell
		 * @param y = the y value of the cell
		 * @return: true if is reveal, false otherwise. Invalid cells will return false
		 */
		bool isRevealed(int x, int y);

		/*
		 * gets the value of the specified cell
		 * @param x = the x value of the cell
		 * @param y = the y value of the cell
		 *
		 */
		int getValue(int x, int y);

		/*
		 * performs the neccessary actions to end a game on the minefields end if called.
		 * requires 'isGameLost' to be 'true'
		 */
		void revealAll();

		/*
		 * Determine whether the specified cell is flagged or not
		 * @param x = the x value of the cell
		 * @param y = the y value of the cell
		 * @return: true if it is flagged, otherwise false
		 */
		bool isFlagged(int x, int y);

		/*
		 * Toggles the flag status of the given cell.
		 * if the cell is not flagged, then flag it,
		 * if it is flagged, then unflag it
		 * @param x = the x value of the cell
		 * @param y = the y value of the cell
		 */
		void toggleFlagged(int x, int y);
		/*FOR DEBUGGING 
		 * prints the values of all the cells and whether they have been revealed
		 */

		void printRawField();
	private:

		//the number of rows in the minefield
		int height;

		//the number of columns in the minefield
		int width;

		//the total number of mines in the field
		int NumOfMines;

		//the number of unrevealed cells that are not mines
		int RemainingCells;
		
		//whether the game has been lost due to a revealed mine
		bool isGameLost;

		//the "play" field. A 2d array that contains cells, 
		//each cell tracks it's value and whether it has been revealed among other things 
		Cell** field;
		
		/*
		 * reveals all adjacent zeroes to the point specified
		 * then recurses through all adjecent cells to reveal and potential zeroes in adjecent cells
		 * @param x = the initial x from which the recursion begins
		 * @param y = the initial y from which the recursion begins
		 * returns when no unrevealed zero-cells are found adjacent to the current cell
		 */
		void RevealZeroes(int x, int y);

};

