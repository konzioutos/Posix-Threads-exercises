Konstantinos Zioutos AM3946 csd3946@csd.uoc.gr

In exercise 1 each car is a thread, I have two flags and two mutexes.The logic I followed is as follows:
the sem_car flag starts with a value of 2 to allow 2 cars before it is locked.
When messing with shared memory (current_road_direction and road[.]) I use mutex.
If a car is already in then if the next car that wants to get in has a different direction
than the one already in, then it is blocked by the sem_direction flag.

In exercise 2(ferry_boat) we have one thread for each car and one for the ferry. I follow the approach 
consumer-producer.The consumer flag starts with a value of 20 to allow 20 cars before it is locked.
I use a mutex for the shared memory. As for which cars get on and off the ship.
I use a state variable in the struct of the cars I'm manipulating to get the output.

In both exercises I check for the correct input and initialize the road and the ship respectively.
