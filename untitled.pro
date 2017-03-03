#-------------------------------------------------
#
# Project created by QtCreator 2017-01-25T14:52:26
#
#-------------------------------------------------

QT       += core gui

TARGET = untitled
TEMPLATE = app


SOURCES += main.cpp\
        garbageapp.cpp\
        SD_jf_mg_back_stride.c\
        SD_jf_mg_back_stride_data.c\
        SD_jf_mg_back_stride_rti.c\
        rti_th_aux.c\
        SD_jf_mg_back_stride_th.c\
        SD_jf_mg_back_stride_trc_ptr.c\
        SD_jf_mg_back_stride_data.c


#
#        PCG_Eval_File_1.c\
#        PI_Cntrl_Reusable.c\
#        PCG_Eval_Fil_Define_Throt_Param.c\
#        eval_data.c\
#        rt_look.c\
#        rt_look1d.c\
#        defineImportedData.c

HEADERS  += garbageapp.h\
            SD_jf_mg_back_stride.h\
            SD_jf_mg_back_stride_types.h\
            simstruc_types.h\
            simstruc.h\
            rtwtypes.h\
            SD_jf_mg_back_stride_trc_ptr.h\
            SD_jf_mg_back_stride_private.h


#            PCG_Eval_File_1.h\
#            rtwtypes.h\
#            rtlibsrc.h\
#            eval_data.h\
#            PCG_Eval_File_1_private.h\
#            ThrottleBus.h\
#            PCG_Eval_File_1_types.h\
#            PI_Cntrl_Reusable.h\
#            PCG_Eval_Fil_Define_Throt_Param.h\
#            defineImportedData.h


FORMS    += garbageapp.ui
