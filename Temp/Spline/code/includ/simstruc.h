/* 
 * Copyright (c) 1990-2002 The MathWorks, Inc. 
 * 
 * File: simstruc_types.h     $Revision: 1.35 $ 
 * 
 * Abstract: 
 *   The embedded RTW code formats do not include simstruc.h, but 
 *   needs these common types. 
 *  
 */  
  
#ifndef __SIMSTRUC_TYPES_H__  
#define __SIMSTRUC_TYPES_H__  
  
#include "tmwtypes.h"  
  
#define UNUSED_ARG(arg)  (void)arg  
  
#ifndef fcn_call_T  
# define fcn_call_T real_T  
#endif  
  
#ifndef action_T  
# define action_T real_T  
#endif  
  
/* 
 * UNUSED_PARAMETER(x) 
 *   Used to specify that a function parameter (argument) is required but not 
 *   accessed by the function body. 
 */  
#ifndef UNUSED_PARAMETER  
# if defined(__LCC__)  
#   define UNUSED_PARAMETER(x)  /* do nothing */  
# else  
/* 
 * This is the semi-ANSI standard way of indicating that a 
 * unused function parameter is required. 
 */  
#   define UNUSED_PARAMETER(x) (void) (x)  
# endif  
#endif  
    
typedef enum {  
  SS_SIMMODE_NORMAL,            /* Running a "normal" Simulink simulation     */  
  SS_SIMMODE_SIZES_CALL_ONLY,   /* Block edit eval to obtain number of ports  */  
  SS_SIMMODE_RTWGEN,            /* Generating code                            */  
  SS_SIMMODE_EXTERNAL          /* External mode simulation                   */  
} SS_SimMode;  
  
/* Must be used when SS_SimMode is SS_SIMMODE_RTWGEN */  
typedef enum {  
    SS_RTWGEN_UNKNOWN,  
    SS_RTWGEN_RTW_CODE,           /* Code generation for RTW */  
    SS_RTWGEN_ACCELERATOR         /* Code generation for accelerator */  
} RTWGenMode;  
  
  
typedef enum {  
    SOLVER_MODE_AUTO,          /* only occurs in 
                                  mdlInitializeSizes/mdlInitializeSampleTimes */  
    SOLVER_MODE_SINGLETASKING,  
    SOLVER_MODE_MULTITASKING  
} SolverMode;  
  
typedef enum {  
    MINOR_TIME_STEP,  
    MAJOR_TIME_STEP  
} SimTimeStep;  
  
typedef enum {                  /* Disabled/enabled modes */  
    SUBSYS_DISABLED = 0,  
    SUBSYS_ENABLED = 1,  
    SUBSYS_BECOMING_DISABLED = 2,  
    SUBSYS_BECOMING_ENABLED  = 3  
} EnableStates;  
  
typedef enum {                  /* What direction to look for zero crossing */  
    FALLING_ZERO_CROSSING  = -1,  
    ANY_ZERO_CROSSING      = 0,  
    RISING_ZERO_CROSSING   = 1  
} ZCDirection;  
  
typedef enum {  
    NEG_ZCSIG              = -1,  
    ZERO_ZCSIG             = 0,  
    POS_ZCSIG              = 1,  
    ZERO_RISING_EV_ZCSIG   = 100, /* zero and had a rising event  */  
    ZERO_FALLING_EV_ZCSIG  = 101, /* zero and had a falling event */  
    UNINITIALIZED_ZCSIG    = INT_MAX  
} ZCSigState;  
  
typedef enum {                  /* What type of zero crossing event occurred */  
    FALLING_ZCEVENT        = -1,  
    NO_ZCEVENT             = 0,  
    RISING_ZCEVENT         = 1  
} ZCEventType;  
  
typedef enum {  
    SS_DOUBLE  =  0,    /* real_T    */  
    SS_SINGLE  =  1,    /* real32_T  */  
    SS_INT8    =  2,    /* int8_T    */  
    SS_UINT8   =  3,    /* uint8_T   */  
    SS_INT16   =  4,    /* int16_T   */  
    SS_UINT16  =  5,    /* uint16_T  */  
    SS_INT32   =  6,    /* int32_T   */  
    SS_UINT32  =  7,    /* uint32_T  */  
    SS_BOOLEAN =  8     /* boolean_T */  
  
    /* if last in list changes also update define below */  
  
} BuiltInDTypeId;  
  
#define SS_NUM_BUILT_IN_DTYPE ((int)SS_BOOLEAN+1)  
  
typedef enum {  
  
    SS_INTEGER = 10,  
    SS_POINTER = 11  
  
    /* if last in list changes also update define below */  
  
} PreDefinedDTypeId;  
  
#define SS_NUM_PREDEFINED_DTYPES  2   
  
  
#ifndef _DTYPEID  
#  define _DTYPEID  
   typedef int_T DTypeId;  
#endif  
  
typedef int_T CSignal_T;  
  
/* DimsInfo_T structure is for S-functions */  
#ifndef _DIMSINFO_T   
#define _DIMSINFO_T   
struct DimsInfo_tag{  
    int                        width;        /* number of elements    */  
    int                        numDims;      /* number of dimensions  */  
    int                        *dims;        /* dimensions            */  
    struct DimsInfo_tag        *nextSigDims; /* for composite signals */  
};  
  
typedef struct DimsInfo_tag DimsInfo_T;  
  
  
/* 
 * DECL_AND_INIT_DIMSINFO(variableName): 
 *    Macro for setting up a DimsInfo in an S-function to DYNAMIC_DIMENSION. 
 *    This macro must be placed at the start of a deceleration, for example: 
 *    
 *           static void mdlInitializeSizes(SimStruct *S) 
 *           { 
 *               DECL_AND_INIT_DIMSINFO(diminfo1); 
 *            
 *               ssSetNumSFcnParams(S, 0); 
 *                
 *           } 
 *    
 *    The reason that this macro must be placed in the deceleration section of a 
 *    function or other scope is that this macro **creates** a local variable of 
 *    the specified name with type DimsInfo_T. The variable is initialized 
 *    to DYNAMIC_DIMENSION. 
 */  
#define DECL_AND_INIT_DIMSINFO(variableName) \  
   DimsInfo_T variableName = {-1,-1,NULL,NULL}  
#endif /* _DIMSINFO_T */  
  
  
/* 
 * Enumeration of work vector used as flag values. 
 */  
typedef enum {  
    SS_DWORK_USED_AS_DWORK  = 0,  /* default */  
    SS_DWORK_USED_AS_DSTATE,      /* will be logged, loaded, etc */  
    SS_DWORK_USED_AS_SCRATCH      /* will be reused */  
} ssDWorkUsageType;  
  
#define SS_NUM_DWORK_USAGE_TYPES 3  
  
/* 
 * DWork structure for S-Functions, one for each dwork. 
 */  
struct _ssDWorkRecord {  
    int_T            width;  
    DTypeId          dataTypeId;  
    CSignal_T        complexSignal;  
    void             *array;  
    const char_T     *name;  
    ssDWorkUsageType usedAs;  
};  
  
/* 
 * INHERITED_SAMPLE_TIME      - Specify for blocks that inherit their sample 
 *                              time from the block that feeds their input. 
 * 
 * CONTINUOUS_SAMPLE_TIME     - A continuous sample time indicates that the 
 *                              block executes every simulation step. 
 * 
 * VARIABLE_SAMPLE_TIME       - Specifies that this sample time is discrete 
 *                              with a varying period. 
 * 
 * FIXED_IN_MINOR_STEP_OFFSET - This can be specified for the offset of either 
 *                              the inherited or continuous sample time 
 *                              indicating that the output does not change 
 *                              in minor steps. 
 */  
  
#define INHERITED_SAMPLE_TIME      ((real_T)-1.0)  
#define CONTINUOUS_SAMPLE_TIME     ((real_T)0.0)  
#define VARIABLE_SAMPLE_TIME       ((real_T)-2.0)  
#define MODEL_EVENT_SAMPLE_TIME    ((real_T)-3.0)  
#define FIXED_IN_MINOR_STEP_OFFSET ((real_T)1.0)  
  
/* 
 * The RTWLogSignalInfo and RTWLogInfo structures are for use by 
 * the Real-Time Workshop and should not be used by S-functions. 
 */  
typedef const int8_T * const * LogSignalPtrsType;  
  
typedef struct RTWLogSignalInfo_tag {  
  int_T                numSignals;  
  const int_T          *numCols;  
  const int_T          *numDims;  
  const int_T          *dims;  
  const BuiltInDTypeId *dataTypes;  
  const int_T          *complexSignals;  
  const int_T          *frameData;  
  const char_T         *labels;  
  const int_T          *labelLengths;  
  const char_T         *titles;  
  const int_T          *titleLengths;  
  const int_T          *plotStyles;  
  const char_T         *blockNames;  
  const int_T          *blockNameLengths;  
  
} RTWLogSignalInfo;  
  
/* ============================================================================= 
 * Logging object 
 * ============================================================================= 
 */  
typedef struct _RTWLogInfo_tag {  
  void              *logInfo;      /* Pointer to a book keeping structure    * 
                                    * used in rtwlog.c                       */      
  
  LogSignalPtrsType logXSignalPtrs;/* Pointers to the memory location        * 
                                    * of the data to be logged into the      * 
                                    * states structure. Not used if logging  * 
                                    * data in matrix format.                 */  
  
  LogSignalPtrsType logYSignalPtrs;/* Pointers to the memory location        * 
                                    * of the data to be logged into the      * 
                                    * outputs structure. Not used if logging * 
                                    * data in matrix format.                 */  
  int_T         logFormat;          /* matrix=0, struct=1, or strut_wo_time=2 */  
  
  int_T         logMaxRows;         /* Max number of rows (0 for no limit)    */  
  int_T         logDecimation;      /* Data logging interval                  */  
  
  const char_T  *logVarNameModifier;/* pre(post)fix string modifier for the   * 
                                     * log variable names                     */  
  
  const char_T  *logT;              /* Name of variable to log time           */  
  const char_T  *logX;              /* Name of variable to log states         */  
  const char_T  *logXFinal;         /* Name of varaible to log final state    */  
  const char_T  *logY;              /* Name of variable(s) to log outputs     */  
  
  const RTWLogSignalInfo *logXSignalInfo;/* Info about the states             */  
  const RTWLogSignalInfo *logYSignalInfo;/* Info about the outptus            */  
  
  void (*mdlLogData)(void *rtli, void *tPtr);  
  
} RTWLogInfo;  
  
/* Macros associated with RTWLogInfo */  
#define rtliGetLogInfo(rtli)     ((LogInfo*)(rtli)->logInfo)  
#define rtliSetLogInfo(rtli,ptr) (rtli)->logInfo = ((void *)ptr)  
  
#define rtliGetLogFormat(rtli)   (rtli)->logFormat  
#define rtliSetLogFormat(rtli,f) (rtli)->logFormat = (f)  
  
#define rtliGetLogVarNameModifier(rtli)      (rtli)->logVarNameModifier  
#define rtliSetLogVarNameModifier(rtli,name) (rtli)->logVarNameModifier=(name)  
  
#define rtliGetLogMaxRows(rtli)     (rtli)->logMaxRows  
#define rtliSetLogMaxRows(rtli,num) (rtli)->logMaxRows = (num)  
  
#define rtliGetLogDecimation(rtli)   (rtli)->logDecimation  
#define rtliSetLogDecimation(rtli,l) (rtli)->logDecimation = (l)  
  
#define rtliGetLogT(rtli)     (rtli)->logT  
#define rtliSetLogT(rtli,lt)  (rtli)->logT = (lt)  
  
#define rtliGetLogX(rtli)     (rtli)->logX  
#define rtliSetLogX(rtli,lx)  (rtli)->logX = (lx)  

#define rtliGetLogY(rtli)     (rtli)->logY  
#define rtliSetLogY(rtli,ly)  (rtli)->logY = (ly)  
  
#define rtliGetLogXFinal(rtli)     (rtli)->logXFinal  
#define rtliSetLogXFinal(rtli,lxf) (rtli)->logXFinal = (lxf)  
  
#define rtliGetLogXSignalInfo(rtli)     (rtli)->logXSignalInfo  
#define rtliSetLogXSignalInfo(rtli,lxi) (rtli)->logXSignalInfo = (lxi)  
                                                
#define rtliGetLogYSignalInfo(rtli)     (rtli)->logYSignalInfo  
#define rtliSetLogYSignalInfo(rtli,lyi) (rtli)->logYSignalInfo = (lyi)  
  
#define rtliGetLogXSignalPtrs(rtli)     (rtli)->logXSignalPtrs  
#define rtliSetLogXSignalPtrs(rtli,lxp) (rtli)->logXSignalPtrs = (lxp)  
  
#define rtliGetLogYSignalPtrs(rtli)     (rtli)->logYSignalPtrs  
#define rtliSetLogYSignalPtrs(rtli,lyp) (rtli)->logYSignalPtrs = (lyp)  
/* ========================================================================== */  
  
/* ============================================================================= 
 * External mode object 
 * ============================================================================= 
 */  
typedef struct _RTWExtModeInfo_tag {  
  
    void *subSysModeVectorAddr;    /* Array of addresses points to 
                                    * the mode vector slots for enabled  
                                    * sub-systems. Enabled sub-systems store 
                                    * information about their enabled state  
                                    * in thier mode vector. 
                                    */  
    uint32_T *checksumsPtr;        /* Pointer to the model's checksums array 
                                    */  
    const void **mdlMappingInfoPtr;/* Pointer to the model's mapping info 
                                    * pointer 
                                    */  
    void       *tPtr;              /* Copy of model's time pointer 
                                    */  
} RTWExtModeInfo;  
  
#define rteiSetSubSystemModeVectorAddresses(E,addr) \  
        (E)->subSysModeVectorAddr = ((void *)addr)  
#define rteiGetSubSystemModeVectorAddresses(E) \  
        (E)->subSysModeVectorAddr  
#define rteiGetAddrOfSubSystemModeVector(E,idx) \  
        ((int_T*)((int8_T**)((E)->subSysModeVectorAddr))[idx])  
  
#define rteiSetModelMappingInfoPtr(E,mip) \  
        (E)->mdlMappingInfoPtr = (mip)  
#define rteiGetModelMappingInfo(E) (*((E)->mdlMappingInfoPtr))  
  
#define rteiSetChecksumsPtr(E,cp) \  
        (E)->checksumsPtr = (cp)  
#define rteiGetChecksum0(E) (E)->checksumsPtr[0]  
#define rteiGetChecksum1(E) (E)->checksumsPtr[1]  
#define rteiGetChecksum2(E) (E)->checksumsPtr[2]  
#define rteiGetChecksum3(E) (E)->checksumsPtr[3]  
  
#define rteiSetTPtr(E,p) (E)->tPtr = (p)  
#define rteiGetT(E)      ((time_T *)(E)->tPtr)[0]  
  
/* ========================================================================== */  
  
/* ============================================================================= 
 * Model methods object 
 * ============================================================================= 
 */  
typedef void (*rtMdlInitializeSizesFcn)(void *rtModel);  
typedef void (*rtMdlInitializeSampleTimesFcn)(void *rtModel);  
typedef void (*rtMdlStartFcn)(void *rtModel);  
typedef void (*rtMdlOutputsFcn)(void *rtModel, int_T tid);  
typedef void (*rtMdlUpdateFcn)(void *rtModel, int_T tid);  
typedef void (*rtMdlDerivativesFcn)(void *rtModel);  
typedef void (*rtMdlProjectionFcn)(void *rtModel);  
typedef void (*rtMdlTerminateFcn)(void *rtModel);  
  
typedef struct _RTWRTModelMethodsInfo_tag {  
    void                          *rtModelPtr;  
    rtMdlInitializeSizesFcn       rtmInitSizesFcn;  
    rtMdlInitializeSampleTimesFcn rtmInitSampTimesFcn;  
    rtMdlStartFcn                 rtmStartFcn;  
    rtMdlOutputsFcn               rtmOutputsFcn;  
    rtMdlUpdateFcn                rtmUpdateFcn;  
    rtMdlDerivativesFcn           rtmDervisFcn;  
    rtMdlProjectionFcn            rtmProjectionFcn;  
    rtMdlTerminateFcn             rtmTerminateFcn;  
} RTWRTModelMethodsInfo;  
  
#define rtmiSetRTModelPtr(M,rtmp) (M).rtModelPtr = (rtmp)  
#define rtmiGetRTModelPtr(M)      (M).rtModelPtr  
  
#define rtmiSetInitSizesFcn(M,fp) \  
  (M).rtmInitSizesFcn = ((rtMdlInitializeSizesFcn)(fp))  
#define rtmiSetInitSampTimesFcn(M,fp) \  
  (M).rtmInitSampTimesFcn = ((rtMdlInitializeSampleTimesFcn)(fp))  
#define rtmiSetStartFcn(M,fp) \  
  (M).rtmStartFcn = ((rtMdlStartFcn)(fp))  
#define rtmiSetOutputsFcn(M,fp) \  
  (M).rtmOutputsFcn = ((rtMdlOutputsFcn)(fp))  
#define rtmiSetUpdateFcn(M,fp) \  
  (M).rtmUpdateFcn = ((rtMdlUpdateFcn)(fp))  
#define rtmiSetDervisFcn(M,fp) \  
  (M).rtmDervisFcn = ((rtMdlDerivativesFcn)(fp))  
#define rtmiSetProjectionFcn(M,fp) \  
  (M).rtmProjectionFcn = ((rtMdlProjectionFcn)(fp))  
#define rtmiSetTerminateFcn(M,fp) \  
  (M).rtmTerminateFcn = ((rtMdlTerminateFcn)(fp))  
  
#define rtmiInitializeSizes(M) \  
         (*(M).rtmInitSizesFcn)((M).rtModelPtr)  
#define rtmiInitializeSampleTimes(M) \  
         (*(M).rtmInitSampTimesFcn)((M).rtModelPtr)  
#define rtmiStart(M) \  
         (*(M).rtmStartFcn)((M).rtModelPtr)  
#define rtmiOutputs(M, tid) \  
         (*(M).rtmOutputsFcn)((M).rtModelPtr,tid)  
#define rtmiUpdate(M, tid) \  
        (*(M).rtmUpdateFcn)((M).rtModelPtr,tid)  
#define rtmiDerivatives(M) \  
         (*(M).rtmDervisFcn)((M).rtModelPtr)  
#define rtmiProjection(M) \  
         (*(M).rtmProjectionFcn)((M).rtModelPtr)  
#define rtmiTerminate(M) \  
         (*(M).rtmTerminateFcn)((M).rtModelPtr)  
  
/* ========================================================================== */  
  
/* ============================================================================= 
 * Solver object 
 * ============================================================================= 
 */  
#ifndef NO_FLOATS /* Needed because ERT code may be integer-only */  
  
typedef struct _RTWSolverInfo_tag {  
    void        *rtModelPtr;  
  
    SimTimeStep *simTimeStepPtr;  
    void        *solverData;  
    const char  *solverName;  
    boolean_T   isVariableStepSolver;  
    boolean_T   solverNeedsReset;  
    SolverMode  solverMode;  
  
    time_T      solverStopTime;  
    time_T      *stepSizePtr;  
    time_T      minStepSize;  
    time_T      maxStepSize;  
    time_T      fixedStepSize;  
  
    int_T       maxNumMinSteps;  
    int_T       solverMaxOrder;  
  
    int_T       solverRefineFactor;  
    real_T      solverRelTol;  
    real_T      solverAbsTol;  
      
    real_T      **dXPtr;  
    time_T      **tPtr;  
  
    int_T       *numContStatesPtr;  
    real_T      **contStatesPtr;  
      
    const char_T **errStatusPtr;  
  
    RTWRTModelMethodsInfo *modelMethodsPtr;  
} RTWSolverInfo;  
  
#define rtsiSetRTModelPtr(S,rtmp) (S)->rtModelPtr = (rtmp)  
#define rtsiGetRTModelPtr(S)      (S)->rtModelPtr  
  
#define rtsiSetSimTimeStepPtr(S,stp) (S)->simTimeStepPtr = (stp)  
#define rtsiGetSimTimeStep(S)        *((S)->simTimeStepPtr)  
#define rtsiSetSimTimeStep(S,st)     *((S)->simTimeStepPtr) = (st)  
  
#define rtsiSetSolverData(S,sd) (S)->solverData = (sd)  
#define rtsiGetSolverData(S)    (S)->solverData  
  
#define rtsiSetSolverName(S,sn) (S)->solverName = (sn)  
#define rtsiGetSolverName(S)    (S)->solverName  
  
#define rtsiSetVariableStepSolver(S,vs) (S)->isVariableStepSolver = (vs)  
#define rtsiIsVariableStepSolver(S)     (S)->isVariableStepSolver  
  
#define rtsiSetSolverNeedsReset(S,sn) (S)->solverNeedsReset = (sn)  
#define rtsiGetSolverNeedsReset(S)    (S)->solverNeedsReset  
  
#define rtsiSetSolverMode(S,sm) (S)->solverMode = (sm)  
#define rtsiGetSolverMode(S)    (S)->solverMode  
  
#define rtsiSetSolverStopTime(S,st) (S)->solverStopTime = (st)  
#define rtsiGetSolverStopTime(S)    (S)->solverStopTime  
  
#define rtsiSetStepSizePtr(S,ssp) (S)->stepSizePtr = (ssp)  
#define rtsiSetStepSize(S,ss)     *((S)->stepSizePtr) = (ss)  
#define rtsiGetStepSize(S)        *((S)->stepSizePtr)  
  
#define rtsiSetMinStepSize(S,ss) (S)->minStepSize = (ss)  
#define rtsiGetMinStepSize(S)    (S)->minStepSize  
  
#define rtsiSetMaxStepSize(S,ss) (S)->maxStepSize = (ss)  
#define rtsiGetMaxStepSize(S)    (S)->maxStepSize  
  
#define rtsiSetFixedStepSize(S,ss) (S)->fixedStepSize = (ss)  
#define rtsiGetFixedStepSize(S)    (S)->fixedStepSize  
  
#define rtsiSetMaxNumMinSteps(S,mns) (S)->maxNumMinSteps = (mns)  
#define rtsiGetMaxNumMinSteps(S)     (S)->maxNumMinSteps  
  
#define rtsiSetSolverMaxOrder(S,smo) (S)->solverMaxOrder = (smo)  
#define rtsiGetSolverMaxOrder(S)     (S)->solverMaxOrder  
  
#define rtsiSetSolverRefineFactor(S,smo) (S)->solverRefineFactor = (smo)  
#define rtsiGetSolverRefineFactor(S)     (S)->solverRefineFactor  
  
#define rtsiSetSolverRelTol(S,smo) (S)->solverRelTol = (smo)  
#define rtsiGetSolverRelTol(S)     (S)->solverRelTol  
  
#define rtsiSetSolverAbsTol(S,smo) (S)->solverAbsTol = (smo)  
#define rtsiGetSolverAbsTol(S)     (S)->solverAbsTol  
  
#define rtsiSetdXPtr(S,dxp) (S)->dXPtr = (dxp)  
#define rtsiSetdX(S,dx)     *((S)->dXPtr) = (dx)  
#define rtsiGetdX(S)        *((S)->dXPtr)  
  
#define rtsiSetTPtr(S,tp) (S)->tPtr = (tp)  
#define rtsiSetT(S,t)     (*((S)->tPtr))[0] = (t)  
#define rtsiGetT(S)       (*((S)->tPtr))[0]  
  
#define rtsiSetContStatesPtr(S,cp) (S)->contStatesPtr = (cp)  
#define rtsiGetContStates(S)       *((S)->contStatesPtr)  
  
#define rtsiSetNumContStatesPtr(S,cp) (S)->numContStatesPtr = (cp)  
#define rtsiGetNumContStates(S)       *((S)->numContStatesPtr)  
  
#define rtsiSetErrorStatusPtr(S,esp) (S)->errStatusPtr = (esp)  
#define rtsiSetErrorStatus(S,es) *((S)->errStatusPtr) = (es)  
#define rtsiGetErrorStatus(S)    *((S)->errStatusPtr)  
  
#define rtsiSetModelMethodsPtr(S,mmp) (S)->modelMethodsPtr = (mmp)  
#define rtsiGetModelMethodsPtr(S)     (S)->modelMethodsPtr  
  
#endif /* !NO_FLOATS */  
  
/* ========================================================================== */  
  
/*  
 * Lightweight structure for holding a real, sparse matrix 
 * as used by the analytical Jacobian methods.   
 */  
typedef struct SparseHeader_Tag {  
    int_T   mRows;                  /* number of rows   */  
    int_T   nCols;                  /* number of cols   */  
    int_T   nzMax;                  /* size of *pr, *ir */  
    int_T   *Ir;                    /* row indices      */  
    int_T   *Jc;                    /* column offsets   */  
#ifndef NO_FLOATS  
    real_T  *Pr;                    /* nonzero entries  */  
#else  
    void    *Pr;  
#endif  
} SparseHeader;  
  
/*========================* 
 * Setup for multitasking * 
 *========================*/  
  
/* 
 * Let MT be synonym for MULTITASKING (to shorten command line for DOS) 
 */  
#if defined(MT)  
# if MT == 0  
#   undef MT  
# else  
#   define MULTITASKING 1  
# endif  
#endif  
  
#if defined(MULTITASKING) && MULTITASKING == 0  
# undef MULTITASKING  
#endif  
  
#if defined(MULTITASKING) && !defined(TID01EQ)  
# define TID01EQ 0  
#endif  
  
/* 
 * Undefine MULTITASKING if there is only one task or there are two 
 * tasks and the sample times are equal (case of continuous and one discrete 
 * with equal rates). 
 */  
#if defined(NUMST) && defined(MULTITASKING)  
# if NUMST == 1 || (NUMST == 2 && TID01EQ == 1)  
#  undef MULTITASKING  
# endif  
#endif  
  
#endif /* __SIMSTRUC_TYPES_H__ */  
                  