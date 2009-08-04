# Makefile for this project, 
# Maintained by Tangel
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
