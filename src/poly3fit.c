//evaluates the fit function at the specified point
long double evalPoly3(long double x, const fit_results * fr)
{
	return fr->a[0]*x*x*x + fr->a[1]*x*x
					+ fr->a[2]*x + fr->a[3];
}

//evaluates the fit function x values at the specified y value
//returns the x value closest to closeToVal, above it if pos = 1, below if pos = 0
long double evalPoly3X(long double y, const fit_results * fr, long double closeToVal, int pos)
{
  int i;
  long double discr;//discriminant
  long double roots[3];
  int numRoots=0;
  long double a,b,c,d;
  long double p,q;

  a=fr->a[0];
  b=fr->a[1];
  c=fr->a[2];
  d=fr->a[3] - y;

  p=(3.0*a*c - b*b)/(3.0*a*a);
  q=(2.0*b*b*b - 9.0*a*b*c + 27.0*a*a*d)/(27.0*a*a*a);
  
  discr=18.0*a*b*c*d - 4.0*b*b*b*d + b*b*c*c - 4.0*a*c*c*c - 27.0*a*a*d*d;
  if(discr>0.0)//3 real roots
  	{
  		for(i=0;i<3;i++)
  			{
  				roots[i]=2*sqrt(-1.0*p/3.0)*cos( (1.0/3.0) * acos( (3.0*q/(2.0*p)) * sqrt(-3.0/p) ) - ((2.0*PI*i)/3.0) );
  				roots[i]-=b/(3.0*a);
  			}
      numRoots=3;
  	}
  else if(discr==0.0)//triple root
  	{
      if(b*b - 3.*a*c == 0)
        {
          roots[0]=b/(3.0*a);
  		    numRoots=1;
        }
      else
        {
          roots[0]=(9.*a*d - b*c)/(2.*(b*b - 3.*a*c));
          roots[1]=(4.*a*b*c - 9.*a*a*d - b*b*b)/(a*(b*b - 3.*a*c));
          numRoots=2;
        }
  	}
  else//one real, 2 complex roots
  	{
      if((p<0.)&&((4.*p*p*p + 27.*q*q)>0.))
        {
          roots[0]=-2.*(abs(q)/q)*sqrt(-1.*(p/3.))*cosh((1./3.)*acosh(-3.*abs(q)*sqrt(-3./p)/(2.*p)));
          numRoots=1;
        }
      else if (p>0.)
        {
          roots[0]=-2.*sqrt(p/3.)*sinh((1./3.)*asinh(3.*q*sqrt(3./p)/(2.*p)));
          numRoots=1;
        }
      else
        numRoots=0;
  	}
  
  long double minDiff,diff;
  int selRoot = -1;
  if(pos==0)
    minDiff=-1*BIG_NUMBER;
  else
    minDiff=BIG_NUMBER;
  for(i=0;i<numRoots;i++)
    {
      diff=roots[i]-closeToVal;
      //printf("pos=%i, roots[%i]=%Lf, diff=%Lf\n",pos,i,roots[i],diff);
      if((pos==1)&&(diff<minDiff)&&(diff>=0.))
        {
          diff=minDiff;
          selRoot=i;
        }
      else if((pos==0)&&(diff>minDiff)&&(diff<0.))
        {
          diff=minDiff;
          selRoot=i;
        }
    }
  
  if(selRoot>=0)
    return roots[selRoot];
  else
    {
      printf("WARNING: could not evaluate roots of function.\n");
      return 0;
    }

}


//determine uncertainty bounds for the critical point by intersection of fit function with line defining values at min + delta
//done by shifting the function by the value at the minimum + a confidence level, and finding the roots around that minimum
//delta is the desired confidence level (1.00 for 1-sigma in 1 parameter)
void fitPoly3ChisqConf(const parameters * p, fit_results * fr, long double pt)
{
  
  long double vertVal = evalPoly3(pt,fr);
  //printf("vertval: %LF\n",vertVal);
  long double a,b,c,d;
  long double delta=p->ciDelta;
  long double discr;//discriminant
  long double roots[3];
  fr->vertBoundsFound=1;
  
  a=fr->a[0];
  b=fr->a[1];
  c=fr->a[2];
  d=fr->a[3] - delta - vertVal;
  
  discr=18.0*a*b*c*d - 4.0*b*b*b*d + b*b*c*c - 4.0*a*c*c*c - 27.0*a*a*d*d;
  if(discr>0.0)//3 real roots
  	{
  		int i;
  		long double p,q;
  		p=(3.0*a*c - b*b)/(3.0*a*a);
  		q=(2.0*b*b*b - 9.0*a*b*c + 27.0*a*a*d)/(27.0*a*a*a);
  		for(i=0;i<3;i++)
  			{
  				roots[i]=2*sqrt(-1.0*p/3.0)*cos( (1.0/3.0) * acos( (3.0*q/(2.0*p)) * sqrt(-3.0/p) ) - ((2.0*PI*i)/3.0) );
  				roots[i]-=b/(3.0*a);
  			}
  	}
  else if(discr==0.0)//triple root
  	{
  		fr->vertBoundsFound=0;
  	}
  else//one real, 2 complex roots
  	{
  		fr->vertBoundsFound=0;
  	}
  
  if(fr->vertBoundsFound==1)
  	{
      //printf("a0=%LE, a1=%LE, a2=%LE, a3=%LE]\n",fr->a[0],fr->a[1],fr->a[2],fr->a[3]);
  		//printf("Bounds around point at x=%LE are [%LE %LE %LE]\n",pt,roots[0],roots[1],roots[2]);
  		int i;
  		long double uInterval=BIG_NUMBER;
  		long double lInterval=BIG_NUMBER;
  		for(i=0;i<3;i++)
  			{
					if(roots[i]-pt > 0.0)
						if(roots[i]-pt < uInterval)
							uInterval=roots[i]-pt;
					if(pt-roots[i] > 0.0)
						if(pt-roots[i] < lInterval)
							lInterval=pt-roots[i];
				}
			fr->vertUBound[0]=pt+uInterval;
			fr->vertLBound[0]=pt-lInterval;
  	}

}


//prints fit data
void printPoly3(const data * d, const parameters * p, const fit_results * fr)
{

  int i;

  //simplified data printing depending on verbosity setting
  if(p->verbose==1)
    {
      //print critical points
      printf("%LE %LE\n",fr->fitVert[0],fr->fitVert[1]);
      return;
    }
  else if(p->verbose==2)
    {
      //print coefficient values
      for(i=0;i<4;i++)
        printf("%LE ",fr->a[i]);
      printf("\n");
      return;
    } 
  
  printf("\nFIT RESULTS\n-----------\n");
  printf("Fit parameter uncertainties reported at 1-sigma.\n");
  printf("Fit function: f(x,y) = a1*x^3 + a2*x^2 + a3*x + a4\n\n");
  printf("Best chisq (fit): %0.3Lf\nBest chisq/NDF (fit): %0.3Lf\n\n",fr->chisq,fr->chisq/fr->ndf);
  printf("Coefficients from fit: a1 = %LE +/- %LE\n",fr->a[0],fr->aerr[0]);
  for(i=1;i<4;i++)
    printf("                       a%i = %LE +/- %LE\n",i+1,fr->a[i],fr->aerr[i]);
  printf("\n");
  
  printf("y-intercept = %LE\n",evalPoly3(0.0,fr));
  if(strcmp(p->dataType,"chisq")==0)
    if((fr->fitVert[0]<0.)||(fr->fitVert[1]<0.)||(p->forceZeroX))
      printf("Upper bound (with %s confidence interval) assuming minimum at zero: x = %LE\n",p->ciSigmaDesc,evalPoly3X(evalPoly3(0.0,fr)+p->ciDelta,fr,0.0,1));
  printf("\n");

  //check for NaN
  if((fr->fitVert[0]==fr->fitVert[0])&&(fr->fitVert[1]==fr->fitVert[1]))
  	{
    	printf("Critical points at x = [ %LE %LE ]\n",fr->fitVert[0],fr->fitVert[1]);
    	printf("At critical points, y = [ %LE %LE ]\n",evalPoly3(fr->fitVert[0],fr),evalPoly3(fr->fitVert[1],fr));
    	//print confidence bounds
    	if((strcmp(p->dataType,"chisq")==0)&&(fr->vertBoundsFound==1))
    		{
					if(evalPoly3(fr->fitVert[0],fr)<evalPoly3(fr->fitVert[1],fr))
						{
							if((float)(fr->vertUBound[0]-fr->fitVert[0])==(float)(fr->fitVert[0]-fr->vertLBound[0]))
								printf("Local minimum (with %s confidence interval): x = %LE +/- %LE\n",p->ciSigmaDesc, fr->fitVert[0],fr->vertUBound[0]-fr->fitVert[0]);
							else
								printf("Local minimum (with %s confidence interval): x = %LE + %LE - %LE\n",p->ciSigmaDesc,fr->fitVert[0],fr->vertUBound[0]-fr->fitVert[0],fr->fitVert[0]-fr->vertLBound[0]);
						}
					else
						{
							if((float)(fr->vertUBound[0]-fr->fitVert[1])==(float)(fr->fitVert[1]-fr->vertLBound[0]))
								printf("Local minimum (with %s confidence interval): x = %LE +/- %LE\n",p->ciSigmaDesc,fr->fitVert[1],fr->vertUBound[0]-fr->fitVert[1]);
							else
								printf("Local minimum with (%s confidence interval): x = %LE + %LE - %LE\n",p->ciSigmaDesc,fr->fitVert[1],fr->vertUBound[0]-fr->fitVert[1],fr->fitVert[1]-fr->vertLBound[0]);
						}
				}
    }
  else
    printf("Fit function is monotonic (no critical points).\n");

  if((p->findMinGridPoint == 1)||(p->findMaxGridPoint == 1)){
    printf("\n");
		int i;
    if(p->findMinGridPoint == 1){
      long double currentVal;
      long double minVal = BIG_NUMBER;
      int minPt = -1;
      for(i=0;i<d->lines;i++){
        currentVal = evalPoly3(d->x[0][i],fr);
        if(currentVal < minVal){
          minVal = currentVal;
          minPt = i;
        }
      }
      if(minPt >= 0){
        printf("Grid point corresponding to the lowest value (%LE) of the fitted function is at [ %0.3LE ].\n",minVal,d->x[0][minPt]);
      }
    }
    if(p->findMaxGridPoint == 1){
      long double currentVal;
      long double maxVal = -1.0*BIG_NUMBER;
      int maxPt = -1;
      for(i=0;i<d->lines;i++){
        currentVal = evalPoly3(d->x[0][i],fr);
        if(currentVal > maxVal){
          maxVal = currentVal;
          maxPt = i;
        }
      }
      if(maxPt >= 0){
        printf("Grid point corresponding to the highest value (%LE) of the fitted function is at [ %0.3LE ].\n",maxVal,d->x[0][maxPt]);
      }
    }
  }
}


void plotFormPoly3(const parameters * p, fit_results * fr)
{
	//set up equation forms for plotting
	if(strcmp(p->plotMode,"1d")==0)
		{
			sprintf(fr->fitForm[0], "%.10LE*x*x*x + %0.20LE*x*x + %.10LE*x + %.10LE",fr->a[0],fr->a[1],fr->a[2],fr->a[3]);
		}
}


//fit data to a 3rd order polynomial of the form
//f(x,y) = a1*x^3 + a2*x^2 + a3*x + a4
void fitPoly3(const parameters * p, const data * d, fit_results * fr, plot_data * pd, int print)
{
  //construct equations (n=1 specific case)
  int i,j;
  lin_eq_type linEq;
  linEq.dim=4;
  
  linEq.matrix[0][0]=d->xxpowsum[0][3][0][3];
  linEq.matrix[0][1]=d->xxpowsum[0][3][0][2];
  linEq.matrix[0][2]=d->xxpowsum[0][3][0][1];
  linEq.matrix[0][3]=d->xpowsum[0][3];
  
  linEq.matrix[1][1]=d->xxpowsum[0][3][0][1];
  linEq.matrix[1][2]=d->xpowsum[0][3];
  linEq.matrix[1][3]=d->xpowsum[0][2];
  
  linEq.matrix[2][2]=d->xpowsum[0][2];
  linEq.matrix[2][3]=d->xpowsum[0][1];
  
  linEq.matrix[3][3]=d->xpowsum[0][0];//bottom right entry
  
  //mirror the matrix (top right half mirrored to bottom left half)
  for(i=1;i<linEq.dim;i++)
    for(j=0;j<i;j++)
      linEq.matrix[i][j]=linEq.matrix[j][i];
  
  linEq.vector[0]=d->mxpowsum[0][3];
  linEq.vector[1]=d->mxpowsum[0][2];
  linEq.vector[2]=d->mxpowsum[0][1];
  linEq.vector[3]=d->mxpowsum[0][0];
    
	//solve system of equations and assign values
	if(!(solve_lin_eq(&linEq)==1))
		{
			printf("ERROR: Could not determine fit parameters (poly3).\n");
			printf("Perhaps there are not enough data points to perform a fit?\n");
      printf("Otherwise you can also try adjusting the fit range using the UPPER_LIMITS and LOWER_LIMITS options.\n");
			exit(-1);
		}
  
  //save fit parameters  
  for(i=0;i<linEq.dim;i++)
    fr->a[i]=linEq.solution[i];
  long double f;
  fr->chisq=0;
  fr->ndf=d->lines-5;
  for(i=0;i<d->lines;i++)//loop over data points for chisq
    {
      f=fr->a[0]*d->x[0][i]*d->x[0][i]*d->x[0][i] + fr->a[1]*d->x[0][i]*d->x[0][i] + fr->a[2]*d->x[0][i] + fr->a[3];
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
    
  //now that the fit is performed, use the fit parameters (and the derivative of the fitting function) to find the critical points
  fr->fitVert[0]=-1.0*fr->a[1] - sqrt(fr->a[1]*fr->a[1] - 3.*fr->a[0]*fr->a[2]);
  fr->fitVert[0]/=3.*fr->a[0];
  fr->fitVert[1]=-1.0*fr->a[1] + sqrt(fr->a[1]*fr->a[1] - 3.*fr->a[0]*fr->a[2]);
  fr->fitVert[1]/=3.*fr->a[0];
  
  
  //find confidence bounds if neccessary
  if(strcmp(p->dataType,"chisq")==0)
  	{
  		if(evalPoly3(fr->fitVert[0],fr)<evalPoly3(fr->fitVert[1],fr))
  			fitPoly3ChisqConf(p,fr,fr->fitVert[0]);
  		else
  			fitPoly3ChisqConf(p,fr,fr->fitVert[1]);
  	}
  //print results
  if(print==1)
		printPoly3(d,p,fr);
	
	if((p->plotData==1)&&(p->verbose<1))
		{
			preparePlotData(d,p,fr,pd);
			plotFormPoly3(p,fr);
			plotData(p,fr,pd);
		}
  
}
