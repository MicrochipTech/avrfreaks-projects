.include "bits.inc"
.ESEG

; Numbers

n0:
	.db __oooo__
	.db _o____o_
	.db _o____o_
	.db _o____o_
	.db _o____o_
	.db _o____o_
	.db _o____o_
	.db __oooo__
n1:
	.db ____o___
	.db ___oo___
	.db ____o___
	.db ____o___
	.db ____o___
	.db ____o___
	.db ____o___
	.db ___ooo__
n2:
	.db __oooo__
	.db _o____o_
	.db ______o_
	.db _____o__
	.db ____o___
	.db __oo____
	.db _o______
	.db _oooooo_
n3:
	.db __oooo__
	.db _o____o_
	.db ______o_
	.db ____oo__
	.db ______o_
	.db ______o_
	.db _o____o_
	.db __oooo__
n4:
	.db _____o__
	.db ____oo__
	.db ___o_o__
	.db __o__o__
	.db _oooooo_
	.db _____o__
	.db _____o__
	.db ____ooo_
n5:
	.db _oooooo_
	.db _o______
	.db _o______
	.db _ooooo__
	.db ______o_
	.db ______o_
	.db _o____o_
	.db __oooo__
n6:
	.db ___oo___
	.db __o_____
	.db _o______
	.db _o______
	.db _ooooo__
	.db _o____o_
	.db _o____o_
	.db __oooo__
n7:
	.db _oooooo_
	.db _o____o_
	.db _____o__
	.db _____o__
	.db ____o___
	.db ____o___
	.db ___o____
	.db ___o____
n8:
	.db __oooo__
	.db _o____o_
	.db _o____o_
	.db __oooo__
	.db _o____o_
	.db _o____o_
	.db _o____o_
	.db __oooo__
n9:
	.db __oooo__
	.db _o____o_
	.db _o____o_
	.db __ooooo_
	.db ______o_
	.db ______o_
	.db _____o__
	.db ___oo___

; Upper case Characters

cA:
	.db ___oo___
	.db __o__o__
	.db __o__o__
	.db _o____o_
	.db _oooooo_
	.db _o____o_
	.db _o____o_
	.db _o____o_
cB:
	.db _ooooo__
	.db _o____o_
	.db _o____o_
	.db _ooooo__
	.db _o____o_
	.db _o____o_
	.db _o____o_
	.db _ooooo__
cC:
	.db __oooo__
	.db _o____o_
	.db _o______
	.db _o______
	.db _o______
	.db _o______
	.db _o____o_
	.db __oooo__
cD:
	.db _ooooo__
	.db _o____o_
	.db _o____o_
	.db _o____o_
	.db _o____o_
	.db _o____o_
	.db _o____o_
	.db _ooooo__
cE:
	.db _oooooo_
	.db _o______
	.db _o______
	.db _oooo___
	.db _o______
	.db _o______
	.db _o______
	.db _oooooo_
cF:
	.db _oooooo_
	.db _o______
	.db _o______
	.db _oooo___
	.db _o______
	.db _o______
	.db _o______
	.db _o______
cG:
	.db __oooo__
	.db _o____o_
	.db _o______
	.db _o______
	.db _o___oo_
	.db _o____o_
	.db _o____o_
	.db __oooo__
cH:
	.db _o____o_
	.db _o____o_
	.db _o____o_
	.db _oooooo_
	.db _o____o_
	.db _o____o_
	.db _o____o_
	.db _o____o_
cI:
	.db __ooooo_
	.db ____o___
	.db ____o___
	.db ____o___
	.db ____o___
	.db ____o___
	.db ____o___
	.db __ooooo_
cJ:
	.db ______o_
	.db ______o_
	.db ______o_
	.db ______o_
	.db ______o_
	.db ______o_
	.db _o____o_
	.db __oooo__
cK:
	.db _o____o_
	.db _o___o__
	.db _o__o___
	.db _ooo____
	.db _ooo____
	.db _o__o___
	.db _o___o__
	.db _o____o_
cL:
	.db _o______
	.db _o______
	.db _o______
	.db _o______
	.db _o______
	.db _o______
	.db _o______
	.db _oooooo_
cM:
	.db o_____o_
	.db oo___oo_
	.db o_o_o_o_
	.db o__o__o_
	.db o_____o_
	.db o_____o_
	.db o_____o_
	.db o_____o_
cN:
	.db _o____o_
	.db _oo___o_
	.db _o_o__o_
	.db _o_o__o_
	.db _o__o_o_
	.db _o__o_o_
	.db _o___oo_
	.db _o____o_
cO:
	.db __oooo__
	.db _o____o_
	.db _o____o_
	.db _o____o_
	.db _o____o_
	.db _o____o_
	.db _o____o_
	.db __oooo__
cP:
	.db _ooooo__
	.db _o____o_
	.db _o____o_
	.db _o____o_
	.db _ooooo__
	.db _o______
	.db _o______
	.db _o______
cQ:
	.db __oooo__
	.db _o____o_
	.db _o____o_
	.db _o____o_
	.db _o____o_
	.db _o__o_o_
	.db _o___oo_
	.db __oooooo
cR:
	.db _ooooo__
	.db _o____o_
	.db _o____o_
	.db _ooooo__
	.db _o__o___
	.db _o__o___
	.db _o___o__
	.db _o___oo_
cS:
	.db __oooo__
	.db _o____o_
	.db _o______
	.db __oooo__
	.db ______o_
	.db ______o_
	.db _o____o_
	.db __oooo__
cT:
	.db ooooooo_
	.db ___o____
	.db ___o____
	.db ___o____
	.db ___o____
	.db ___o____
	.db ___o____
	.db ___o____
cU:
	.db _o____o_
	.db _o____o_
	.db _o____o_
	.db _o____o_
	.db _o____o_
	.db _o____o_
	.db _o____o_
	.db __oooo__
cV:
	.db _o____o_
	.db _o____o_
	.db _o____o_
	.db __o__o__
	.db __o__o__
	.db __o__o__
	.db ___oo___
	.db ___oo___
cW:
	.db o_____o_
	.db o_____o_
	.db o_____o_
	.db o_____o_
	.db o__o__o_
	.db o__o__o_
	.db o__o__o_
	.db _oo_oo__
cX:
	.db _o____o_
	.db __o__o__
	.db __o__o__
	.db ___oo___
	.db ___oo___
	.db __o__o__
	.db __o__o__
	.db _o____o_
cY:
	.db _o___o__
	.db _o___o__
	.db _o___o__
	.db __o_o___
	.db ___o____
	.db ___o____
	.db ___o____
	.db ___o____
cZ:
	.db _oooooo_
	.db ______o_
	.db _____o__
	.db ____o___
	.db ___o____
	.db __o_____
	.db _o______
	.db _oooooo_

; Lower case characters

la:
	.db ________
	.db ________
	.db ________
	.db __oooo__
	.db ______o_
	.db __ooooo_
	.db _o____o_
	.db __ooooo_
lb:
	.db _o______
	.db _o______
	.db _o______
	.db _ooooo__
	.db _o____o_
	.db _o____o_
	.db _o____o_
	.db _ooooo__
lc:
	.db ________
	.db ________
	.db ________
	.db __oooo__
	.db _o____o_
	.db _o______
	.db _o____o_
	.db __oooo__
ld:
	.db ______o_
	.db ______o_
	.db ______o_
	.db __ooooo_
	.db _o____o_
	.db _o____o_
	.db _o____o_
	.db __ooooo_
le:
	.db ________
	.db ________
	.db ________
	.db __oooo__
	.db _o____o_
	.db _ooooo__
	.db _o______
	.db __oooo__
lf:
	.db ____oo__
	.db ___o____
	.db ___o____
	.db __ooo___
	.db ___o____
	.db ___o____
	.db ___o____
	.db ___o____
lg:
	.db ________
	.db ________
	.db __oooo__
	.db _o____o_
	.db _o____o_
	.db __ooooo_
	.db ______o_
	.db __oooo__
lh:
	.db _o______
	.db _o______
	.db _o______
	.db _ooooo__
	.db _o____o_
	.db _o____o_
	.db _o____o_
	.db _o____o_
li:
	.db ________
	.db ________
	.db ___o____
	.db ________
	.db ___o____
	.db ___o____
	.db ___o____
	.db ___o____
lj:
	.db ________
	.db ____o___
	.db ________
	.db ____o___
	.db ____o___
	.db ____o___
	.db ____o___
	.db __oo____
lk:
	.db _o______
	.db _o______
	.db _o___o__
	.db _o__o___
	.db _o_o____
	.db _ooo____
	.db _o__o___
	.db _o___o__
ll:
	.db ___o____
	.db ___o____
	.db ___o____
	.db ___o____
	.db ___o____
	.db ___o____
	.db ___o____
	.db ____o___
lm:
	.db ________
	.db ________
	.db ________
	.db _oo_oo__
	.db o__o__o_
	.db o_____o_
	.db o_____o_
	.db o_____o_
ln:
	.db ________
	.db ________
	.db ________
	.db _o_ooo__
	.db _oo___o_
	.db _o____o_
	.db _o____o_
	.db _o____o_
lo:
	.db ________
	.db ________
	.db ________
	.db __oooo__
	.db _o____o_
	.db _o____o_
	.db _o____o_
	.db __oooo__
lp:
	.db ________
	.db ________
	.db _ooooo__
	.db _o____o_
	.db _o____o_
	.db _ooooo__
	.db _o______
	.db _o______
lq:
	.db ________
	.db ________
	.db __ooooo_
	.db _o____o_
	.db _o____o_
	.db __ooooo_
	.db ______o_
	.db ______o_
lr:
	.db ________
	.db ________
	.db ________
	.db _o_ooo__
	.db _oo___o_
	.db _o______
	.db _o______
	.db _o______
ls:
	.db ________
	.db ________
	.db ________
	.db __oooo__
	.db _o______
	.db __oooo__
	.db ______o_
	.db _ooooo__
lt:
	.db ________
	.db ___o____
	.db ___o____
	.db __ooo___
	.db ___o____
	.db ___o____
	.db ___o_o__
	.db ____o___
lu:
	.db ________
	.db ________
	.db ________
	.db _o___o__
	.db _o___o__
	.db _o___o__
	.db _o___o__
	.db __ooo_o_
lv:
	.db ________
	.db ________
	.db ________
	.db _o___o__
	.db _o___o__
	.db _o___o__
	.db __o_o___
	.db ___o____
lw:
	.db ________
	.db ________
	.db ________
	.db o_____o_
	.db o_____o_
	.db o_____o_
	.db o__o__o_
	.db _oo_oo__
lx:
	.db ________
	.db ________
	.db ________
	.db _o___o__
	.db __o_o___
	.db ___o____
	.db __o_o___
	.db _o___o__
ly:
	.db ________
	.db ________
	.db ________
	.db __o___o_
	.db ___o_o__
	.db ____o___
	.db ___o____
	.db _oo_____
lz:
	.db ________
	.db ________
	.db ________
	.db _oooooo_
	.db _____o__
	.db ___oo___
	.db __o_____
	.db _oooooo_

