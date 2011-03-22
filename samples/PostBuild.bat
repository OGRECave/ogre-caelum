setlocal
set TargetDir=%1
set Config=%2

IF NOT DEFINED OGRE_HOME (
	echo "The OGRE_HOME environment variable is not defined."
	exit /b
)

echo "Copying stuff from OGRE_HOME=%OGRE_HOME% to TargetDir=%TargetDir%, Config=%Config%"

IF /i %Config%=="Debug" (
    copy /Y "%OGRE_HOME%\bin\debug\OgreMain_d.dll" %TargetDir%
    copy /Y "%OGRE_HOME%\bin\debug\OIS_d.dll" %TargetDir%
    copy /Y "%OGRE_HOME%\bin\debug\cg.dll" %TargetDir%
    
    REM This file was renamed in trunk; try to copy both.
    copy /Y "%OGRE_HOME%\bin\debug\OgreGUIRenderer_d.dll" %TargetDir%
    copy /Y "%OGRE_HOME%\bin\debug\CEGUIOgreRenderer_d.dll" %TargetDir%
    
    copy /Y "%OGRE_HOME%\bin\debug\CEGUIBase_d.dll" %TargetDir%
    copy /Y "%OGRE_HOME%\bin\debug\CEGUIExpatParser_d.dll" %TargetDir%
    copy /Y "%OGRE_HOME%\bin\debug\CEGUIFalagardWRBase_d.dll" %TargetDir%
    
    copy /Y "%OGRE_HOME%\bin\debug\RenderSystem_Direct3D9_d.dll" %TargetDir%
    copy /Y "%OGRE_HOME%\bin\debug\RenderSystem_GL_d.dll" %TargetDir%
    copy /Y "%OGRE_HOME%\bin\debug\Plugin_CgProgramManager_d.dll" %TargetDir%
    copy /Y "%OGRE_HOME%\bin\debug\Plugin_OctreeSceneManager_d.dll" %TargetDir%

    del %TargetDir%plugins.cfg
    echo PluginFolder=. >> %TargetDir%plugins.cfg
    echo Plugin=Plugin_CgProgramManager_d >> %TargetDir%plugins.cfg
    echo Plugin=Plugin_OctreeSceneManager_d >> %TargetDir%plugins.cfg
    echo Plugin=RenderSystem_GL_d >> %TargetDir%plugins.cfg
    echo Plugin=RenderSystem_Direct3D9_d >> %TargetDir%plugins.cfg
) else (
    copy /Y "%OGRE_HOME%\bin\release\OgreMain.dll" %TargetDir%
    copy /Y "%OGRE_HOME%\bin\release\OIS.dll" %TargetDir%
    copy /Y "%OGRE_HOME%\bin\release\cg.dll" %TargetDir%
    
    REM This file was renamed in trunk; try to copy both.
    copy /Y "%OGRE_HOME%\bin\release\OgreGUIRenderer.dll" %TargetDir%
    copy /Y "%OGRE_HOME%\bin\release\CEGUIOgreRenderer.dll" %TargetDir%
    
    copy /Y "%OGRE_HOME%\bin\release\CEGUIBase.dll" %TargetDir%
    copy /Y "%OGRE_HOME%\bin\release\CEGUIExpatParser.dll" %TargetDir%
    copy /Y "%OGRE_HOME%\bin\release\CEGUIFalagardWRBase.dll" %TargetDir%
    
    copy /Y "%OGRE_HOME%\bin\release\RenderSystem_Direct3D9.dll" %TargetDir%
    copy /Y "%OGRE_HOME%\bin\release\RenderSystem_GL.dll" %TargetDir%
    copy /Y "%OGRE_HOME%\bin\release\Plugin_CgProgramManager.dll" %TargetDir%
    copy /Y "%OGRE_HOME%\bin\release\Plugin_OctreeSceneManager.dll" %TargetDir%

    del %TargetDir%plugins.cfg
    echo PluginFolder=. >> %TargetDir%plugins.cfg
    echo Plugin=Plugin_CgProgramManager >> %TargetDir%plugins.cfg
    echo Plugin=Plugin_OctreeSceneManager >> %TargetDir%plugins.cfg
    echo Plugin=RenderSystem_GL >> %TargetDir%plugins.cfg
    echo Plugin=RenderSystem_Direct3D9 >> %TargetDir%plugins.cfg
)

del %TargetDir%resources.cfg
echo [Bootstrap] >> %TargetDir%resources.cfg
echo Zip=..\..\samples\resources\OgreCore.zip >> %TargetDir%resources.cfg

echo [Caelum] >> %TargetDir%resources.cfg
echo FileSystem=..\..\main\resources\ >> %TargetDir%resources.cfg

echo [Samples] >> %TargetDir%resources.cfg
echo FileSystem=..\..\samples\resources\ >> %TargetDir%resources.cfg
echo FileSystem=..\..\samples\resources\trees >> %TargetDir%resources.cfg

echo [CEGUI] >> %TargetDir%resources.cfg
echo FileSystem=..\..\samples\resources\gui\ >> %TargetDir%resources.cfg

endlocal
