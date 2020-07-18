void add_fp(long *, signed char *, char);
void fpn_multiply(long *, signed char *);
void fpn_divide(long *, signed char*);
long fpn_reciprocal(long, signed char, signed char*);



/************Add & subtract floating point numbers********************/
void add_fp(long *RHS_of_BP, signed char *expnt, char op){
char mode;

if (expnt[0] == expnt[1])mode = 'A';
if (expnt[0] > expnt[1])mode = 'B';
if (expnt[1] > expnt[0])mode = 'C';

switch (mode){
case 'A':
expnt[2] = expnt[1]; break;

case 'B':
while ((expnt[0] > expnt[1]) &&  (RHS_of_BP[1])){RHS_of_BP[1] = RHS_of_BP[1]/10; expnt[1]++;}
expnt[2] = expnt[0]; break;

case 'C':
while ((expnt[1] > expnt[0]) && (RHS_of_BP[0])) {RHS_of_BP[0] = RHS_of_BP[0]/10; expnt[0]++;}
expnt[2] = expnt[1]; break;}

if(op == '+'){
if(RHS_of_BP[0] == RHS_of_BP[1]*(-1)){RHS_of_BP[2]=0; return;}
else {	RHS_of_BP[0] /= 10; RHS_of_BP[1] /= 10; expnt[2] = expnt[2] + 1;
		RHS_of_BP[2] = (RHS_of_BP[0]) + (RHS_of_BP[1]);}}

if(op == '-'){
if(RHS_of_BP[0] == RHS_of_BP[1]){RHS_of_BP[2]=0; return;}
else {	RHS_of_BP[0] /= 10; RHS_of_BP[1] /= 10; expnt[2] = expnt[2] + 1;
		RHS_of_BP[2] = (RHS_of_BP[0]) - (RHS_of_BP[1]);}}


while((RHS_of_BP[2] < 0x7FFFFFFF/10) && (RHS_of_BP[2] >= -0x7FFFFFFF/10))
{RHS_of_BP[2] *= 10; expnt[2]--;}

if(expnt[2] <= (-5)){RHS_of_BP[2]=0;expnt[2]=0;}

}



/************Multiply 2 floating point numbers********************/
void fpn_multiply(long number[], signed char expnt[]){
signed char expnt_reciprocal;

number[1] = fpn_reciprocal(number[1],expnt[1],&expnt_reciprocal);
expnt[1] = expnt_reciprocal;
fpn_divide(number, expnt);}



/************Divide two floating point numbers********************/
void fpn_divide(long num[], signed char  expnt[]){
long Ans_LHS;

Ans_LHS = num[0]/num[1]; expnt[2] = expnt[0] - expnt[1];
if(Ans_LHS != 0)
{num[2] = Fraction_to_Binary_Signed(((num[0]/10)%num[1]), num[1]);
expnt[2]++;}
else num[2] = Fraction_to_Binary_Signed((num[0]%num[1]), num[1]);}



/************Get the reciprocal of a floating point number********************/
long fpn_reciprocal(long num, signed char num_expnt, signed char *res_expnt){
signed char  local_expnt[3];
long local_num[3]; 

local_num[0] = 0x0CCCCCCC;		//0.1
local_expnt[0] = 1;				//10
local_num[1] = num;
local_expnt[1] = num_expnt;
fpn_divide(local_num, local_expnt);
*res_expnt = local_expnt[2];
return local_num[2];}


