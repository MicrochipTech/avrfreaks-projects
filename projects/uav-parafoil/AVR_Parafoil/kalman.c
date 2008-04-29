#include "matrix.h"
#include "kalman.h"


kalman_state predict_and_update(kalman_state * Prev, kalman_model * model, vector * u, vector * y) 
{
	kalman_state New;
	matrix K;
	New.state=predict_state(model,Prev,u);
	New.P=predict_P(model,Prev);
	K=optimal_gain(model,&New);
	New.state=state_update(&New,model,&K,y);
	New.P=P_update(&K,model,&New);
	return New;
}

vector predict_state(kalman_model * model, kalman_state * state, vector * u)		//x is system state, u control input(s), B input model
{
	vector a;
	vector b;
	a=matrix_vector(&model->F,&state->state);
	b=matrix_vector(&model->B,u);
	return vector_add(&a,&b);								//estimated state
}

matrix predict_P(kalman_model * model, kalman_state * state)		//F is propagation model, Q model noise
{
	matrix S=transpose(&model->F);
	S=multiply(&state->P,&S);
	S=multiply(&model->F,&S);
	return add(&S,&model->Q);										//estimated estimate covariance
}

matrix optimal_gain(kalman_model * model, kalman_state * state)		//H is measurement model, R measurement noise
{
	matrix T=transpose(&model->H);
	matrix S=multiply(&state->P,&T);
	S=multiply(&model->H,&S);
	S=add(&model->R,&S);					//S is now the innovation covariance	
	S=inverse(&S);
	S=multiply(&T,&S);
	return multiply(&state->P,&S);			//the optimal gain
} 

vector state_update(kalman_state * state, kalman_model * model, matrix * K, vector * y)	//y is the measurements
{
	vector a=matrix_vector(&model->H,&state->state);
	a=vector_subtract(y,&a);										//a is now the difference or residual
	a=matrix_vector(K,&a);
	return vector_add(&state->state,&a);
}

matrix P_update(matrix *K, kalman_model * model, kalman_state * state)		//K is optimal gain, H measurement model
{
	matrix S=multiply(K,&model->H);
	S.tl=1-S.tl;					//subtraction from identity
	S.tr=-S.tr;
	S.bl=-S.bl;
	S.br=1-S.br;
	return multiply(&S,&state->P);
}
