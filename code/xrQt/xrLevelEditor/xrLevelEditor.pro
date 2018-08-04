QT += quick
CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += main.cpp \
    src/Edit/AppendObjectInfoForm.cpp \
    src/Edit/Builder.cpp \
    src/Edit/BuilderCF.cpp \
    src/Edit/BuilderCore.cpp \
    src/Edit/BuilderGame.cpp \
    src/Edit/BuilderLODs.cpp \
    src/Edit/BuilderLTX.cpp \
    src/Edit/BuilderOGF.cpp \
    src/Edit/BuilderRemote.cpp \
    src/Edit/Cursor3D.cpp \
    src/Edit/Custom2DProjector.cpp \
    src/Edit/CustomObject.cpp \
    src/Edit/CustomObjectAnimation.cpp \
    src/Edit/CustomObjectLE.cpp \
    src/Edit/EditObjectExportLWO.cpp \
    src/Edit/ELight.cpp \
    src/Edit/ELight_IO.cpp \
    src/Edit/ELight_props.cpp \
    src/Edit/EParticlesObject.cpp \
    src/Edit/ESceneAIMapControls.cpp \
    src/Edit/ESceneAIMapTools.cpp \
    src/Edit/ESceneAIMapTools_Controls.cpp \
    src/Edit/ESceneAIMapTools_Export.cpp \
    src/Edit/ESceneAIMapTools_Generate.cpp \
    src/Edit/ESceneAIMapTools_MotionSimulator.cpp \
    src/Edit/ESceneAIMapTools_Pick.cpp \
    src/Edit/ESceneAIMapTools_Render.cpp \
    src/Edit/ESceneControlsCustom.cpp \
    src/Edit/ESceneCustomMTools.cpp \
    src/Edit/ESceneCustomMToolsControls.cpp \
    src/Edit/ESceneCustomOTools.cpp \
    src/Edit/ESceneCustomOToolsFactory.cpp \
    src/Edit/ESceneCustomOToolsIO.cpp \
    src/Edit/ESceneDOTools.cpp \
    src/Edit/ESceneDOTools_Controls.cpp \
    src/Edit/ESceneDOTools_Generate.cpp \
    src/Edit/ESceneDOTools_Pick.cpp \
    src/Edit/ESceneDummyTools.cpp \
    src/Edit/ESceneFogVolumeTools.cpp \
    src/Edit/ESceneGlowTools.cpp \
    src/Edit/ESceneGlowToolsIO.cpp \
    src/Edit/ESceneGroupControls.cpp \
    src/Edit/ESceneGroupTools.cpp \
    src/Edit/ESceneGroupToolsIO.cpp \
    src/Edit/ESceneLightTools.cpp \
    src/Edit/ESceneLightToolsIO.cpp \
    src/Edit/ESceneObjectControls.cpp \
    src/Edit/ESceneObjectTools.cpp \
    src/Edit/ESceneObjectTools_.cpp \
    src/Edit/ESceneObjectTools__.cpp \
    src/Edit/ESceneObjectToolsExportBreakable.cpp \
    src/Edit/ESceneObjectToolsIO.cpp \
    src/Edit/EScenePortalControls.cpp \
    src/Edit/EScenePortalTools.cpp \
    src/Edit/EScenePortalToolsIO.cpp \
    src/Edit/EScenePSControls.cpp \
    src/Edit/EScenePSTools.cpp \
    src/Edit/EScenePSToolsIO.cpp \
    src/Edit/ESceneSectorControls.cpp \
    src/Edit/ESceneSectorTools.cpp \
    src/Edit/ESceneSectorToolsIO.cpp \
    src/Edit/ESceneShapeControls.cpp \
    src/Edit/ESceneShapeTools.cpp \
    src/Edit/ESceneShapeToolsIO.cpp \
    src/Edit/ESceneSoundEnvTools.cpp \
    src/Edit/ESceneSoundEnvToolsIO.cpp \
    src/Edit/ESceneSoundSrcTools.cpp \
    src/Edit/ESceneSoundSrcToolsIO.cpp \
    src/Edit/ESceneSpawnControls.cpp \
    src/Edit/ESceneSpawnTools.cpp \
    src/Edit/ESceneSpawnToolsIO.cpp \
    src/Edit/ESceneWallmarkControls.cpp \
    src/Edit/ESceneWallmarkTools.cpp \
    src/Edit/ESceneWayControls.cpp \
    src/Edit/ESceneWayTools.cpp \
    src/Edit/ESceneWayToolsIO.cpp \
    src/Edit/EShape.cpp \
    src/Edit/ESound_Environment.cpp \
    src/Edit/ESound_Source.cpp \
    src/Edit/FS2.cpp \
    src/Edit/GeometryPartExtractor.cpp \
    src/Edit/glow.cpp \
    src/Edit/GroupObject.cpp \
    src/Edit/GroupObjectUtils.cpp \
    src/Edit/LevelPreferences.cpp \
    src/Edit/portal.cpp \
    src/Edit/portalutils.cpp \
    src/Edit/scene.cpp \
    src/Edit/Scene_Tools.cpp \
    src/Edit/SceneIO.cpp \
    src/Edit/SceneObject.cpp \
    src/Edit/SceneObjectIO.cpp \
    src/Edit/ScenePick.cpp \
    src/Edit/SceneRender.cpp \
    src/Edit/SceneSelection.cpp \
    src/Edit/SceneSnap.cpp \
    src/Edit/SceneSummaryInfo.cpp \
    src/Edit/SceneUndo.cpp \
    src/Edit/SceneUtil.cpp \
    src/Edit/sector.cpp \
    src/Edit/SoundManager_LE.cpp \
    src/Edit/SpawnPoint.cpp \
    src/Edit/UI_LevelMain.cpp \
    src/Edit/UI_LevelTools.cpp \
    src/Edit/WayPoint.cpp \
    src/BottomBar.cpp \
    src/DOOneColor.cpp \
    src/DOShuffle.cpp \
    src/EditLibrary.cpp \
    src/EditLightAnim.cpp \
    src/FrameAIMap.cpp \
    src/FrameDetObj.cpp \
    src/FrameEmitter.cpp \
    src/FrameFogVol.cpp \
    src/FrameGroup.cpp \
    src/FrameLight.cpp \
    src/FrameObject.cpp \
    src/FramePortal.cpp \
    src/FramePS.cpp \
    src/FrameSector.cpp \
    src/FrameShape.cpp \
    src/FrameSpawn.cpp \
    src/FrameWayPoint.cpp \
    src/FrmDBXpacker.cpp \
    src/iniStreamImpl.cpp \
    src/LEClipEditor.cpp \
    src/LeftBar.cpp \
    src/LEPhysics.cpp \
    src/LevelEditor.cpp \
    src/main.cpp \
    src/ObjectList.cpp \
    src/previewimage.cpp \
    src/PropertiesEObject.cpp \
    src/RightForm.cpp \
    src/ShaderFunction.cpp \
    src/Splash.cpp \
    src/stdafx.cpp \
    src/TopBar.cpp \
    main.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    src/Edit/AppendObjectInfoForm.ddp \
    src/BottomBar.ddp \
    src/DOOneColor.ddp \
    src/DOShuffle.ddp \
    src/EditLibrary.ddp \
    src/EditLightAnim.ddp \
    src/FrameAIMap.ddp \
    src/FrameDetObj.ddp \
    src/FrameEmitter.ddp \
    src/FrameFogVol.ddp \
    src/FrameGroup.ddp \
    src/FrameLight.ddp \
    src/FrameObject.ddp \
    src/FramePortal.ddp \
    src/FramePS.ddp \
    src/FrameSector.ddp \
    src/FrameShape.ddp \
    src/FrameSpawn.ddp \
    src/FrameWayPoint.ddp \
    src/FrmDBXpacker.ddp \
    src/LEClipEditor.ddp \
    src/LeftBar.ddp \
    src/LevelEditor.res \
    src/main.ddp \
    src/ObjectList.ddp \
    src/previewimage.ddp \
    src/PropertiesEObject.ddp \
    src/resource.res \
    src/TopBar.ddp \
    src/LevelEditor.bpr \
    src/Edit/AppendObjectInfoForm.dfm \
    src/BottomBar.dfm \
    src/DOOneColor.dfm \
    src/DOShuffle.dfm \
    src/EditLibrary.dfm \
    src/EditLightAnim.dfm \
    src/FrameAIMap.dfm \
    src/FrameDetObj.dfm \
    src/FrameEmitter.dfm \
    src/FrameFogVol.dfm \
    src/FrameGroup.dfm \
    src/FrameLight.dfm \
    src/FrameObject.dfm \
    src/FramePortal.dfm \
    src/FramePS.dfm \
    src/FrameSector.dfm \
    src/FrameShape.dfm \
    src/FrameSpawn.dfm \
    src/FrameWayPoint.dfm \
    src/FrmDBXpacker.dfm \
    src/LEClipEditor.dfm \
    src/LeftBar.dfm \
    src/LevelEditor.todo \
    src/main.dfm \
    src/ObjectList.dfm \
    src/previewimage.dfm \
    src/PropertiesEObject.dfm \
    src/RightForm.dfm \
    src/ShaderFunction.dfm \
    src/Splash.dfm \
    src/TopBar.dfm

HEADERS += \
    src/Edit/AppendObjectInfoForm.h \
    src/Edit/Builder.h \
    src/Edit/Cursor3D.h \
    src/Edit/Custom2DProjector.h \
    src/Edit/CustomObject.h \
    src/Edit/ELight.h \
    src/Edit/ELight_def.h \
    src/Edit/EParticlesObject.h \
    src/Edit/ESceneAIMapControls.h \
    src/Edit/ESceneAIMapTools.h \
    src/Edit/ESceneAIMapTools_Export.h \
    src/Edit/ESceneClassList.h \
    src/Edit/ESceneControlsCustom.h \
    src/Edit/ESceneCustomMTools.h \
    src/Edit/ESceneCustomOTools.h \
    src/Edit/ESceneDOTools.h \
    src/Edit/ESceneDummyTools.h \
    src/Edit/ESceneFogVolumeTools.h \
    src/Edit/ESceneGlowTools.h \
    src/Edit/ESceneGroupControls.h \
    src/Edit/ESceneGroupTools.h \
    src/Edit/ESceneLightTools.h \
    src/Edit/ESceneObjectControls.h \
    src/Edit/ESceneObjectTools.h \
    src/Edit/EScenePortalControls.h \
    src/Edit/EScenePortalTools.h \
    src/Edit/EScenePSControls.h \
    src/Edit/EScenePSTools.h \
    src/Edit/ESceneSectorControls.h \
    src/Edit/ESceneSectorTools.h \
    src/Edit/ESceneShapeControls.h \
    src/Edit/ESceneShapeTools.h \
    src/Edit/ESceneSoundEnvTools.h \
    src/Edit/ESceneSoundSrcTools.h \
    src/Edit/ESceneSpawnControls.h \
    src/Edit/ESceneSpawnTools.h \
    src/Edit/ESceneWallmarkControls.h \
    src/Edit/ESceneWallmarkTools.h \
    src/Edit/ESceneWayControls.h \
    src/Edit/ESceneWayTools.h \
    src/Edit/EShape.h \
    src/Edit/ESound_Environment.h \
    src/Edit/ESound_Source.h \
    src/Edit/FS2.h \
    src/Edit/GeometryPartExtractor.h \
    src/Edit/glow.h \
    src/Edit/GroupObject.h \
    src/Edit/LevelPreferences.h \
    src/Edit/lwchunks.h \
    src/Edit/portal.h \
    src/Edit/portalutils.h \
    src/Edit/scene.h \
    src/Edit/SceneGraph.h \
    src/Edit/SceneObject.h \
    src/Edit/SceneSummaryInfo.h \
    src/Edit/sector.h \
    src/Edit/SoundManager_LE.h \
    src/Edit/SpawnPoint.h \
    src/Edit/UI_LevelMain.h \
    src/Edit/UI_LevelTools.h \
    src/Edit/WayPoint.h \
    src/Engine/communicate.h \
    src/Engine/ShapeData.h \
    src/BottomBar.h \
    src/DOOneColor.h \
    src/DOShuffle.h \
    src/EditLibrary.h \
    src/EditLightAnim.h \
    src/FrameAIMap.h \
    src/FrameDetObj.h \
    src/FrameEmitter.h \
    src/FrameFogVol.h \
    src/FrameGroup.h \
    src/FrameLight.h \
    src/FrameObject.h \
    src/FramePortal.h \
    src/FramePS.h \
    src/FrameSector.h \
    src/FrameShape.h \
    src/FrameSpawn.h \
    src/FrameWayPoint.h \
    src/FrmDBXpacker.h \
    src/iniStreamImpl.h \
    src/LEClipEditor.h \
    src/LeftBar.h \
    src/lephysics.h \
    src/main.h \
    src/mesh_data.h \
    src/ObjectList.h \
    src/previewimage.h \
    src/PropertiesEObject.h \
    src/RightForm.h \
    src/ShaderFunction.h \
    src/Splash.h \
    src/stdafx.h \
    src/TopBar.h
