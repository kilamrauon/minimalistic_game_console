# minimalistic_game_console

A minimilastic game console with an attiny85, a 5 button ad keyboard, a 8x7 segment red led and a buzzer

I made it to simulate a game I had when I was young : battlestar galactica - space alert , from mattel

before taking the time to make a schematics :
for the 8*7 led segment :
DIN -> PB1
CS -> PB3
CLK -> PB4

Keyboard out -> A1

Buzzer  + -> PB0

how to play:
the game starts automatically (the high score is displayed for 2 seconds)

push the button 1 to 4 to move the missile
push button 5 to shoot

you have 5 lives (==) at the botton and the missile is the '|' that moves.

aliens are going from top to bottom and each time an alien reaches bottom you lose a life.

Up to 3 aliens can  go down.
