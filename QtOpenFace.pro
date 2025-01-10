QT += quick

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/GazeAnalyser.cpp \
    src/gazeTracker.cpp \
        main.cpp \
    openFace/lib/local/FaceAnalyser/src/FaceAnalyser.cpp \
    openFace/lib/local/FaceAnalyser/src/FaceAnalyserParameters.cpp \
    openFace/lib/local/FaceAnalyser/src/Face_utils.cpp \
    openFace/lib/local/FaceAnalyser/src/SVM_dynamic_lin.cpp \
    openFace/lib/local/FaceAnalyser/src/SVM_static_lin.cpp \
    openFace/lib/local/FaceAnalyser/src/SVR_dynamic_lin_regressors.cpp \
    openFace/lib/local/FaceAnalyser/src/SVR_static_lin_regressors.cpp \
    openFace/lib/local/FaceAnalyser/src/stdafx_fa.cpp \
    openFace/lib/local/GazeAnalyser/src/GazeEstimation.cpp \
    openFace/lib/local/LandmarkDetector/src/CCNF_patch_expert.cpp \
    openFace/lib/local/LandmarkDetector/src/CEN_patch_expert.cpp \
    openFace/lib/local/LandmarkDetector/src/CNN_utils.cpp \
    openFace/lib/local/LandmarkDetector/src/FaceDetectorMTCNN.cpp \
    openFace/lib/local/LandmarkDetector/src/LandmarkDetectionValidator.cpp \
    openFace/lib/local/LandmarkDetector/src/LandmarkDetectorFunc.cpp \
    openFace/lib/local/LandmarkDetector/src/LandmarkDetectorModel.cpp \
    openFace/lib/local/LandmarkDetector/src/LandmarkDetectorParameters.cpp \
    openFace/lib/local/LandmarkDetector/src/LandmarkDetectorUtils.cpp \
    openFace/lib/local/LandmarkDetector/src/PAW.cpp \
    openFace/lib/local/LandmarkDetector/src/PDM.cpp \
    openFace/lib/local/LandmarkDetector/src/Patch_experts.cpp \
    openFace/lib/local/LandmarkDetector/src/SVR_patch_expert.cpp \
    openFace/lib/local/LandmarkDetector/src/stdafx.cpp \
    openFace/lib/local/Utilities/src/ImageCapture.cpp \
    openFace/lib/local/Utilities/src/RecorderCSV.cpp \
    openFace/lib/local/Utilities/src/RecorderHOG.cpp \
    openFace/lib/local/Utilities/src/RecorderOpenFace.cpp \
    openFace/lib/local/Utilities/src/RecorderOpenFaceParameters.cpp \
    openFace/lib/local/Utilities/src/SequenceCapture.cpp \
    openFace/lib/local/Utilities/src/VisualizationUtils.cpp \
    openFace/lib/local/Utilities/src/Visualizer.cpp \
    openFace/lib/local/Utilities/src/stdafx_ut.cpp

RESOURCES += qml.qrc
INCLUDEPATH += include \
            openFace/lib/3rdParty/dlib/include \
            openFace/lib/3rdParty/CameraEnumerator \
            openFace/lib/3rdParty/OpenBLAS/include \
            D:/Software/OpenCV/opencv410/build/include \
            openFace/lib/local/FaceAnalyser/include \
            openFace/lib/local/GazeAnalyser/include \
            openFace/lib/local/LandmarkDetector/include \
            openFace/lib/local/Utilities/include
CONFIG += c++17
DEFINES += CONFIG_DIR=\\\"$$PWD\\\"
CONFIG(debug,debug|release){
message("Building in Debug mode---")

LIBS+=-L$$YOUR_PATH/openFace/lib/3rdParty/dlib/lib -ldlib19.24.0_debug_64bit_msvc1929
LIBS+=-L$$YOUR_PATH/openFace/lib/3rdParty/OpenBLAS/lib/x64 -lopenblas
LIBS+=-L$$YOUR_PATH/opencv410/build/x64/vc16/lib -lopencv_world4100d
}
CONFIG(release,debug|release){
message("Building in Release mode---")
LIBS+=-L"$$YOUR_PATH/openFace/lib/3rdParty/dlib/lib" -ldlib19.24.0_release_64bit_msvc1929
LIBS+=-L$$YOUR_PATH/openFace/lib/3rdParty/OpenBLAS/lib/x64 -lopenblas
LIBS+=-L$$YOUR_PATH/opencv410/build/x64/vc16/lib -lopencv_world4100
}

TRANSLATIONS += \
    QtOpenFace_zh_CN.ts
CONFIG += lrelease
CONFIG += embed_translations

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    include/GazeAnalyser.h \
    include/LandmarkDetectorApi.hpp \
    include/OpenCVwrappers.hpp \
    include/gazeTracker.h \
    openFace/lib/local/FaceAnalyser/include/FaceAnalyser.h \
    openFace/lib/local/FaceAnalyser/include/FaceAnalyserParameters.h \
    openFace/lib/local/FaceAnalyser/include/Face_utils.h \
    openFace/lib/local/FaceAnalyser/include/SVM_dynamic_lin.h \
    openFace/lib/local/FaceAnalyser/include/SVM_static_lin.h \
    openFace/lib/local/FaceAnalyser/include/SVR_dynamic_lin_regressors.h \
    openFace/lib/local/FaceAnalyser/include/SVR_static_lin_regressors.h \
    openFace/lib/local/FaceAnalyser/include/stdafx_fa.h \
    openFace/lib/local/GazeAnalyser/include/GazeEstimation.h \
    openFace/lib/local/LandmarkDetector/include/CCNF_patch_expert.h \
    openFace/lib/local/LandmarkDetector/include/CEN_patch_expert.h \
    openFace/lib/local/LandmarkDetector/include/CNN_utils.h \
    openFace/lib/local/LandmarkDetector/include/FaceDetectorMTCNN.h \
    openFace/lib/local/LandmarkDetector/include/LandmarkCoreIncludes.h \
    openFace/lib/local/LandmarkDetector/include/LandmarkDetectionValidator.h \
    openFace/lib/local/LandmarkDetector/include/LandmarkDetectorFunc.h \
    openFace/lib/local/LandmarkDetector/include/LandmarkDetectorModel.h \
    openFace/lib/local/LandmarkDetector/include/LandmarkDetectorParameters.h \
    openFace/lib/local/LandmarkDetector/include/LandmarkDetectorUtils.h \
    openFace/lib/local/LandmarkDetector/include/PAW.h \
    openFace/lib/local/LandmarkDetector/include/PDM.h \
    openFace/lib/local/LandmarkDetector/include/Patch_experts.h \
    openFace/lib/local/LandmarkDetector/include/SVR_patch_expert.h \
    openFace/lib/local/LandmarkDetector/include/stdafx.h \
    openFace/lib/local/Utilities/include/ConcurrentQueue.h \
    openFace/lib/local/Utilities/include/ImageCapture.h \
    openFace/lib/local/Utilities/include/ImageManipulationHelpers.h \
    openFace/lib/local/Utilities/include/RecorderCSV.h \
    openFace/lib/local/Utilities/include/RecorderHOG.h \
    openFace/lib/local/Utilities/include/RecorderOpenFace.h \
    openFace/lib/local/Utilities/include/RecorderOpenFaceParameters.h \
    openFace/lib/local/Utilities/include/RotationHelpers.h \
    openFace/lib/local/Utilities/include/SequenceCapture.h \
    openFace/lib/local/Utilities/include/VisualizationUtils.h \
    openFace/lib/local/Utilities/include/Visualizer.h \
    openFace/lib/local/Utilities/include/stdafx_ut.h

DISTFILES += \
    README.md \
    image/work_out.png

