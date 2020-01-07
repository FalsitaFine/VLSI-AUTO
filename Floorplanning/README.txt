Usage:
>>> make
>>> ./floorplanning inputfilename -option
-option can be -a, -c or -w (I also provide -ca and -cw for different cost functions, see the report).

For easy testing, I provided the 'autotest.sh' in the folder, all tests can be done by >>> ./autotest.sh


Then a output file (and corresponding intermediate results .csv) will be generated.
Note: my implementation appends the new result at the end of the existing result file(if exists), so if you need to test it for more than 1 time, please delete the old result files at first. I used dynamic MOVES_PER_ITERATION so in the intermediate results the sum of AcceptedMove and RejectedMove may changes over time.

A set of sample outputs and intermediate results are provided in the folder sample_result_intermediate.

Note: the coordinates of blocks(X,Y) shown in the output file is the coordinates of bottom-left corners of the blocks. And therefore the circuit X,Y are not equals to largest block X,Y since it is calculated from the bottom-left of the whole area to the top-right of the area., instead, it is equals to max(block.X + block.width), max(block.Y + block.height).


The following parts are also in the report.pdf.

-Special Strategies-
To reduce the runtime cost of the algorithm, the value NUM\_MOVES\_PER\_TEMP\_STEP (How many random moves within a certain iteration) is dynamically chosen in my implementation. It begin with a low value N at the beginning (picked as 6 in the implementation) and increases as 1.01N after each iteration, so that N will be increased to about 100 in the last iterations. This implementation can greatly reduce the runtime while keep a similar annealing result compares to set and fix N equals to 100 at the beginning. Since with the initially high temperature, most accepted moves are randomly and do not have a certain direction, this implementation is designed to reduce the non-directional moves and increase the directional (towards the optimal solution) moves.

Some special data structures are used in the implementation to obtain a better runtime. For each sequence-pair, two different kinds of vectors are generated, one is the original sequence (e.g. {1,4,2,3}), while the other one suggests the position of each block in the sequence(e.g. for {1,4,2,3}, the position vector is {0,2,3,1}). By generating these position vectors, the program can quicker decide if a block is "above/below/left/right" of another block. 


-Parameters Selection-
The initial temperature and cooling rate of the algorithm is chosen as same as the example given by the lecture slides: initial temperature is 40000, cooling rate is 95%. To obtain better annealing results, I chose freezing temperature as 0.01(1/10 of the example value) to make the annealing process longer and more complete.

The K value used in Boltzmann probability function is chosen based on the certain floorplanning file. The program runs N times random moves on the initial floorplan and records the average costs of these moves(N is proportional to the model number of the file). Then the K is computed to make 95\% of the moves can be accept with the initial temperature. If we want about 100\% of the moves can be accept at the beginning, K value should be closed to infinity and make the annealing too slow. The value 95\% is inspired by the cooling rate and is tested to be a good choice.

The cost function with option -c is chosen based on experiments. I tested propto = 0.3, propto = 0.5 and propto = 0.7, and recorded the results(I kept these options in the code and you can still test propto = 0.3 with option -cw, propto = 0.7 with option -ca). And found propto = 0.5 is a relatively good one, see the report.




Thanks for reading!
