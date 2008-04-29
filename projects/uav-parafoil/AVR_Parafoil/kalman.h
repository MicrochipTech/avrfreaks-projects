typedef struct
{
	vector state;	//system state
	matrix P;		//estimate covariance
} kalman_state;

typedef struct
{
	matrix H;	//measurement model
	matrix B;	//control model
	matrix F;	//propogation model
	matrix Q;	//process noise
	matrix R;	//measurement moise
} kalman_model;

kalman_state predict_and_update(kalman_state * Prev, kalman_model * model, vector * u, vector * y);
vector predict_state(kalman_model * model, kalman_state * state, vector * u);
matrix predict_P(kalman_model * model, kalman_state * state);
matrix optimal_gain(kalman_model * model, kalman_state * state);
vector state_update(kalman_state * state, kalman_model * model, matrix * K, vector * y);
matrix P_update(matrix *K, kalman_model * model, kalman_state * state);
