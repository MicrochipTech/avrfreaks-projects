10 rem ****************************
20 rem *** Play the HI/LOW game ***
30 rem ****************************
40 n = rnd(100):c =0
50 input "Guess a number?", g
60 c = c+1
70 if g=n then 110
80 if g>n then print "lower"
90 if g<n then print "higher"
100 goto 50
110 print "You guessed it in", c, " tries!"
