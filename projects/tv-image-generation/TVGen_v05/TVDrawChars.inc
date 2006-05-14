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


cT:
	.db ooooooo_
	.db ___o____
	.db ___o____
	.db ___o____
	.db ___o____
	.db ___o____
	.db ___o____
	.db ___o____
cV:
	.db _o____o_
	.db _o____o_
	.db _o____o_
	.db __o__o__
	.db __o__o__
	.db __o__o__
	.db ___oo___
	.db ___oo___


cD:
	.db _ooooo__
	.db _o____o_
	.db _o____o_
	.db _o____o_
	.db _o____o_
	.db _o____o_
	.db _o____o_
	.db _ooooo__
la:
	.db ________
	.db ________
	.db ________
	.db __oooo__
	.db ______o_
	.db __ooooo_
	.db _o____o_
	.db __ooooo_
lr:
	.db ________
	.db ________
	.db ________
	.db _o_ooo__
	.db _oo___o_
	.db _o______
	.db _o______
	.db _o______
lw:
	.db ________
	.db ________
	.db ________
	.db o_____o_
	.db o_____o_
	.db o_____o_
	.db o__o__o_
	.db _oo_oo__

; Icons

PENICON:
	.db _____o__
	.db ____o_o_
	.db ___o_o_o
	.db __o___o_
	.db _o___o__
	.db _o__o___
	.db oooo____
	.db oo______
ERSICON:
	.db _____o__
	.db ____ooo_
	.db ___ooooo
	.db __ooooo_
	.db _ooooo__
	.db ooooo___
	.db _ooo____
	.db __o_____
sp:
	.db ________
	.db ________
	.db ________
	.db ________
	.db ________
	.db ________
	.db ________
	.db ________
al:
	.db ________
	.db ________
	.db __o_____
	.db _o______
	.db ooooooo_
	.db _o______
	.db __o_____
	.db ________
ar:
	.db ________
	.db ________
	.db ____o___
	.db _____o__
	.db ooooooo_
	.db _____o__
	.db ____o___
	.db ________
au:
	.db ________
	.db ___o____
	.db __ooo___
	.db _o_o_o__
	.db ___o____
	.db ___o____
	.db ___o____
	.db ___o____
ad:
	.db ________
	.db ___o____
	.db ___o____
	.db ___o____
	.db ___o____
	.db _o_o_o__
	.db __ooo___
	.db ___o____

alx:
	.db ________
	.db o_______
	.db o__o____
	.db o_o_____
	.db ooooooo_
	.db o_o_____
	.db o__o____
	.db o_______
arx:
	.db ________
	.db ______o_
	.db ___o__o_
	.db ____o_o_
	.db ooooooo_
	.db ____o_o_
	.db ___o__o_
	.db ______o_
aux:
	.db ________
	.db ooooooo_
	.db ___o____
	.db __ooo___
	.db _o_o_o__
	.db ___o____
	.db ___o____
	.db ___o____
adx:
	.db ________
	.db ___o____
	.db ___o____
	.db ___o____
	.db _o_o_o__
	.db __ooo___
	.db ___o____
	.db ooooooo_


cp1:
	.db _ooo____
	.db __o__oo_
	.db _o___oo_
	.db ________
	.db o_o_____
	.db ooo_oo__
	.db ooo_o_o_
	.db o_o_oooo
cp2:
	.db o_______
	.db oo__oo__
	.db o_o_o_o_
	.db ________
	.db _o_o___o
	.db o__o_o__
	.db _o_oo__o
	.db o__o_o_o
cp3:
	.db ________
	.db ________
	.db ________
	.db ________
	.db ______o_
	.db _oo__o__
	.db _o_o__o_
	.db _o_o_o__
	