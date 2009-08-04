# Makefile for this project, 
# Maintained by Tangel
#
#
# This is of cource not a good makefile, cause that the prerequisites are not include, for example, if vector.h is change, most of the files in these project should be 
# recompiled, but not. if a *.cpp file include a *.h file, we should add the .h file into the prerequisites
#

billiard: ball.o collision.o gameloop.o gamestate.o object.o render.o table.o vector.o
	g++ -o billiard ball.o collision.o gameloop.o gamestate.o object.o render.o table.o vector.o
ball.o: ball.cpp
	g++ -c ball.cpp
collision.o: collision.cpp
	g++ -c collision.cpp
gameloop.o: gameloop.cpp
	g++ -c gameloop.cpp
gamestate.o: gamestate.cpp
	g++ -c gamestate.cpp
object.o: object.cpp
	g++ -c object.cpp
render.o: render.cpp
	g++ -c render.cpp
table.o: table.cpp
	g++ -c table.cpp
vector.o: vector.cpp
	g++ -c vector.cpp
clean:
	del ball.o collision.o gameloop.o gamestate.o object.o render.o table.o vector.o 
