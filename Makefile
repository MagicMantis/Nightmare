CXX = g++

# Warnings frequently signal eventual errors:
CXXFLAGS=`sdl2-config --cflags` -g -W -Wall -std=c++11 -Weffc++ -Wextra -pedantic -O0 -I `sdl2-config --prefix`/include/

LDFLAGS = `sdl2-config --libs` -lm -lexpat -lSDL2_ttf -lSDL2_image -lSDL2_mixer

OBJS = \
  renderContext.o \
	ioMod.o \
	parseXML.o \
	gamedata.o \
	viewport.o \
	sound.o \
	HUD.o \
	world.o \
	spriteSheet.o \
	frame.o \
	frameFactory.o \
	frameGenerator.o \
	objectManager.o \
	collider.o \
	sprite.o \
	multisprite.o \
	twoWayMultiSprite.o \
	explodingSprite.o \
	chunk.o \
	vector2f.o \
	sludge.o \
	pool.o \
	smog.o \
	player.o \
	shield.o \
	rain.o \
	clock.o \
	engine.o \
	main.o
EXEC = run

%.o: %.cpp %.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LDFLAGS)

clean:
	rm -rf $(OBJS)
	rm -rf $(EXEC)
	rm -rf frames/*
