TEMPLATE = app

CONFIG += c++11

QT += core qml quick network

CONFIG += c++11 console

DEFINES += QT_DEPRECATED_WARNINGS

# python module for windows and linux
# if you change this switch, you may need to clear the cache
#DEFINES += USE_PYTHON_MODULE
#DEFINES += USE_CUDA_MODULE
DEFINES += USE_OPENACC_MODULE
#DEFINES += USE_OPENMP

ZSS_LIBS =
ZSS_INCLUDES =

CONFIG(debug, debug|release) {
    #CONFIG += console
    TARGET = MedusaD
    DESTDIR = $$PWD/../ZBin
    MOC_DIR = ./temp
    OBJECTS_DIR = ./temp
}
CONFIG(release, debug|release) {
    TARGET = Medusa
    DESTDIR = $$PWD/../ZBin
    MOC_DIR = ./temp
    OBJECTS_DIR = ./temp
}

win32 {
    if(contains(DEFINES,USE_OPENMP)){
        QMAKE_CXXFLAGS += -openmp
    }
    win32-msvc*:QMAKE_CXXFLAGS += /wd"4819"
    # Third party library dir
    THIRD_PARTY_DIR = $$PWD/../ZBin/3rdParty
    contains(QMAKE_TARGET.arch, x86_64){
        message("64-bit")
        CONFIG(release,debug|release){
            ZSS_LIBS += $${THIRD_PARTY_DIR}/protobuf/lib/x64/libprotobuf.lib \
                        $${THIRD_PARTY_DIR}/tolua++/lib/x64/tolua++.lib \
                        $${THIRD_PARTY_DIR}/lua/lib/x64/lua5.1.lib
        }
        CONFIG(debug,debug|release){
            ZSS_LIBS += $${THIRD_PARTY_DIR}/protobuf/lib/x64/libprotobufd.lib \
                        $${THIRD_PARTY_DIR}/tolua++/lib/x64/tolua++D.lib \
                        $${THIRD_PARTY_DIR}/lua/lib/x64/lua5.1.lib
        }
    } else {
        message("32-bit")
        CONFIG(release,debug|release){
            ZSS_LIBS += $${THIRD_PARTY_DIR}/protobuf/lib/x86/libprotobuf.lib \
                        $${THIRD_PARTY_DIR}/tolua++/lib/x86/tolua++.lib \
                        $${THIRD_PARTY_DIR}/lua/lib/x86/lua5.1.lib
        }
        CONFIG(debug,debug|release){
            ZSS_LIBS += $${THIRD_PARTY_DIR}/protobuf/lib/x86/libprotobufd.lib \
                        $${THIRD_PARTY_DIR}/tolua++/lib/x86/tolua++.lib \
                        $${THIRD_PARTY_DIR}/lua/lib/x86/lua5.1.lib
        }
    }

    ZSS_INCLUDES += \
        $${THIRD_PARTY_DIR}/protobuf/include \
        $${THIRD_PARTY_DIR}/Eigen \

}

unix:!macx{
    ZSS_INCLUDES += \
        /usr/include/eigen3 \
        /usr/local/include
    ZSS_LIBS += \
        -llua5.1 \
        -ldl \
        -ltolua++5.1 \
        #/usr/local/lib/libprotobuf.so
        -lprotobuf

    #OPENACC Module
    if(contains(DEFINES,USE_OPENACC_MODULE)){
        QMAKE_CXXFLAGS += -fopenacc -fcf-protection=none -fopt-info-optimized-omp
        QMAKE_LFLAGS += -fopenacc -fcf-protection=none -fopt-info-optimized-omp
    }
#    if(contains(DEFINES,USE_OPENACC_MODULE)){
#        message("Using OpenAcc Module")
#        HEADERS += src/OpenAccModule/GetBestUtils.h
#        SOURCES += src/OpenAccModule/GetBestUtils.cpp
#        OPENACC_OBJECTS_DIR += ./temp/openacc
#        OPENACC_SOURCES +=  src/OpenAccModule/BestPosCalculate.cpp
#        OPENACC_DIR = "/opt/nvidia/hpc_sdk/Linux_x86_64/22.7/compilers"
#        CUDA_DIR = "/opt/nvidia/hpc_sdk/Linux_x86_64/2022/cuda"
#        #SYSTEM_NAME = ubuntu
#        #SYSTEM_TYPE = 64
#        #OPENACC_ARCH = compute_61
#        #OPENACC_MODE = sm_61

#        #DEFINES       = -DQT_DEPRECATED_WARNINGS -DUSE_OPENACC_MODULE -DQT_NO_DEBUG -DQT_QUICK_LIB -DQT_GUI_LIB -DQT_QMLMODELS_LIB -DQT_QML_LIB -DQT_NETWORK_LIB -DQT_CORE_LIB
#       # CFLAGS        = -pipe -O2 -Wall -Wextra -D_REENTRANT -fPIC ${DEFINES}
#        #CXXFLAGS      = -pipe -utf-8 -Wno-comment -Wno-reorder -Wno-conversion-null -O2 -std=gnu++11 -Wall -Wextra -D_REENTRANT -fPIC ${DEFINES}
#        INCPATH       = -I../Medusa -I. -I/opt/nvidia/hpc_sdk/Linux_x86_64/22.7/compilers/etc/include_acc -isystem /usr/include/eigen3 -isystem /usr/local/include -I../Medusa/share -I../Medusa/share/proto/cpp -I../Medusa/src -I../Medusa/src/Algorithm -I../Medusa/src/OpenAccModule -I../Medusa/src/LuaModule -I../Medusa/src/Main -I../Medusa/src/MotionControl -I../Medusa/src/OtherLibs -I../Medusa/src/OtherLibs/cmu -I../Medusa/src/OtherLibs/cornell -I../Medusa/src/OtherLibs/nlopt -I../Medusa/src/PathPlan -I../Medusa/src/RefereeBox -I../Medusa/src/Simulator -I../Medusa/src/Strategy -I../Medusa/src/Strategy/rolematch -I../Medusa/src/Strategy/skill -I../Medusa/src/Utils -I../Medusa/src/PointCalculation -I../Medusa/src/Vision -I../Medusa/src/Vision/mediator/net -I../Medusa/src/Wireless -I../Medusa/src/WorldModel -I../../../Qt5.14.0/5.14.0/gcc_64/include -I../../../Qt5.14.0/5.14.0/gcc_64/include/QtQuick -I../../../Qt5.14.0/5.14.0/gcc_64/include/QtGui -I../../../Qt5.14.0/5.14.0/gcc_64/include/QtQmlModels -I../../../Qt5.14.0/5.14.0/gcc_64/include/QtQml -I../../../Qt5.14.0/5.14.0/gcc_64/include/QtNetwork -I../../../Qt5.14.0/5.14.0/gcc_64/include/QtCore -Itemp -isystem /usr/include/libdrm -I../../../Qt5.14.0/5.14.0/gcc_64/mkspecs/linux-g++


#        #include paths  -acc$$OPENACC_DIR/bin/nvc++

#        QMAKE_LIBDIR += $$CUDA_DIR/lib64/
#        #CUDA_INC = $$join(INCLUDEPATH,'" -I"','-I"','"')

#        CUDA_LIBS = -L$$CUDA_DIR/lib64 -lcudart

#        INCLUDEPATH += $$OPENACC_DIR#/include/
#        #INCLUDEPATH += $$OPENACC_DIR/include/openacc.h
#        #INCLUDEPATH += $$OPENACC_DIR/include/_cplus_preinclude.h
#        #INCLUDEPATH += $$OPENACC_DIR/include/openacc_predef.h
#        #INCLUDEPATH += $$OPENACC_DIR/include/openacc.h
#        #INCLUDEPATH += $$CUDA_DIR/include
#        #ACC_INC = $$join(INC,'')
#        QMAKE_LIBDIR += $$OPENACC_DIR/lib/
#        LIBS +=$$CUDA_LIBS
#        OPENACC_LIBS = -L$$OPENACC_DIR/lib -lnvc -lpgc -lpgm -lacchost   -lnvhpcatm -lacccuda10 \ #-lpgmath -lomp -laccdevice
#        -lacccuda -laccdevaux10 -laccdevaux -laccdevice  -laccnotify \
#        -laccprof -lblas_ilp64 -lblas_lp64 -lblas -lcudadevice -lcudafor_102 \
#        -lcudafor_110 -lcudafor_113 -lcudafor2 -lcudafor -lcudaforwrapblas117 \
#        -lcudaforwrapblas -lcudaforwrapnccl -lcudaforwraprand -lcudaforwrapsparse10 \
#        -lcudaforwrapsparse11 -lcudaforwraptensor -lcudanvhpc -lgomp -lhugetlbfs_nvhpc \
#        -llapack_ilp64 -llapack_lp64 -llapack -lnvcpumath-avx2 -lnvcpumath-avx512 \
#        -lnvcpumath-avx -lnvcpumath -lnvf-avx2 -lnvf -lnvhpcatm -lnvhpcmanaux \
#        -lnvhpcman -lnvhpcwrapcufftmp -lnvhpcwrapcufft -lnvhpcwrapnvtx \
#        -lnvlamath -lnvomp_ompt -lnvomp -lomp

#        NVC++_LIBS = $$join(OPENACC_LIBS,' -l','-l', '')

#        LIBS += $$OPENACC_LIBS

#        OPENACC_INC = $$join(INCLUDEPATH,' -I','-I',' ')


#        #openacc.commands = /usr/bin/g++ -c -pipe -utf-8 -Wno-comment -Wno-reorder -Wno-conversion-null -O2 -std=gnu++11 -Wall -Wextra -D_REENTRANT -fPIC \
#        #-DQT_DEPRECATED_WARNINGS -DUSE_OPENACC_MODULE -DQT_NO_DEBUG -DQT_QUICK_LIB -DQT_GUI_LIB -DQT_QMLMODELS_LIB -DQT_QML_LIB -DQT_NETWORK_LIB -DQT_CORE_LIB \
#        # -I../Medusa -I. -I/opt/nvidia/hpc_sdk/Linux_x86_64/22.7/compilers/etc/include_acc -isystem /usr/include/eigen3 -isystem /usr/local/include -I../Medusa/share -I../Medusa/share/proto/cpp -I../Medusa/src -I../Medusa/src/Algorithm -I../Medusa/src/OpenAccModule -I../Medusa/src/LuaModule -I../Medusa/src/Main -I../Medusa/src/MotionControl -I../Medusa/src/OtherLibs -I../Medusa/src/OtherLibs/cmu -I../Medusa/src/OtherLibs/cornell -I../Medusa/src/OtherLibs/nlopt -I../Medusa/src/PathPlan -I../Medusa/src/RefereeBox -I../Medusa/src/Simulator -I../Medusa/src/Strategy -I../Medusa/src/Strategy/rolematch -I../Medusa/src/Strategy/skill -I../Medusa/src/Utils -I../Medusa/src/PointCalculation -I../Medusa/src/Vision -I../Medusa/src/Vision/mediator/net -I../Medusa/src/Wireless -I../Medusa/src/WorldModel -I../../../Qt5.14.0/5.14.0/gcc_64/include -I../../../Qt5.14.0/5.14.0/gcc_64/include/QtQuick -I../../../Qt5.14.0/5.14.0/gcc_64/include/QtGui -I../../../Qt5.14.0/5.14.0/gcc_64/include/QtQmlModels -I../../../Qt5.14.0/5.14.0/gcc_64/include/QtQml -I../../../Qt5.14.0/5.14.0/gcc_64/include/QtNetwork -I../../../Qt5.14.0/5.14.0/gcc_64/include/QtCore -Itemp -isystem /usr/include/libdrm -I../../../Qt5.14.0/5.14.0/gcc_64/mkspecs/linux-g++ \
#         #-o ${QMAKE_FILE_OUT} ${QMAKE_FILE_NAME}
#        openacc.commands = $$OPENACC_DIR/bin/nvc++   -c -gpu=ccnative  -acc $(INCPATH) $$OPENACC_INC $$LIBS -fast -O2  -Wall -Wextra -D_REENTRANT  \
#        -fPIC -o ${QMAKE_FILE_OUT} ${QMAKE_FILE_NAME}
#        #  -std=gnu++11   -⁠mcmodel=medium -arch=compute_61 -gpu=cuda11.7-stdpar-std=gnu++11    -utf-8

#        openacc.dependency_type = TYPE_C
#        #openacc.depend_command = $$OPENACC_DIR/bin/nvc++  -M $$OPENACC_INC -acc  ${QMAKE_FILE_NAME}

#        openacc.input  = OPENACC_SOURCES
#        openacc.output = ${OBJECTS_DIR}${QMAKE_FILE_BASE}_openacc.o
#        QMAKE_EXTRA_COMPILERS += openacc
#    }# end of OPENACC module
}

INCLUDEPATH += \
    $$ZSS_INCLUDES

LIBS += \
    $$ZSS_LIBS

INCLUDEPATH += \
    share \
    share/proto/cpp \
    src \
    src/Algorithm \
    src/OpenAccModule\
    src/LuaModule \
    src/Main \
    src/MotionControl \
    src/OtherLibs \
    src/OtherLibs/cmu \
    src/OtherLibs/cornell \
    src/OtherLibs/nlopt \
    src/PathPlan \
    src/RefereeBox \
    src/Simulator \
    src/Strategy \
    src/Strategy/rolematch \
    src/Strategy/skill \
    src/Utils \
    src/PointCalculation \
    src/Vision \
    src/Vision/mediator/net \
    src/Wireless \
    src/WorldModel

SOURCES += \
    share/geometry.cpp \
    share/parammanager.cpp \
    share/proto/cpp/grSim_Commands.pb.cc \
    share/proto/cpp/grSimMessage.pb.cc \
    share/proto/cpp/grSim_Packet.pb.cc \
    share/proto/cpp/grSim_Replacement.pb.cc \
    share/proto/cpp/log_labeler_data.pb.cc \
    share/proto/cpp/log_labels.pb.cc \
    share/proto/cpp/messages_robocup_ssl_detection.pb.cc \
    share/proto/cpp/messages_robocup_ssl_geometry_legacy.pb.cc \
    share/proto/cpp/messages_robocup_ssl_geometry.pb.cc \
    share/proto/cpp/messages_robocup_ssl_refbox_log.pb.cc \
    share/proto/cpp/messages_robocup_ssl_wrapper_legacy.pb.cc \
    share/proto/cpp/messages_robocup_ssl_wrapper.pb.cc \
    share/proto/cpp/ssl_game_controller_auto_ref.pb.cc \
    share/proto/cpp/ssl_game_controller_common.pb.cc \
    share/proto/cpp/ssl_game_controller_team.pb.cc \
    share/proto/cpp/ssl_game_event_2019.pb.cc \
    share/proto/cpp/ssl_game_event.pb.cc \
    share/proto/cpp/ssl_referee.pb.cc \
    share/proto/cpp/vision_detection.pb.cc \
    share/proto/cpp/zss_cmd.pb.cc \
    share/proto/cpp/zss_debug.pb.cc \
    src/Algorithm/ShootModule.cpp \
    src/Algorithm/ShootRangeList.cpp \
    src/Algorithm/messidecition.cpp \
    src/Algorithm/newrunpos.cpp \
    src/Algorithm/passposevaluate.cpp \
    src/Algorithm/runpos.cpp \
    src/LuaModule/LuaModule.cpp \
    src/LuaModule/lua_zeus.cpp \
    src/Main/ActionModule.cpp \
    src/Main/DecisionModule.cpp \
    src/Main/Global.cpp \
    src/Main/OptionModule.cpp \
    src/Main/zeus_main.cpp \
    src/MotionControl/CMmotion.cpp \
    src/MotionControl/ControlModel.cpp \
    src/MotionControl/CubicEquation.cpp \
    src/MotionControl/DynamicsSafetySearch.cpp \
    src/MotionControl/noneTrapzodalVelTrajectory.cpp \
    src/MotionControl/QuadraticEquation.cpp \
    src/MotionControl/QuarticEquation.cpp \
    src/MotionControl/TrapezoidalVelTrajectory.cpp \
    src/OpenAccModule/BestPosCalculate.cpp \
    src/OpenAccModule/GetBestUtils.cpp \
    src/OtherLibs/cmu/obstacle.cpp \
    src/OtherLibs/cmu/path_planner.cpp \
    src/OtherLibs/cornell/Trajectory.cpp \
    src/OtherLibs/cornell/TrajectoryStructs.cpp \
    src/OtherLibs/cornell/TrajectorySupport.cpp \
    src/PathPlan/KDTreeNew.cpp \
    src/PathPlan/ObstacleNew.cpp \
    src/PathPlan/PathPlanner.cpp \
    src/PathPlan/PredictTrajectory.cpp \
    src/PathPlan/RRTPathPlanner.cpp \
    src/PathPlan/RRTPathPlannerNew.cpp \
    src/PointCalculation/guardpos.cpp \
    src/RefereeBox/RefereeBoxIf.cpp \
    src/Simulator/CommandInterface.cpp \
    src/Strategy/defence/AttributeSet.cpp \
    src/Strategy/defence/DefenceInFo.cpp \
    src/Strategy/defence/OppAttributesFactory.cpp \
    src/Strategy/defence/OppPlayer.cpp \
    src/Strategy/defence/OppRoleFactory.cpp \
    src/Strategy/defence/OppRoleMatcher.cpp \
    src/Strategy/defence/Trigger.cpp \
    src/Strategy/defence/defencesequence.cpp \
    src/Strategy/rolematch/matrix.cpp \
    src/Strategy/rolematch/munkres.cpp \
    src/Strategy/rolematch/munkresTacticPositionMatcher.cpp \
    src/Strategy/skill/BIGpenaltyKick.cpp \
    src/Strategy/skill/BasicPlay.cpp \
    src/Strategy/skill/ChaseKick.cpp \
    src/Strategy/skill/Defend.cpp \
    src/Strategy/skill/Dnamicfindpos.cpp \
    src/Strategy/skill/DynamicFindPos.cpp \
    src/Strategy/skill/Escape.cpp \
    src/Strategy/skill/Factory.cpp \
    src/Strategy/skill/FetchBall.cpp \
    src/Strategy/skill/Goalie.cpp \
    src/Strategy/skill/GotoPosition.cpp \
    src/Strategy/skill/LeftHalf.cpp \
    src/Strategy/skill/Marking.cpp \
    src/Strategy/skill/MarkingFront.cpp \
    src/Strategy/skill/MarkingTouch.cpp \
    src/Strategy/skill/MiddleHalf.cpp \
    src/Strategy/skill/OpenSpeed.cpp \
    src/Strategy/skill/PeanltyGoalie.cpp \
    src/Strategy/skill/Penaltykick.cpp \
    src/Strategy/skill/PlayerTask.cpp \
    src/Strategy/skill/RightHalf.cpp \
    src/Strategy/skill/SingleTier.cpp \
    src/Strategy/skill/SmartGotoPosition.cpp \
    src/Strategy/skill/Speed.cpp \
    src/Strategy/skill/StaticGetBall.cpp \
    src/Strategy/skill/StopRobot.cpp \
    src/Strategy/skill/Tier.cpp \
    src/Strategy/skill/Tier1.cpp \
    src/Strategy/skill/Tire.cpp \
    src/Strategy/skill/Touch.cpp \
    src/Strategy/skill/Uniquekillskilll.cpp \
    src/Strategy/skill/Uniquekillskillm.cpp \
    src/Strategy/skill/Uniquekillskillr.cpp \
    src/Strategy/skill/assister.cpp \
    src/Strategy/skill/defend.cpp \
    src/Strategy/skill/defend_05.cpp \
    src/Strategy/skill/defend_07_0.cpp \
    src/Strategy/skill/defend_07_1.cpp \
    src/Strategy/skill/goAndTurnKick.cpp \
    src/Strategy/skill/graph.cpp \
    src/Strategy/skill/guan.cpp \
    src/Strategy/skill/leader.cpp \
    src/Strategy/skill/leader_getball.cpp \
    src/Strategy/skill/naqiu.cpp \
    src/Strategy/skill/sao.cpp \
    src/Strategy/skill/special.cpp \
    src/Strategy/skill/wback.cpp \
    src/Strategy/skill/wbreak.cpp \
    src/Strategy/skill/wdrag.cpp \
    src/Strategy/skill/wmarking.cpp \
    src/Utils/BufferCounter.cpp \
    src/Utils/GDebugEngine.cpp \
    src/Utils/SkillUtils.cpp \
    src/Utils/utils.cpp \
    src/Vision/BallPredictor.cpp \
    src/Vision/CollisionSimulator.cpp \
    src/Vision/RobotPredictError.cpp \
    src/Vision/RobotPredictor.cpp \
    src/Vision/RobotsCollision.cpp \
    src/Vision/VisionModule.cpp \
    src/Vision/VisionReceiver.cpp \
    src/Wireless/PlayerCommandV2.cpp \
    src/Wireless/RobotSensor.cpp \
    src/WorldModel/PlayInterface.cpp \
    src/WorldModel/RobotCapability.cpp \
    src/WorldModel/WorldModel_basic.cpp \
    src/Algorithm/ballmodel.cpp \
    src/Algorithm/BallSpeedModel.cpp \
    src/Algorithm/BallStatus.cpp \
    src/Algorithm/CollisionDetect.cpp \
    src/Algorithm/ContactChecker.cpp \
    src/Algorithm/runtimepredictor.cpp


HEADERS += \
    share/Semaphore.h \
    share/ThreadPool.h \
    share/dataqueue.hpp \
    share/geometry.h \
    share/parammanager.h \
    share/proto/cpp/grSim_Commands.pb.h \
    share/proto/cpp/grSimMessage.pb.h \
    share/proto/cpp/grSim_Packet.pb.h \
    share/proto/cpp/grSim_Replacement.pb.h \
    share/proto/cpp/log_labeler_data.pb.h \
    share/proto/cpp/log_labels.pb.h \
    share/proto/cpp/messages_robocup_ssl_detection.pb.h \
    share/proto/cpp/messages_robocup_ssl_geometry_legacy.pb.h \
    share/proto/cpp/messages_robocup_ssl_geometry.pb.h \
    share/proto/cpp/messages_robocup_ssl_refbox_log.pb.h \
    share/proto/cpp/messages_robocup_ssl_wrapper_legacy.pb.h \
    share/proto/cpp/messages_robocup_ssl_wrapper.pb.h \
    share/proto/cpp/ssl_game_controller_auto_ref.pb.h \
    share/proto/cpp/ssl_game_controller_common.pb.h \
    share/proto/cpp/ssl_game_controller_team.pb.h \
    share/proto/cpp/ssl_game_event_2019.pb.h \
    share/proto/cpp/ssl_game_event.pb.h \
    share/proto/cpp/ssl_referee.pb.h \
    share/proto/cpp/vision_detection.pb.h \
    share/proto/cpp/zss_cmd.pb.h \
    share/proto/cpp/zss_debug.pb.h \
    share/singleton.hpp \
    share/staticparams.h \
    src/Algorithm/ShootModule.h \
    src/Algorithm/ShootRangeList.h \
    src/Algorithm/messidecition.h \
    src/Algorithm/newrunpos.h \
    src/Algorithm/passposevaluate.h \
    src/Algorithm/runpos.h \
    src/LuaModule/lauxlib.h \
    src/LuaModule/luaconf.h \
    src/LuaModule/lua.h \
    src/LuaModule/lualib.h \
    src/LuaModule/LuaModule.h \
    src/LuaModule/tolua++.h \
    src/Main/ActionModule.h \
    src/Main/DecisionModule.h \
    src/Main/Global.h \
    src/Main/OptionModule.h \
    src/Main/TaskMediator.h \
    src/MotionControl/CMmotion.h \
    src/MotionControl/ControlModel.h \
    src/MotionControl/CubicEquation.h \
    src/MotionControl/DynamicsSafetySearch.h \
    src/MotionControl/noneTrapzodalVelTrajectory.h \
    src/MotionControl/QuadraticEquation.h \
    src/MotionControl/QuarticEquation.h \
    src/MotionControl/TrapezoidalVelTrajectory.h \
    src/OpenAccModule/BestPosCalculate.h \
    src/OpenAccModule/GetBestUtils.h \
    #src/OpenAccModule/test1.h \
    src/OtherLibs/cmu/constants.h \
    src/OtherLibs/cmu/fast_alloc.h \
    src/OtherLibs/cmu/kdtree.h \
    src/OtherLibs/cmu/obstacle.h \
    src/OtherLibs/cmu/path_planner.h \
    src/OtherLibs/cmu/util.h \
    src/OtherLibs/cmu/vector.h \
    src/OtherLibs/cornell/Trajectory.h \
    src/OtherLibs/cornell/TrajectoryStructs.h \
    src/OtherLibs/cornell/TrajectorySupport.h \
    src/OtherLibs/nlopt/nlopt.h \
    src/OtherLibs/nlopt/nlopt.hpp \
    src/PathPlan/FastAllocator.h \
    src/PathPlan/KDTreeNew.h \
    src/PathPlan/ObstacleNew.h \
    src/PathPlan/PathPlanner.h \
    src/PathPlan/PredictTrajectory.h \
    src/PathPlan/RRTPathPlanner.h \
    src/PathPlan/RRTPathPlannerNew.h \
    src/PathPlan/Stack.h \
    src/PointCalculation/guardpos.h \
    src/RefereeBox/game_state.h \
    src/RefereeBox/playmode.h \
    src/RefereeBox/RefereeBoxIf.h \
    src/RefereeBox/referee_commands.h \
    src/Simulator/CommandInterface.h \
    src/Simulator/server.h \
    src/Simulator/ServerInterface.h \
    src/Strategy/defence/Attribute.h \
    src/Strategy/defence/AttributeSet.h \
    src/Strategy/defence/DefenceInFo.h \
    src/Strategy/defence/OppAttributesFactory.h \
    src/Strategy/defence/OppPlayer.h \
    src/Strategy/defence/OppRole.h \
    src/Strategy/defence/OppRoleFactory.h \
    src/Strategy/defence/OppRoleMatcher.h \
    src/Strategy/defence/Trigger.h \
    src/Strategy/defence/defencesequence.h \
    src/Strategy/rolematch/matrix.h \
    src/Strategy/rolematch/munkres.h \
    src/Strategy/rolematch/munkresTacticPositionMatcher.h \
    src/Strategy/skill/BIGpenaltyKick.h \
    src/Strategy/skill/BasicPlay.h \
    src/Strategy/skill/ChaseKick.h \
    src/Strategy/skill/Defend.h \
    src/Strategy/skill/Dnamicfindpos.h \
    src/Strategy/skill/DynamicFindPos.h \
    src/Strategy/skill/Escape.h \
    src/Strategy/skill/Factory.h \
    src/Strategy/skill/FetchBall.h \
    src/Strategy/skill/GetBallV1.h \
    src/Strategy/skill/Goalie.h \
    src/Strategy/skill/GotoPosition.h \
    src/Strategy/skill/LeftHalf.h \
    src/Strategy/skill/Marking.h \
    src/Strategy/skill/MarkingFront.h \
    src/Strategy/skill/MarkingTouch.h \
    src/Strategy/skill/MiddleHalf.h \
    src/Strategy/skill/OpenSpeed.h \
    src/Strategy/skill/PeanltyGoalie.h \
    src/Strategy/skill/PenaltyKick.h \
    src/Strategy/skill/PlayerTask.h \
    src/Strategy/skill/RightHalf.h \
    src/Strategy/skill/SingleTier.h \
    src/Strategy/skill/SmartGotoPosition.h \
    src/Strategy/skill/Speed.h \
    src/Strategy/skill/StaticGetBall.h \
    src/Strategy/skill/StopRobot.h \
    src/Strategy/skill/Tier.h \
    src/Strategy/skill/Tier1.h \
    src/Strategy/skill/Tire.h \
    src/Strategy/skill/Touch.h \
    src/Strategy/skill/Uniquekillskilll.h \
    src/Strategy/skill/Uniquekillskillm.h \
    src/Strategy/skill/Uniquekillskillr.h \
    src/Strategy/skill/assister.h \
    src/Strategy/skill/defend.h \
    src/Strategy/skill/defend_05.h \
    src/Strategy/skill/defend_07_0.h \
    src/Strategy/skill/defend_07_1.h \
    src/Strategy/skill/goAndTurnKick.h \
    src/Strategy/skill/graph.h \
    src/Strategy/skill/guan.h \
    src/Strategy/skill/leader.h \
    src/Strategy/skill/leader_getball.h \
    src/Strategy/skill/naqiu.h \
    src/Strategy/skill/sao.h \
    src/Strategy/skill/special.h \
    src/Strategy/skill/wback.h \
    src/Strategy/skill/wbreak.h \
    src/Strategy/skill/wdrag.h \
    src/Strategy/skill/wmarking.h \
    src/Utils/BufferCounter.h \
    src/Utils/ClassFactory.h \
    src/Utils/DataQueue.hpp \
    src/Utils/GDebugEngine.h \
    src/Utils/SkillUtils.h \
    src/Utils/misc_types.h \
    src/Utils/MultiThread.h \
    src/Utils/Semaphore.h \
    src/Utils/singleton.h \
    src/Utils/utils.h \
    src/Utils/ValueRange.h \
    src/Utils/weerror.h \
    src/Vision/BallPredictor.h \
    src/Vision/CollisionSimulator.h \
    src/Vision/mediator/net/message.h \
    src/Vision/RobotPredictData.h \
    src/Vision/RobotPredictError.h \
    src/Vision/RobotPredictor.h \
    src/Vision/RobotsCollision.h \
    src/Vision/VisionModule.h \
    src/Vision/VisionReceiver.h \
    src/Wireless/CommandFactory.h \
    src/Wireless/CommControl.h \
    src/Wireless/PlayerCommand.h \
    src/Wireless/PlayerCommandV2.h \
    src/Wireless/RobotCommand.h \
    src/Wireless/RobotSensor.h \
    src/Wireless/SerialPort.h \
    src/WorldModel/DribbleStatus.h \
    src/WorldModel/KickStatus.h \
    src/WorldModel/PlayInterface.h \
    src/WorldModel/RobotCapability.h \
    src/WorldModel/WorldDefine.h \
    src/WorldModel/WorldModel.h \
    src/Algorithm/ballmodel.h \
    src/Algorithm/BallSpeedModel.h \
    src/Algorithm/BallStatus.h \
    src/Algorithm/CollisionDetect.h \
    src/Algorithm/ContactChecker.h \
    src/Algorithm/runtimepredictor.h



win32-msvc*: QMAKE_LFLAGS += /FORCE:MULTIPLE

QMAKE_CXXFLAGS += -utf-8
#QMAKE_CXXFLAGS += -fopenacc
unix:!macx{
    QMAKE_CXXFLAGS += -Wno-comment -Wno-reorder -Wno-conversion-null
}

#message($$INCLUDEPATH)

#LD_LIBRARY_PATH=dir：$LD_LIBRARY_PATH
#export LD_LIBRARY_PATH
