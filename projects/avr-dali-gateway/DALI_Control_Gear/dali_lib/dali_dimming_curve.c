// @brief This array contains logarithmic dimming curve values

code U8 DIMMING_CURVE[254]={
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,
1,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,
2,2,2,2,3,3,3,3,
3,3,3,3,3,3,3,3,
4,4,4,4,4,4,4,4,
4,4,5,5,5,5,5,5,
5,6,6,6,6,6,6,7,
7,7,7,7,8,8,8,8,
8,9,9,9,9,10,10,10,
10,11,11,11,12,12,12,13,
13,13,14,14,15,15,15,16,
16,17,17,18,18,19,19,20,
20,21,21,22,22,23,24,24,
25,26,26,27,28,29,29,30,
31,32,33,34,35,36,37,38,
39,40,41,42,43,44,46,47,
48,50,51,52,54,55,57,58,
60,62,63,65,67,69,71,73,
75,77,79,81,83,86,88,90,
93,95,98,101,104,106,109,112,
116,119,122,125,129,132,136,140,
144,148,152,156,160,165,169,174,
179,184,189,194,199,205,211,216,
222,229,235,241,248,255
};

//@brief This array contains fade times, in milliseconds

code U16 FADE_TIME[16]={0,707,1000,1414,2000,2828,4000,5657,8000,11314,16000,22627,32000,45255,64000,45255}; 
//last value (FADE_TIME[15]) is actually 90.510s. 
//The fade period will be multiplied by 2 in software for this value.

//@brief This array contains the fade rate, /!\ in steps/200ms /!\
  
code U8 FADE_RATE[16]={0,72,51,36,25,18,13,9,6,4,3,2,2,1,1,0};


