//Raul Velasco
//date: 02/22/2023
//

//Examle used in class
void ship_cool_down(float *color){
	color[1] += 0.01;
	color[2] += 0.01;

	if(color[1] > 1.0){
		color[1] = 1.0;
	}

	if(color[2] > 1.0){
		color[2] = 1.0;
	}
}
