#pragma once
#include <iostream>
#include <string.h>
#include <SDL.h>
#include <SDL_image.h>
#include "Minefield.h"

/*
 * gets the x and y value of a cell in terms of minefield indices
 * @param x = the storage variable of the x coordinate
 * @param y = the storage variable of the y coordinate
 */
void getCell(SDL_Event mouseClick ,int& x, int& y, int scale, int offset);


/*
 * renders the minefield to the given renderer
 */
void displayField(SDL_Renderer* ren, Minefield* playField, int offset, int highlightedCell[3] );