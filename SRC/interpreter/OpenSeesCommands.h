/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 1999, The Regents of the University of California    **
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
** ****************************************************************** */
                                                                        
// Written: Minjie

// Description: all opensees APIs are defined or declared here
//

#ifndef OpenSeesCommands_h
#define OpenSeesCommands_h

#include "DL_Interpreter.h"
#include <UniaxialMaterial.h>
#include <Domain.h>
#include <StaticAnalysis.h>
#include <DirectIntegrationAnalysis.h>
#include <LinearSOE.h>
#include <EigenSOE.h>
#include <DOF_Numberer.h>
#include <ConstraintHandler.h>
#include <StaticIntegrator.h>
#include <TransientIntegrator.h>
#include <EquiSolnAlgo.h>
#include <FE_Datastore.h>
#include <FEM_ObjectBrokerAllClasses.h>
#include <PFEMAnalysis.h>
#include <VariableTimeStepDirectIntegrationAnalysis.h>
#ifdef _RELIABILITY
#include <ReliabilityStaticAnalysis.h>
#include <ReliabilityDirectIntegrationAnalysis.h>
#endif
#include <Timer.h>
#include <SimulationInformation.h>

class OpenSeesCommands
{
public:

    explicit OpenSeesCommands(DL_Interpreter* interpreter);
    ~OpenSeesCommands();

    DL_Interpreter* getInterpreter();
    Domain* getDomain();
    
    int getNDF() const {return ndf;}
    void setNDF(int n) {ndf = n;}
    
    int getNDM() const {return ndm;}
    void setNDM(int n) {ndm = n;}

    void setSOE(LinearSOE* soe);
    LinearSOE* getSOE() {return theSOE;}
    
    void setNumberer(DOF_Numberer* numberer);
    DOF_Numberer* getNumberer() {return theNumberer;}
    
    void setHandler(ConstraintHandler* handler);
    ConstraintHandler* getHandler() {return theHandler;}
    
    void setStaticIntegrator(StaticIntegrator* integrator);
    StaticIntegrator* getStaticIntegrator() {return theStaticIntegrator;}
    
    void setTransientIntegrator(TransientIntegrator* integrator);
    TransientIntegrator* getTransientIntegrator() {return theTransientIntegrator;}
    
    void setAlgorithm(EquiSolnAlgo* algo);
    EquiSolnAlgo* getAlgorithm() {return theAlgorithm;}
    
    void setCTest(ConvergenceTest* test);
    ConvergenceTest* getCTest() {return theTest;}
    
    void setStaticAnalysis();
    StaticAnalysis* getStaticAnalysis() {return theStaticAnalysis;}
    
    int setPFEMAnalysis();
    PFEMAnalysis* getPFEMAnalysis() {return thePFEMAnalysis;}
    
    void setVariableAnalysis();
    VariableTimeStepDirectIntegrationAnalysis*
    getVariableAnalysis() {return theVariableTimeStepTransientAnalysis;}
    
    void setTransientAnalysis();
    DirectIntegrationAnalysis* getTransientAnalysis() {return theTransientAnalysis;}

    void setNumEigen(int num) {numEigen = num;}
    int getNumEigen() {return numEigen;}
    EigenSOE* getEigenSOE() {return theEigenSOE;}
    
    void setFileDatabase(const char* filename);
    FE_Datastore* getDatabase() {return theDatabase;}

    Timer* getTimer() {return &theTimer;}
    SimulationInformation* getSimulationInformation() {return &theSimulationInfo;}
    
#ifdef _RELIABILITY
    int setReliabilityStaticAnalysis();
    int setReliabilityTransientAnalysis();
#endif
    
    void wipeAnalysis();
    void wipe();
    int eigen(int typeSolver, double shift,
	      bool generalizedAlgo, bool findSmallest);
    
private:
    
    DL_Interpreter* interpreter;
    Domain* theDomain;
    int ndf, ndm;

    LinearSOE* theSOE;
    EigenSOE* theEigenSOE;
    DOF_Numberer* theNumberer;
    ConstraintHandler* theHandler;
    StaticIntegrator *theStaticIntegrator;
    TransientIntegrator *theTransientIntegrator;
    EquiSolnAlgo *theAlgorithm;
    StaticAnalysis* theStaticAnalysis;
    DirectIntegrationAnalysis* theTransientAnalysis;
    PFEMAnalysis* thePFEMAnalysis;
    VariableTimeStepDirectIntegrationAnalysis* theVariableTimeStepTransientAnalysis;
    AnalysisModel* theAnalysisModel;
    ConvergenceTest *theTest;

    int numEigen;
    FE_Datastore* theDatabase;
    FEM_ObjectBrokerAllClasses theBroker;
    Timer theTimer;
    SimulationInformation theSimulationInfo;

// AddingSensitivity:BEGIN /////////////////////////////////////////////
#ifdef _RELIABILITY
    SensitivityAlgorithm *theSensitivityAlgorithm;
    Integrator *theSensitivityIntegrator;
    ReliabilityStaticAnalysis* theReliabilityStaticAnalysis;
    ReliabilityDirectIntegrationAnalysis* theReliabilityTransientAnalysis;
#endif
// AddingSensitivity:END ///////////////////////////////////////////////

};

///////////////////////////////////////////////////////////////////////////
// Declaration of all OpenSees APIs except those declared in elementAPI.h//
///////////////////////////////////////////////////////////////////////////

/* OpenSeesUniaxialMaterialCommands.cpp */
int OPS_UniaxialMaterial();
int OPS_testUniaxialMaterial();
int OPS_setStrain();
int OPS_getStrain();
int OPS_getStress();
int OPS_getTangent();
int OPS_getDampTangent();
int OPS_LimitCurve();

/* OpenSeesNDMaterialCommands.cpp */
int OPS_NDMaterial();

/* OpenSeesElementCommands.cpp */
int OPS_Element();
int OPS_doBlock2D();
int OPS_doBlock3D();

/* OpenSeesTimeSeriesCommands.cpp */
int OPS_TimeSeries();

/* OpenSeesPatternCommands.cpp */
int OPS_Pattern();
int OPS_NodalLoad();
int OPS_ElementalLoad();
int OPS_SP();
int OPS_ImposedMotionSP();
int OPS_groundMotion();

/* OpenSeesSectionCommands.cpp */
int OPS_Section();
int OPS_Fiber();
int OPS_Patch();
int OPS_Layer();

/* OpenSeesCrdTransfCommands.cpp */
int OPS_CrdTransf();

/* OpenSeesBeamIntegrationCommands.cpp */
int OPS_BeamIntegration();

/* OpenSeesOutputCommands.cpp */
int OPS_nodeDisp();
int OPS_nodeReaction();
int OPS_nodeEigenvector();
int OPS_getTime();
int OPS_eleResponse();
int OPS_getLoadFactor();
int OPS_printModel();
int OPS_printModelGID();
int OPS_getCTestNorms();
int OPS_getCTestIter();
int OPS_Recorder();
int OPS_eleForce();
int OPS_eleDynamicalForce();
int OPS_nodeUnbalance();
int OPS_nodeVel();
int OPS_nodeAccel();
int OPS_nodeResponse();
int OPS_nodeCoord();
int OPS_setNodeCoord();
int OPS_updateElementDomain();
int OPS_eleNodes();
int OPS_nodeMass();
int OPS_nodePressure();
int OPS_nodeBounds();
int OPS_setPrecision();
int OPS_getEleTags();
int OPS_getNodeTags();
int OPS_getParamTags();
int OPS_getParamValue();
int OPS_sectionForce();
int OPS_sectionDeformation();
int OPS_sectionStiffness();
int OPS_sectionFlexibility();
int OPS_sectionLocation();
int OPS_sectionWeight();
int OPS_basicDeformation();
int OPS_basicForce();
int OPS_basicStiffness();
int OPS_version();
int OPS_maxOpenFiles();

/* OpenSeesMiscCommands.cpp */
int OPS_loadConst();
int OPS_calculateNodalReactions();
int OPS_rayleighDamping();
int OPS_setTime();
int OPS_removeObject();
int OPS_addNodalMass();
int OPS_buildModel();
int OPS_setNodeVel();
int OPS_setElementRayleighDampingFactors();
int OPS_MeshRegion();
int OPS_peerNGA();
int OPS_domainChange();
int OPS_stripOpenSeesXML();
int OPS_convertBinaryToText();
int OPS_convertTextToBinary();
int OPS_InitialStateAnalysis();
int OPS_RigidLink();
int OPS_RigidDiaphragm();
int OPS_addElementRayleigh();
int OPS_mesh();
int OPS_remesh();

/* OpenSeesCommands.cpp */
int OPS_wipe();
int OPS_wipeAnalysis();
int OPS_model();
int OPS_System();
int OPS_Numberer();
int OPS_ConstraintHandler();
int OPS_CTest();
int OPS_Integrator();
int OPS_Algorithm();
int OPS_Analysis();
int OPS_analyze();
int OPS_eigenAnalysis();
int OPS_resetModel();
int OPS_initializeAnalysis();
int OPS_printA();
int OPS_printB();
int OPS_Database();
int OPS_save();
int OPS_restore();
int OPS_startTimer();
int OPS_stopTimer();
int OPS_modalDamping();
int OPS_modalDampingQ();
int OPS_neesMetaData();
int OPS_neesUpload();
int OPS_totalCPU();
int OPS_solveCPU();
int OPS_accelCPU();
int OPS_numFact();
int OPS_numIter();
int OPS_systemSize();

void* OPS_KrylovNewton();
void* OPS_RaphsonNewton();
void* OPS_MillerNewton();
void* OPS_SecantNewton();
void* OPS_PeriodicNewton();
void* OPS_NewtonLineSearch();


/* Defined in its own class.cpp*/
int OPS_Node();
int OPS_HomogeneousBC();
int OPS_EqualDOF();
int OPS_EqualDOF_Mixed();
int OPS_HomogeneousBC_X();
int OPS_HomogeneousBC_Y();
int OPS_HomogeneousBC_Z();
int OPS_BackgroundMesh();
int OPS_ShallowFoundationGen();

void* OPS_TimeSeriesIntegrator();

void* OPS_BandGenLinLapack();
void* OPS_BandSPDLinLapack();
void* OPS_SuperLUSolver();
void* OPS_ProfileSPDLinDirectSolver();
void* OPS_UmfpackGenLinSolver();
void* OPS_DiagonalDirectSolver();
void* OPS_SProfileSPDLinSolver();
void* OPS_PFEMSolver();
void* OPS_PFEMCompressibleSolver();
void* OPS_PFEMSolver_Umfpack();
void* OPS_SymSparseLinSolver();
void* OPS_FullGenLinLapackSolver();

void* OPS_PlainNumberer();

void* OPS_PlainHandler();
void* OPS_PenaltyConstraintHandler();
void* OPS_LagrangeConstraintHandler();
void* OPS_TransformationConstraintHandler();

void* OPS_CTestNormUnbalance();
void* OPS_CTestNormDispIncr();
void* OPS_CTestEnergyIncr();
void* OPS_NormDispAndUnbalance();
void* OPS_NormDispOrUnbalance();
void* OPS_CTestPFEM();
void* OPS_CTestFixedNumIter();
void* OPS_CTestRelativeNormUnbalance();
void* OPS_CTestRelativeNormDispIncr();
void* OPS_CTestRelativeEnergyIncr();
void* OPS_CTestRelativeTotalNormDispIncr();

void* OPS_LoadControlIntegrator();
void* OPS_DisplacementControlIntegrator();
void* OPS_Newmark();
void* OPS_ArcLength();
void* OPS_ArcLength1();
void* OPS_HSConstraint();
void* OPS_MinUnbalDispNorm();
void* OPS_TRBDF2();
void* OPS_TRBDF3();
void* OPS_Houbolt();
void* OPS_BackwardEuler();
void* OPS_PFEMIntegrator();
void* OPS_NewmarkExplicit();
void* OPS_NewmarkHSIncrReduct();
void* OPS_NewmarkHSIncrLimit();
void* OPS_NewmarkHSFixedNumIter();
void* OPS_HHT();
void* OPS_HHT_TP();
void* OPS_HHTGeneralized();
void* OPS_HHTGeneralized_TP();
void* OPS_HHTExplicit();
void* OPS_HHTExplicit_TP();
void* OPS_HHTGeneralizedExplicit();
void* OPS_HHTGeneralizedExplicit_TP();
void* OPS_HHTHSIncrLimit();
void* OPS_HHTHSIncrLimit_TP();
void* OPS_HHTHSIncrReduct();
void* OPS_HHTHSIncrReduct_TP();
void* OPS_HHTHSFixedNumIter();
void* OPS_HHTHSFixedNumIter_TP();
void* OPS_GeneralizedAlpha();
void* OPS_KRAlphaExplicit();
void* OPS_KRAlphaExplicit_TP();
void* OPS_AlphaOS();
void* OPS_AlphaOS_TP();
void* OPS_AlphaOSGeneralized();
void* OPS_AlphaOSGeneralized_TP();
void* OPS_Collocation();
void* OPS_CollocationHSIncrReduct();
void* OPS_CollocationHSIncrLimit();
void* OPS_CollocationHSFixedNumIter();
void* OPS_Newmark1();
void* OPS_WilsonTheta();
void* OPS_CentralDifference();
void* OPS_CentralDifferenceAlternative();
void* OPS_CentralDifferenceNoDamping();

void* OPS_LinearAlgorithm();
void* OPS_NewtonRaphsonAlgorithm();
void* OPS_ModifiedNewton();
void* OPS_Broyden();
void* OPS_BFGS();


//////////////////////////////////////////////////////

// commands that changed or added:
//    pattern section block2d block3d beamIntegration
//    forceBeamColumn dispBeamColumn timoshenkoBeamColumn
//    forceBeamColumnCBDI forceBeamColumnCSBDI forceBeamColumnWarping
//    elasticForceBeamColumnWarping dispBeamColumnNL dispBeamColumnThermal
//    elasticForceBeamColumn nonlinearBeamColumn dispBeamColumnWithSensitivity
//
//    missing : recorder, video, logFile, getNP, getPI, barrier, send, recv, partition,
//              record, defaultUnits, setParameter, reliability, wipeReliability,
//              parameter, addToParameter, updateParameter, FiberThermal, FiberInt,
//              UCFiber, TclModelBuilderYS_SectionCommand, yieldSurface_BC,
//              ysEvolutionModel, plasticMaterial, cyclicModel, damageModel,
//              FirePattern, PySimple1Gen, TzSimple1Gen, Hfiber, frictionModel,
//              stiffnessDegradation, unloadingRule, strengthDegradation,
//              hystereticBackbone, updateMaterialStage, updateMaterials,
//              loadPackage
#endif
