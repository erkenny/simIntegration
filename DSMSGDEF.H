/*******************************************************************************
*
*  FILE
*    DSMSGDEF.H
*
*  RELATED FILES
*    many
*
*  DESCRIPTION
*    Global definitions for message module
*
*  REMARKS
*
*  AUTHOR(S)
*    V. Lang, S. Hoener, R. Leinfellner
*
*  dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
*  $RCSFile: DSMSGDEF.H $ $Revision: 1.1 $ $Date: 2006/09/13 12:52:53GMT+01:00 $
*******************************************************************************/

#ifndef __DSMSGDEF_H__
#define __DSMSGDEF_H__

/* version 1.0                                                              */
/* errorhead configuration                                                  */
/*+--------------------------------------------------------------------+    */
/*¦Version ¦ Class  ¦ Dialog ¦C¦      SubModul         ¦   MainModul   ¦    */
/*¦ 4 Bit  ¦ 4 Bit  ¦ 4 Bit  ¦1¦       11 Bit          ¦    8 Bit      ¦    */
/*+--------------------------------------------------------------------+    */
/*31      +27      +23      +19+18                    +7              +0    */


#define OFFSET_SUBMODUL                                 8
#define OFFSET_DLL                                      19
#define OFFSET_DIALOG                                   20
#define OFFSET_CLASS                                    24
#define OFFSET_VERSION                                  28


#define VERSION_1                                       1

/* This enum describs the severity of a message. The severity is used for
 error handling and to generate special information and formats to the log file
 , log viewer and message boxes*/
enum DSERR_ClassEnum
{
        /* The message is only for support and development. It will be written
         to the log file, only*/
        DSERR_CLASS_LOG,
        /* The message is a state/text message. It will be written
         to the log file and the log viewer*/
        DSERR_CLASS_TEXT,
        /* The message is a warning message. It will be written
         to the log file and the log viewer. It is formated as warning*/
        DSERR_CLASS_WARN,
        /* The message is a error message. It will be written
         to the log file and the log viewer. It is formated as error*/
        DSERR_CLASS_ERROR,
        /* The message is a severe error message. It will be written
         to the log file and the log viewer. It is formated as severe error*/
        DSERR_CLASS_SEVERE,
        /* The message is a system error message. It will be written
         to the log file and the log viewer. It is formated as system error.*/
        DSERR_CLASS_SYSTEM
};

/* This enum lays down wether and how the message is dislayed in an messagebox.
there is no dependancy to DSERR_ClassEnum*/
enum DSERR_DialogEnum 
{
        /*No message box will be displayed */
        DSERR_DLG_NOBOX,
        /*A message box with an OK button will be displayed */
        DSERR_DLG_OK,
        /*A message box with an OK and a Canncel button will be displayed */
        DSERR_DLG_OKCAN,
        /*A message box with a Yes and a No button will be displayed */
        DSERR_DLG_YESNO,
        /*A message box with a Yes, No and Canncel button will be displayed */
        DSERR_DLG_YESNOCAN,
        /*A message box with a Retry, Ignore and Abort button will be displayed */
        DSERR_DLG_RETIGNAB,
        /*A message box with a Close button will be displayed */
        DSERR_DLG_CLOSE,
		/*A message box with a Retry and a Cancel button will be displayed */
		DSERR_DLG_RETRYCANCEL
};

/* Use this define to load the message string from the commonerr.dll*/ 
#define COMMON_DLL                                      0
/* Use this define to load the message string from the module dll*/
#define MODUL_DLL                                       1

/* new defines added by SaschaH											*/
/* Use this define to specify that a new stack should be created*/
#define DS_NEW_STACK									NULL
/* Use this define to initialize a new stack id*/
#define DS_NO_STACK										0

/* All main module nummbers are defined here. Do not change any values*/
enum DSERR_MainModEnum
{
    DS_DEFAULT,                     /* 0 = default                      */
    DS_USER_DEF,                    /* 1 = User defined                 */
	DS_COMMON_MODULES,				/* 2 = MainModule number for CommonModules */
	//DS_COCKPIT,                     /* 2 = cockpit                     */
    DS_TRACE,                       /* 3 = trace                        */
    DS_GUIMON,                      /* 4 = guimon                       */
    DS_GMSERVER,                    /* 5 = guimon IServer               */
	DS_DEVICE_MANAGER,				/* 6 = Device Manager				*/
    DS_LD_1103=10,                  /* 10 = loader for 1103             */
    DS_WORKBENCH=20,                /* 20 = workbench                   */
    DS_APPLICATION_SVR_MNG=21,      /* 21 = application server manager  */
    DS_LAYOUT_COMP=22,              /* 22 = layout component            */
    DS_TEST_AUTOMATION=23,          /* 23 = Test Automation             */
    DS_SKRIPT_EDITOR= 24,           /* 24 = Skript editor               */
    DS_SEQUENCE_EDITOR=25,          /* 25 = sequence editor             */
    DS_HIERARCHY_BROWSER=26,        /* 26 = hierarchy browser           */
    DS_EXPERIMENT=27,               /* 27 = experiment component        */
    DS_TRC_PARSER=28,               /* 28 = trace file parser           */
    DS_PYTHON_INTERP=29,            /* 29 = pyton interpreter           */
    DS_REFERENCE_DATA=30,           /* 30 = reference data              */
    DS_VISUALISATION=31,           	/* 31 = the "new RealMotion" comp.	*/
    DS_VRTOOLCHAIN=32,           	/* 32 = the 3D-Desk ToolChain	    */
	DS_FAULTSIM	= 33,				/* 33 = Fault Simulation Component	*/
	DS_TAPROJECTMANAGER	= 34,		/* 34 = TAProjectManager Component	*/
	DS_TATESTEDITOR	= 35,			/* 35 = TATestEditor Component		*/
	DS_MTEST = 36,					/* 36 = MTest                       */
	DS_MATLAB_ENGINE = 37,  		/* 37 = Matlab Engine 				*/
    DS_VARIABLE_MANAGEMENT = 40,    /* 40 = Variable Component          */
    DS_MEASURE_DATA_MANAGEMENT = 41,/* 41 = Measurement component       */
    DS_PRJ_EXP_MANAGEMENT = 42,     /* 42 = Project/Experiment component*/
	DS_PARAMETER_MAMAGEMENT = 43,   /* 43 = Parametermanagement component*/
	DS_LOG_VIEWER = 44,   			/* 44 = Log Viewer component        */
	DS_USER_MANAGER = 45,   		/* 45 = User Manager component      */
	DS_MEASURED_DATA_NAV = 46,      /* 46 = MeasuredDataNavigator component */
	DS_GLOBAL_STATE_MANAGER = 47,	/* 47 = GlobalStateManager component*/
	DS_ECUDIAGNOSTIC = 48,			/* 48 = ECUDiagnostic Component		*/
	DS_X_DESK = 49,					/* 49 = XDesk frame					*/
	DS_TESTAUTOMATION_OBJECT_MODELL = 50,		/* 50 = TAM	TESTAUTOMATION_OBJECT_MODELL*/
	DS_PYTHON_ERROR = 51,			/* 51 = generic Python Wrapper	*/
	DS_ASAP3_SERVER			= 52,	/* 52 = ASAP3 server */
	DS_ML_CON_CREATOR       = 53,	/* 53 = MATLAB connection creator */
	DS_GEN_ML_CONNECTION	= 54,	/* 54 = Generic MATLAB connection */
	DS_ML_CONNECTION		= 55,	/* 55 = MATLAB connection for ConfigurationDesk */
	DS_ASAP3_MANAGER		= 56,	/* 56 = ASAP3 Manager */
	DS_LAYOUTER_CAL			= 57,	/* 57 = CalDesk Layouter */
	DS_A2L_EDITOR			= 58,	/* 58 = A2L-Editor  */
	DS_LIBRARY_MANAGER		= 59,	/* 59 = Library Manager */
    DS_CONTROLS=60,                 /* 60 = for all Controls            */
	DS_MDLDGUI_MANAGER = 61,		/* 61 = MDLDGUIManager           */
	DS_MDLD_PARAMETERIZATION = 62,	/* 62 = DS_MDLD_PARAMETERIZATION */	
	DS_ROADGENERATOR = 63,			/* 63 = DS_ROADGENERATOR */
	DS_MANEUVEREDITOR = 64,			/* 64 = DS_MANEUVEREDITOR */
	DS_MDLD_PROJECTMANAGER = 65,	/* 65 = DS_MDLD_PROJECTMANAGER */
	DS_CONTROL_MANAGER = 66,		/* 66 = DS_CONTROL_MANAGER AD*/
	DS_SEQUENCE_BUILDER	= 67,		/* 67 = DS_SEQUENCE_BUILDER AD */
    DS_GENERIC_CAN_API = 70,        /* 70 = Generic CAN API   */
	DS_BUS_MNGR	= 71,				/* 71 = BUSManager */
	DS_RTT_HOST_SCRIPT_MANAGER = 72,  /* host script manager for real time testing*/
	DS_SYSTEMDESKAPPLICATION = 90,	/* 90 = SystemDesk Application */
	DS_PROGRAMPLATFORM	= 91,		/* 91 = ProgramPlatform */
	DS_PERSISTENCEMANAGER	= 92,	/* 92 = PersistenceManager */
	DS_PROJECTMANAGEMENT	= 93,	/* 93 = ProjectManagement */
	DS_SWSYSTEMMODELLER	= 94,		/* 94 = SwSystemModeller */
	DS_SYSTEMMODELLER	= 95,		/* 95 = SystemModeller */
	DS_RTEGENERATOR	= 96,			/* 96 = RteGenerator */
	DS_ECUCONFIGURATION	= 97,		/* 97 = EcuConfiguration */
	DS_TOOLINTEGRATION	= 98,		/* 98 = ToolIntegration */
	DS_ASTERIXSIMULATION = 99,		/* 99 = Asterix Simulation */
	DS_ASTERIXLIBRARIES	= 100,		/* 100 = AsterixLibraries */
	DS_ASTERIXENTITIES	= 101,		/* 101 = AsterixEntities */
	DS_SYSTEMDESKAUTOMATION	= 102,	/* 102 = SystemDesk Automation */
    DS_1003_ERR_MOD=128,            /* 128 = message module for DS1003  */
    DS_1004_ERR_MOD=129,            /* 129 = message module for DS1004  */
    DS_1103_ERR_MOD=130,            /* 130 = message module for DS1103  */
    DS_1401_ERR_MOD=131,            /* 131 = message module for DS1401  */
    DS_1102_ERR_MOD=132,            /* 132 = message module for DS1102  */
    DS_1005_ERR_MOD=133,            /* 133 = message module for DS1005  */
    DS_1104_ERR_MOD=134,            /* 134 = message module for DS1104  */
    DS_1006_ERR_MOD=135,             /* 135 = message module for DS1006  */
	// 16.06.2005 changed/remove for Rainer B
	//DS_1602_ERR_MOD=136,             /* 136 = message module for DS1602  */
	//DS_1603_ERR_MOD=137,             /* 137 = message module for DS1603  */
	DS_RAPID_PRO=136,				/* 136 = message module for RapidPro  */
	DS_PROJECT_APPLICATION	= 200,	/* 200 = message module for io-configurator component ProjectApplication */
	DS_CONFIGURATION_MGR		= 201,	/* 201 = message module for io-configurator component PeripheryManager */
	DS_BUILD_MGR			= 202,	/* 202 = message module for io-configurator component BuildManager */
	DS_MAPPING_MGR	= 203,	/* 203 = message module for io-configurator component PeripheryMappingManager */
	DS_HARDWARE_ACCESS		= 204,	/* 204 = message module for io-configurator component TargetAccess */	
//	DS_CORE_PROJECT_MGR		= 205,	/* 205 = message module for CoreProjectManagement */
//	DS_CORE_APPLICATION_MGR = 206,	/* 206 = message module for CoreApplicationManagement */
//	DS_CORE_PERIPHERYMAPPING_MGR = 207,	/* 207 = message module for CorePeripheryManagement */
//	DS_CORE_PERIPHERY_MGR	= 208,	/* 208 = message module for CorePeripheryMappingManagement */
//	DS_CORE_BUILD_MGR		= 209,	/* 209 = message module for CoreBuildManagement */
//	DS_CORE_TARGETSW_MGR	= 210,	/* 210 = message module for CoreTargetSoftwareManagement */
	DS_CORE_PARAMETER		= 211,	/* 211 = message module for CoreParameter */
	DS_HWTOPOLOGY_PLUGINS	= 212,	/* 212 = message module for HWTopologyPlugins */
	DS_IOFUNCTION_PLUGINS	= 213,	/* 213 = message module for IOFunctionPlugins */
//	DS_CORE_HARDWAREBASEOBJECTS	= 214,	/* 214 = message module for CoreHardwareBaseObject */
//	DS_CORE_IOFUNCTIONLIBRARY	= 215,	/* 215 = message module for CoreIOFunctionLibrary */
//	DS_CORE_IOCODEGENERATOR		= 216,	/* 216 = message module for CoreIOCodegenerator */
//	DS_CORE_HARDWAREFACTORIES	= 217,	/* 217 = message module for CoreHardwareFactories */
//	DS_CORE_PERIPHERYACCESS		= 218	/* 218 = message module for CorePeripheryAccess */
	


	// main module numbers are limited to 255 !!
	
	

};

#define DS_1602_ERR_MOD DS_RAPID_PRO

/* Submodules                   */
/*0-2047                        */
#define MSG_SM_DEFAULT                          0

/* Language enum */
enum DSERR_LanguageEnum
{
        /* Language is english*/    
        DSERR_LANGUAGE_ENGLISH,
        /* Language is german*/    
        DSERR_LANGUAGE_GERMAN
};

#define DSERR_DEFAULT_LANGUAGE DSERR_LANGUAGE_ENGLISH

#endif

