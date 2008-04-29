#include "matrix.h"
//matrix operations

matrix multiply(matrix * a, matrix * b)
{
	matrix ans;
	ans.tl=(a->tl*b->tl)+(a->tr*b->bl);
	ans.tr=(a->tl*b->tr)+(a->tr*b->br);
	ans.bl=(a->bl*b->tl)+(a->br*b->bl);
	ans.br=(a->bl*b->tr)+(a->br*b->br);
	return ans;
}

matrix inverse(matrix * a)
{
	matrix ans;
	float factor;
	factor=1.0/((a->tl*a->br)-(a->bl*a->tr));
	ans.tl=factor*a->br;
	ans.tr=-factor*a->tr;
	ans.bl=-factor*a->bl;
	ans.br=factor*a->tl;
	return ans;
}

matrix add(matrix * a, matrix * b)
{
	matrix ans;
	ans.tl=a->tl+b->tl;
	ans.tr=a->tr+b->tr;
	ans.bl=a->bl+b->bl;
	ans.br=a->br+b->br;
	return ans;
}
/*
matrix subtract(matrix * a, matrix * b)
{
	matrix ans;
	ans.tl=a->tl-b->tl;
	ans.tr=a->tr-b->tr;
	ans.bl=a->bl-b->bl;
	ans.br=a->br-b->br;
	return ans;
}
*/
matrix transpose(matrix * a)
{
	matrix ans;
	ans.tl=a->tl;
	ans.tr=a->bl;
	ans.bl=a->tr;
	ans.br=a->br;
	return ans;
}

vector matrix_vector(matrix * a, vector * b)
{
	vector ans;
	ans.t=(a->tl*b->t)+(a->tr*b->b);
	ans.b=(a->bl*b->t)+(a->br*b->b);
	return ans;
}

vector vector_add(vector * a, vector * b)
{
	vector ans;
	ans.t=a->t+b->t;
	ans.b=a->b+b->b;
	return ans;
}

vector vector_subtract(vector * a, vector * b)
{
	vector ans;
	ans.t=a->t-b->t;
	ans.b=a->b-b->b;
	return ans;
}



