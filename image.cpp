#include "image.hpp"

using namespace std;

void image::setWidth(double w)
{
	width = w;
}
void image::setHeight(double h)
{
	height = h;
}
void image::setBgcolor(vector<double> bg)
{
	bgcolor = bg;
}

double image::getWidth()
{
	return width;
}

double image::getHeight()
{
	return height;
}

vector<double> image::getBgcolor()
{
	return bgcolor;
}