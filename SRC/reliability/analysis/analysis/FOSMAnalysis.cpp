/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 2001, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** Reliability module developed by:                                   **
**   Terje Haukaas (haukaas@ce.berkeley.edu)                          **
**   Armen Der Kiureghian (adk@ce.berkeley.edu)                       **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.4 $
// $Date: 2007-10-31 16:41:40 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/analysis/analysis/FOSMAnalysis.cpp,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu)
//

#include <FOSMAnalysis.h>
#include <ReliabilityAnalysis.h>
#include <ReliabilityDomain.h>
#include <RandomVariablePositioner.h>
#include <GFunEvaluator.h>
#include <GradGEvaluator.h>
#include <Matrix.h>
#include <Vector.h>
#include <tcl.h>

#include <RandomVariableIter.h>
#include <LimitStateFunctionIter.h>

#include <fstream>
#include <iomanip>
#include <iostream>
using std::ifstream;
using std::ios;
using std::setw;
using std::setprecision;
using std::setiosflags;


FOSMAnalysis::FOSMAnalysis(ReliabilityDomain *passedReliabilityDomain,
							   GFunEvaluator *passedGFunEvaluator,
							   GradGEvaluator *passedGradGEvaluator,
							   Tcl_Interp *passedTclInterp,
							   TCL_Char *passedFileName)
:ReliabilityAnalysis()
{
	theReliabilityDomain	= passedReliabilityDomain;
	theGFunEvaluator		= passedGFunEvaluator;
	theGradGEvaluator = passedGradGEvaluator;
	theTclInterp			= passedTclInterp;
	strcpy(fileName,passedFileName);
}


FOSMAnalysis::~FOSMAnalysis()
{

}



int 
FOSMAnalysis::analyze(void)
{

	// Alert the user that the FORM analysis has started
	opserr << "FOSM Analysis is running ... " << endln;


	// Initial declarations
	int i;


	// Open output file
	ofstream outputFile( fileName, ios::out );


	// Get number of random variables and limit-state function
	int nrv = theReliabilityDomain->getNumberOfRandomVariables();
	int numLsf = theReliabilityDomain->getNumberOfLimitStateFunctions();

	
	// Get mean point
	Vector meanVector(nrv);
	// Establish vector of standard deviations
	Vector stdvVector(nrv);

	RandomVariableIter rvIter = theReliabilityDomain->getRandomVariables();
	RandomVariable *aRandomVariable;
	//for (i=1; i<=nrv; i++) {
	while ((aRandomVariable = rvIter()) != 0) {
	  int i = aRandomVariable->getIndex();
	  meanVector(i) = aRandomVariable->getMean();
	  stdvVector(i) = aRandomVariable->getStdv();
	}

	// Evaluate limit-state functions
	Vector meanEstimates(numLsf);
	int result;
	int lsf;
	result = theGFunEvaluator->runGFunAnalysis(meanVector);
	if (result < 0) {
		opserr << "FOSMAnalysis::analyze() - " << endln
			<< " could not run analysis to evaluate limit-state function. " << endln;
		return -1;
	}
	LimitStateFunctionIter lsfIter = theReliabilityDomain->getLimitStateFunctions();
	LimitStateFunction *theLSF;
	//for (lsf=1; lsf<=numLsf; lsf++ ) {
	while ((theLSF = lsfIter()) != 0) {
	  
	  int lsf = theLSF->getTag();
	  theReliabilityDomain->setTagOfActiveLimitStateFunction(lsf);
		
	  // set namespace variable for tcl functions
	  Tcl_SetVar2Ex(theTclInterp,"RELIABILITY_lsf",NULL,Tcl_NewIntObj(lsf),TCL_NAMESPACE_ONLY);
		
	  result = theGFunEvaluator->evaluateG(meanVector);
	  if (result < 0) {
	    opserr << "FOSMAnalysis::analyze() - " << endln
		   << " could not tokenize limit-state function. " << endln;
	    return -1;
	  }
	  int i = theLSF->getIndex();
	  meanEstimates(i) = theGFunEvaluator->getG();
	}


	// Evaluate the gradients of limit-state functions
	Matrix matrixOfGradientVectors(nrv,numLsf);
	result = theGradGEvaluator->computeAllGradG(meanEstimates,meanVector);
	if (result < 0) {
		opserr << "FOSMAnalysis::analyze() -- could not" << endln
			<< " compute gradients of the limit-state function. " << endln;
		return -1;
	}
	matrixOfGradientVectors = theGradGEvaluator->getAllGradG();


	// Establish covariance matrix
	Matrix covMatrix(nrv,nrv);
	for (i = 0; i < nrv; i++) {
	  covMatrix(i,i) = stdvVector(i)*stdvVector(i);
	}
	int ncorr = theReliabilityDomain->getNumberOfCorrelationCoefficients();
	CorrelationCoefficient *theCorrelationCoefficient;
	double covariance, correlation;
	int rv1, rv2;
	RandomVariable *rv1Ptr;
	RandomVariable *rv2Ptr;
	for (i=1 ; i<=ncorr ; i++) {
		theCorrelationCoefficient = theReliabilityDomain->getCorrelationCoefficientPtr(i);
		correlation = theCorrelationCoefficient->getCorrelation();
		rv1 = theCorrelationCoefficient->getRv1();
		rv2 = theCorrelationCoefficient->getRv2();
		rv1Ptr = theReliabilityDomain->getRandomVariablePtr(rv1);
		if (rv1Ptr == 0) {
		  opserr << "FOSMAnalysis::analyze -- random variable with tag " << rv1 << " not found in domain" << endln;
		  return -1;
		}
		rv2Ptr = theReliabilityDomain->getRandomVariablePtr(rv2);
		if (rv2Ptr == 0) {
		  opserr << "FOSMAnalysis::analyze -- random variable with tag " << rv2 << " not found in domain" << endln;
		  return -1;
		}
		int i1 = rv1Ptr->getIndex();
		int i2 = rv2Ptr->getIndex();
		covariance = correlation*stdvVector(i1)*stdvVector(i2);
		covMatrix(i1,i2) = covariance;
		covMatrix(i2,i1) = covariance;
	}


	// Post-processing loop over limit-state functions
	Vector responseStdv(numLsf);
	Vector gradient(nrv);
	double responseVariance;
	lsfIter.reset();
	//for (lsf=1; lsf<=numLsf; lsf++ ) {
	while ((theLSF = lsfIter()) != 0) {

	  int j = theLSF->getIndex();
	  int lsf = theLSF->getTag();


		// Set tag of active limit-state function
		theReliabilityDomain->setTagOfActiveLimitStateFunction(lsf);


		// Extract relevant gradient
		for (i=0; i<nrv; i++) {
			gradient(i) = matrixOfGradientVectors(i,j);
		}


		// Estimate of standard deviation of response
		responseVariance = (covMatrix^gradient)^gradient;
		if (responseVariance <= 0.0) {
			opserr << "ERROR: Response variance of limit-state function number "<< lsf
				<< " is zero! " << endln;
		}
		else {
			responseStdv(j) = sqrt(responseVariance);
		}

		// Compute importance measure (dgdx*stdv)
		Vector importance(nrv);
		for (i = 0; i < nrv ; i++) {
		  importance(i) = gradient(i) * stdvVector(i);
		}
		double imptNorm = importance.Norm();
		rvIter.reset();
		for (i = 0; i < nrv ; i++)
		  importance(i) = importance(i)/imptNorm;
	
		// Print FOSM results to the output file
		outputFile << "#######################################################################" << endln;
		outputFile << "#  FOSM ANALYSIS RESULTS, LIMIT-STATE FUNCTION NUMBER   "
			<<setiosflags(ios::left)<<setprecision(1)<<setw(4)<<lsf <<"          #" << endln;
		outputFile << "#                                                                     #" << endln;
		
		outputFile << "#  Estimated mean: .................................... " 
			<<setiosflags(ios::left)<<setprecision(5)<<setw(12)<<meanEstimates(lsf-1) 
			<< "  #" << endln;
		outputFile << "#  Estimated standard deviation: ...................... " 
			<<setiosflags(ios::left)<<setprecision(5)<<setw(12)<<responseStdv(lsf-1) 
			<< "  #" << endln;
		outputFile << "#                                                                     #" << endln;
		outputFile << "#      Rvtag        Importance measure (dgdx*stdv)                    #" << endln;
		outputFile.setf( ios::scientific, ios::floatfield );
		rvIter.reset();
		//for (int i=0;  i<nrv; i++) {
		while ((aRandomVariable = rvIter()) != 0) {
		  int i = aRandomVariable->getIndex();
		  outputFile << "#       " <<setw(3)<<aRandomVariable->getTag()<<"              ";
		  outputFile << "";
		  if (importance(i) < 0.0)
		    outputFile << "-"; 
		  else
		    outputFile << " "; 
		  outputFile <<setprecision(3)<<setw(11)<<fabs(importance(i));
		  outputFile << "                                 #" << endln;
		}
		outputFile << "#                                                                     #" << endln;
		outputFile << "#######################################################################" << endln << endln << endln;
		outputFile.flush();

	}


	// Estimation of response covariance matrix
	Matrix responseCovMatrix(numLsf,numLsf);
	double responseCovariance;
	Vector gradientVector1(nrv), gradientVector2(nrv);
	for (i = 0; i < numLsf; i++) {
	  for (int k = 0; k < nrv; k++) {
	    gradientVector1(k) = matrixOfGradientVectors(k,i);
	  }
	  for (int j = i; j < numLsf; j++) {
	    for (int k = 0; k < nrv; k++) {
	      gradientVector2(k) = matrixOfGradientVectors(k,j);
	    }
	    responseCovariance = (covMatrix^gradientVector1)^gradientVector2;
	    responseCovMatrix(i,j) = responseCovariance;
	  }
	}
	for (i = 0 ; i < numLsf; i++) {
	  for (int j = 0; j < i; j++) {
	    responseCovMatrix(i,j) = responseCovMatrix(j,i);
	  }
	}


	// Corresponding correlation matrix
	Matrix correlationMatrix(numLsf,numLsf);
	for (i = 0; i < numLsf; i++) {
	  for (int j = i; j < numLsf; j++) {
	    correlationMatrix(i,j) = responseCovMatrix(i,j)/(responseStdv(i)*responseStdv(j));
	  }
	}
	for (i = 0; i < numLsf; i++) {
	  for (int j = 0; j < i; j++) {
	    correlationMatrix(i,j) = correlationMatrix(j,i);
	  }
	}

	
	// Print correlation results
	outputFile << "#######################################################################" << endln;
	outputFile << "#  RESPONSE CORRELATION COEFFICIENTS                                  #" << endln;
	outputFile << "#                                                                     #" << endln;
	if (numLsf <=1) {
		outputFile << "#  Only one limit-state function!                                     #" << endln;
	}
	else {
		outputFile << "#   gFun   gFun     Correlation                                       #" << endln;
		outputFile.setf(ios::fixed, ios::floatfield);

		LimitStateFunctionIter lsfIter2 = lsfIter;
		LimitStateFunction *theLSF2;
		lsfIter.reset();
		//for (i=0; i<numLsf; i++) {
		while ((theLSF = lsfIter()) != 0) {
		  int i = theLSF->getIndex();
		  int iTag = theLSF->getTag();
		  lsfIter2.reset();
		  //for (j=i+1; j<numLsf; j++) {
		  while ((theLSF2 = lsfIter2()) != 0) {
		    int j = theLSF2->getIndex();
		    int jTag = theLSF2->getTag();
		    //				outputFile.setf(ios::fixed, ios::floatfield);
		    outputFile << "#    " <<setw(3)<<iTag<<"    "<<setw(3)<<jTag<<"     ";
		    if (correlationMatrix(i,j)<0.0) { outputFile << "-"; }
		    else { outputFile << " "; }
		    //				outputFile.setf(ios::scientific, ios::floatfield);
		    outputFile <<setprecision(7)<<setw(11)<<fabs(correlationMatrix(i,j));
		    outputFile << "                                      #" << endln;
		  }
		}
	}
	outputFile << "#                                                                     #" << endln;
	outputFile << "#######################################################################" << endln << endln << endln;



	// Print summary of results to screen (more here!!!)
	opserr << "FOSMAnalysis completed." << endln;


	return 0;
}

