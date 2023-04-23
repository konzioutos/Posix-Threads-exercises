all:
	gcc -pthread -o road narrow_road.c
	gcc -pthread -o ferry ferry_boat.c

clean:
	rm road
	rm ferry
