//fit data to a line of the form
//f(x) = a1*x + a2
void fitLin(const parameters * p, const data * d, fit_results * fr)
{
  //construct equations
  int i,j;
  lin_eq_type linEq;
  linEq.dim=2;
  
  linEq.matrix[0][0]=d->xpowsum[0][2];
  linEq.matrix[0][1]=d->xpowsum[0][1]; 
  linEq.matrix[1][0]=d->xpowsum[0][1]; 
  linEq.matrix[1][1]=d->xpowsum[0][0];//bottom right entry
  
  linEq.vector[0]=d->mxpowsum[0][1];
  linEq.vector[1]=d->mxpowsum[0][0];
    
  //solve system of equations and assign values
  if(!(solve_lin_eq(&linEq)==1))
    {
      printf("ERROR: Could not determine fit parameters.\n");
      exit(-1);
    }
  
  //save fit parameters  
  for(i=0;i<linEq.dim;i++)
    fr->a[i]=linEq.solution[i];
  long double f;
  fr->chisq=0;
  fr->ndf=d->lines-3;
  for(i=0;i<d->lines;i++)//loop over data points for chisq
    {
      f=fr->a[0]*d->x[0][i] + fr->a[1];
      fr->chisq+=(d->x[1][i] - f)*(d->x[1][i] - f)
                  /(d->x[1+1][i]*d->x[1+1][i]);
    }
  
  //Calculate covariances and uncertainties, see J. Wolberg 
  //'Data Analysis Using the Method of Least Squares' sec 2.5
  for(i=0;i<linEq.dim;i++)
    for(j=0;j<linEq.dim;j++)
      fr->covar[i][j]=linEq.inv_matrix[i][j]*(fr->chisq/fr->ndf);
  for(i=0;i<linEq.dim;i++)
    fr->aerr[i]=(long double)sqrt((double)(fr->covar[i][i]));
    
  //now that the fit is performed, use the fit parameters to find the intercept(s)
  fr->fitVert[0]=-1.0*fr->a[1]/fr->a[0];//x-intercept
  fr->fitVert[1]=fr->a[1];//y-intercept
  
  //set up quantities to compute confidence interval
  long double xb,yb,sxx,syx,tval;
  tval=t_stat(d->lines-2,0.025);//t-statistic t(df,alpha/2)
  xb=d->xpowsum[0][1]/d->lines;
  yb=d->msum/d->lines;
  sxx=0.;
  syx=0.;
  for(i=0;i<d->lines;i++)
  	{
  		sxx+=(d->x[0][i] - xb)*(d->x[0][i] - xb);
  		syx+=(d->x[p->numVar][i] - yb)*(d->x[p->numVar][i] - yb);
  	}
  
  /*if(p->verbose>=2)
  	{
		 	printf("Confidence interval t-statistic: %LF\n",tval);
		 	printf("xb: %LF\n",xb);
		 	printf("yb: %LF\n",yb);
		 	printf("sxx: %LF\n",sxx);
		 	printf("syx: %LF\n",syx);
		 	printf("1/n: %lF\n",1.0/d->lines);
 		}*/
  
  //set up equation forms for plotting
  if(strcmp(p->plotMode,"1d")==0)
    {
      sprintf(fr->fitForm[0], "%Lf*x + %Lf",fr->a[0],fr->a[1]);
      sprintf(fr->ciUForm[0], "%Lf*x + %Lf + (%LF * sqrt(%LF/%i) * ((1.0/%i) + sqrt(((x - %LF)**2)/%LF)))",fr->a[0],fr->a[1],tval,syx,d->lines-2,d->lines,xb,sxx);
      sprintf(fr->ciLForm[0], "%Lf*x + %Lf - (%LF * sqrt(%LF/%i) * ((1.0/%i) + sqrt(((x - %LF)**2)/%LF)))",fr->a[0],fr->a[1],tval,syx,d->lines-2,d->lines,xb,sxx);
      sprintf(fr->piUForm[0], "%Lf*x + %Lf + (%LF * sqrt(%LF/%i) * (1.0 + (1.0/%i) + sqrt(((x - %LF)**2)/%LF)))",fr->a[0],fr->a[1],tval,syx,d->lines-2,d->lines,xb,sxx);
      sprintf(fr->piLForm[0], "%Lf*x + %Lf - (%LF * sqrt(%LF/%i) * (1.0 + (1.0/%i) + sqrt(((x - %LF)**2)/%LF)))",fr->a[0],fr->a[1],tval,syx,d->lines-2,d->lines,xb,sxx);
    }
  
  
	/*//generate slope/intercept pairs for confidence interval
	//calculates the value(s) of the confidence interval at the given point
	//Ref: A. Chester master thesis
  int gridSize=(int)(CI_EE_DIM/2.);
	long double c=2.30;//confidence level for 1-sigma in 2 parameters
	long double a0=sqrt((d->xpowsum[0][0]*c)/(d->xpowsum[0][0]*d->xpowsum[0][2] - d->xpowsum[0][1]*d->xpowsum[0][1]));
	long double p0,p1,p2,a0b,a1b1,a1b2;
  p0=d->xpowsum[0][0];
  fr->ciEEValues=0;
	for (i=0;i<=gridSize;i++)
		{
			a0b=fr->a[0]+((2*i - gridSize)/(double)gridSize)*a0;
			//printf("a0b: %Lf\n",a0b);
			p1=(2.*d->xpowsum[0][1]*(a0b - fr->a[0])) - 2.*d->xpowsum[0][0]*fr->a[1];
			p2=d->xpowsum[0][0]*fr->a[1]*fr->a[1] - 2.*d->xpowsum[0][1]*fr->a[1]*(a0b - fr->a[0]) + d->xpowsum[0][2]*(a0b - fr->a[0])*(a0b - fr->a[0]) - c;
			//printf("p1*p1 - 4.*p0*p2: %Lf\n",p1*p1 - 4.*p0*p2);
			a1b1=(-1.*p1 + sqrt(p1*p1 - 4.*p0*p2))/(2.*p0);
			a1b2=(-1.*p1 - sqrt(p1*p1 - 4.*p0*p2))/(2.*p0);
			//printf("a1b1: %Lf, a1b2: %Lf\n",a1b1,a1b2);
			//record pairs of slope/intercept values on error ellipse
			fr->ciEEVal[fr->ciEEValues][0]=a0b;
			fr->ciEEVal[fr->ciEEValues][1]=a1b1;
			fr->ciEEValues++;
			fr->ciEEVal[fr->ciEEValues][0]=a0b;
			fr->ciEEVal[fr->ciEEValues][1]=a1b2;
			fr->ciEEValues++;
		}*/
  
}

/*long double confIntVal(long double input, const fit_results * fr, const data * d, int upper)
{
	int i;	
	long double cVal;
	long double maxCVal=0.;
	long double minCVal=BIG_NUMBER;
	
	for (i=0;i<fr->ciEEValues;i++)
		{
			cVal=fr->ciEEVal[i][0]*input + fr->ciEEVal[i][1];
			//printf("cVal: %LE\n",cVal);
			if(cVal>maxCVal)
				maxCVal=cVal;
			if(cVal<minCVal)
				minCVal=cVal;
		}
	if(upper==1)
		return maxCVal;
	else
		return minCVal;

}*/

//prints the results
void printLin(const data * d, const parameters * p, const fit_results * fr)
{
  //simplified data printing depending on verbosity setting
  if(p->verbose==1)
    {
      //print x and y intercept
      printf("%LE %LE\n",fr->fitVert[0],fr->fitVert[1]);
      return;
    }
  
  printf("\nFIT RESULTS\n-----------\n");
  printf("Uncertainties reported at 1-sigma.\n");
  printf("Fit function: f(x,y) = a1*x + a2\n\n");
  printf("Best chisq (fit): %0.3Lf\nBest chisq/NDF (fit): %0.3Lf\n\n",fr->chisq,fr->chisq/fr->ndf);
  printf("Coefficients from fit: a1 = %LE +/- %LE\n",fr->a[0],fr->aerr[0]);
  printf("                       a2 = %LE +/- %LE\n",fr->a[1],fr->aerr[1]);
  printf("\n");
  
  printf("x-intercept = %LE\n",fr->fitVert[0]);
  printf("y-intercept = %LE\n",fr->fitVert[1]);
  
  //printf("value at x=90 = %LE\n",fr->a[0]*90. + fr->a[1]);
  //printf("CI at x=90 = [%LE %LE]\n",confIntVal(90.,fr,d,1),confIntVal(90.,fr,d,0));
    
}
