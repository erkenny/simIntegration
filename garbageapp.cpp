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
#include "SD_jf_mg_back_stride.h"
#include "rtwtypes.h"
#include "SD_jf_mg_back_stride_private.h"


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
static BlockIO_SD_jf_mg_back_stride SD_jf_mg_back_stride_B;// Block signals (auto storage) [from SD_jf_mg_back_stride.c]
static D_Work_SD_jf_mg_back_stride SD_jf_mg_back_stride_DWork; // Block states (auto storage) [from SD_jf_mg_back_stride.c]




double count = 0;

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
    timerId = startTimer(1000);         // call timer event ever second
}

void garbageApp::timerEvent(QTimerEvent *event)
{
    ui->timerOut->setValue(count);
    count++;
}

void garbageApp::on_simInit_clicked()
{

      int whatever = simMain();
}

int garbageApp::simMain(void)
{
    /*
    // Initialize model
      PC_Pos_Command_Arbitration_Init(); // Set up the data structures for chart
      PCG_Eval_Fil_Define_Throt_Param(); // SubSystem: '<Root>/Define_Throt_Param'
      defineImportData();               // Defines the memory and values of inputs

      do // This is the "Schedule" loop.
          Functions would be called based on a scheduling algorithm
      {
        /// HARDWARE I/O

            /// Call control algorithms //
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
      } while (simulationLoop < 2);
      return 0;
    */
}

#endif

