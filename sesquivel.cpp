//Stephanie Esquivel

void pink_ship(float *color)
{
	color[0] += 0.01;
	color[1] += 0.01;
	if (color[0] > 1.0)
		color[1] = 0.0;
	if (color[1] > 1.0)
		color[1] = 1.0;
	
}
