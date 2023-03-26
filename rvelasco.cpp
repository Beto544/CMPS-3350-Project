//Raul Velasco
//date: 02/22/2023
//

//Examle used in class
void cool_down(float *color){
    color[0] += .01;
    color[1] += .01;
    if (color[0] > 1.0)
        color[0] = 0.1;

    if (color[1] > 1.0)
        color[1] = 0.1;
}
	
