Yaroslav Yanin
yyanin@ucsc.edu



I plan to have a goly defend the goals from the incoming balls.

It will look something like this:

|
   \   <--- This is the goly              Balls--->*

|


The player recieves 100 points for every goal he saves. He also has 3 lives. 
The score expands continuously untill you run out of lives. When the player
runs out of lives, the end screen is shown where the player sees his score. 
At any point the game can be restarted with B1. 

B4 - go up
B3 - go down


The ball might also change direction in mid air. 


Main:
	Sets up all the variables to the correct values

	starts while(1) loop for the project to run.

			- Ball State Mchine runs all the time unless at an end screen 
			- Goalie State machine when a button event is pressed
	
Goalie state machine:
	Waiting for input:
		if a button is pressed: change case to move goalie
	Move goalie:	
		if button 4 is pressed: move ip
		else if B3 is pressed move down.
		
		change back to waiting for input

Ball state machine:
	Idle:
		Generate a random number
		Check for the end of the game
			if so, go to the end screen
	Kick:
		If B1 is pressed, restart the game
		If the ball is at the goals: Check for the goalie position. 
				-If the position of the goalie matches the position of the ball = case Blocked
				- else: case failed
	Failed:
		Subtract a life
		Check for amount of lives left
				if 0: set the game end counter to 1. Idle will end the game
				else: case : idle



OledUpdate:
		Prints everything depending on the counter
					



	
