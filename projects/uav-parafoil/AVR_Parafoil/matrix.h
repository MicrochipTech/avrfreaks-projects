typedef struct
{
	float tl;
	float tr;
	float bl;
	float br;
} matrix;

typedef struct
{
	float t;
	float b;
} vector;


matrix multiply(matrix * a, matrix * b);
matrix inverse(matrix * a);
matrix add(matrix * a, matrix * b);
//matrix subtract(matrix * a, matrix * b);
matrix transpose(matrix * a);
vector matrix_vector(matrix * a, vector * b);
vector vector_add(vector * a, vector * b);
vector vector_subtract(vector * a, vector * b);
