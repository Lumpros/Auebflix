#include "app.h"
#include "Film.h"

#include <stdexcept>

int main(void)
{
	try 
	{
		Film::loadFilms("assets\\films.txt");

		AppWindow app(Size(1370, 720), "Auebflix");
		app.doMessageLoop();

		Film::unloadFilms();
	}

	catch (std::exception& e) 
	{
		puts(e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}