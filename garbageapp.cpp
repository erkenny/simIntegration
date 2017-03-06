#ifndef MODEL


#include "garbageapp.h"
#include "ui_garbageapp.h"


extern "C" {
/*
#include "PCG_Eval_File_1.h"
#include "rtwtypes.h"
#include "PCG_Eval_File_1_private.h"
#include "defineImportedData.h"
*/
#include "SD_jf_median.h"
#include "rtwtypes.h"
#include "SD_jf_median_private.h"


}
/************************
static BlockIO_PCG_Eval_File_4 PCG_Eval_File_4_B; // Observable signals
static D_Work_PCG_Eval_File_4 PCG_Eval_File_4_DWork; // Observable states

real_T pos_cmd_one;                    // '<Root>/Signal Conversion1'
real_T pos_cmd_two;                    // '<Root>/Signal Conversion2'
extern ThrottleCommands ThrotComm;            // '<Root>/Pos_Command_Arbitration'
extern ThrottleParams Throt_Param;            // '<S1>/Bus Creator'

int simulationLoop = 0;

/************/  
// These three are storage/states from sd_jf_median.c
static BlockIO_SD_jf_median SD_jf_median_B;                // Block signals (auto storage)
static ContinuousStates_SD_jf_median SD_jf_median_X;       // continuous states                 (***maybe should not be static b/c continuously updated?)
static D_Work_SD_jf_median SD_jf_median_DWork;             // Block states (auto storage)

real_T out_vel;                                             // hopefully will be vcomm??


double count = 0;
bool simulateOn = false;

garbageApp::garbageApp(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::garbageApp)
{
    ui->setupUi(this);
}

garbageApp::~garbageApp()
{
    delete ui;
}

void garbageApp::on_timerInit_clicked()
{
    timerId = startTimer(100);         // call timer event every 0.1 seconds
}

void garbageApp::timerEvent(QTimerEvent *event)
{
    ui->timerOut->setValue(count);
    count++;
}

void garbageApp::on_simInit_clicked()
{
      simulateOn = !simulateOn;
      if (simulateOn)
        int whatever = simMain();
}

int garbageApp::simMain(void)
{
    
    // Initialize model

    /******
      PC_Pos_Command_Arbitration_Init(); // Set up the data structures for chart
      PCG_Eval_Fil_Define_Throt_Param(); // SubSystem: '<Root>/Define_Throt_Param'
      defineImportData();               // Defines the memory and values of inputs
    */

    // use GRT compatible call interface b/c call the user def'd fns'
    // set up num of states, blocks, outputs, inputs etc., then initialize them
    MdlInitializeSizes();                       
    // MdlInitialize();
    SD_jf_median_initialize();          // initialize model fn from sd_jf_median.c
    MdlStart();                         // calls MdlInitialize(), starts model?

    // initiate real time engine?? (sd_jf_median_rti.c)
    // rti_TIMERA(rtk_p_task_control_block task);       // maybe eeded to set up base timer but unclear where this actually gets called?
    rti_mdl_initialize_host_services();                 // set up model as host
    rti_mdl_initialize_io_boards();                     // set up board io
    // do this next one before prevs? order will matter. this fn creates tasks, but unclear how to access
    rti_th_initialize();                // initialize rti task handling to create adn bind all tasks (timerA interrupt)
                                        // from task handler (sd_jf_median_th.c)

      do // This is the "Schedule" loop. Functions would be called based on a scheduling algorithm
      {
        /// HARDWARE I/O
                // i have no idea what to put here. I think this is handled by setting up task handling?


        /// Call control algorithms //

        /*
        PI_Cntrl_Reusable((*pos_rqst),fbk_1,&PCG_Eval_File_4_B.PI_ctrl_1,
                           &PCG_Eval_File_4_DWork.PI_ctrl_1,
                           I_Gain, P_Gain);
        PI_Cntrl_Reusable((*pos_rqst),fbk_2,&PCG_Eval_File_4_B.PI_ctrl_2,
                           &PCG_Eval_File_4_DWork.PI_ctrl_2,
                           I_Gain_2, P_Gain_2);
        pos_cmd_one = PCG_Eval_File_4_B.PI_ctrl_1.Saturation1;                      // from structs in PCG
        pos_cmd_two = PCG_Eval_File_4_B.PI_ctrl_2.Saturation1;

        PCG_Eva_Pos_Command_Arbitration(pos_cmd_one, &Throt_Param, pos_cmd_two);

            simulationLoop++;
            QString s = QString::number((double)pos_cmd_two);
            ui->simOut->addItem(s);
        */
        // from SD_jf_median.c                           //   need to figure out where to get tid ==> oh so "tid is required for a uniform function interface, val is not used in function"
            // replaced tid input with 1
            MdlUpdate(1);                              // update model 
            MdlOutputs(1);                             // calc new outputs                                                        
            // SD_jf_median_output(int_T tid);                // model output fn also from sd_jf_median.c
                                                        // assuming that tid is a time input? unsure where this comes into play here 
                                                        // may be more appropriate to call MdlOutputs
           out_vel = SD_jf_median_B.Vcomm;
           QString s = QString::number((double)out_vel);
           ui->simOut->addItem(s);
      } while (simulateOn);

      return 0;
    
}

#endif

