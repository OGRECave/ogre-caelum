/*
This file is part of Caelum.
See http://www.ogre3d.org/wiki/index.php/Caelum 

Copyright (c) 2008 Caelum team. See Contributors.txt for details.

Caelum is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published
by the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Caelum is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with Caelum. If not, see <http://www.gnu.org/licenses/>.
*/

#include "CaelumLab.h"

CaelumLabFrameListener::CaelumLabFrameListener (RenderWindow* win, Camera* cam):
        ExampleFrameListener (win, cam, true, true),
        mSceneMgr (cam->getSceneManager ()),
        mShutdownRequested (false),
        mLastMousePositionSet (false),
        mMaterialScheme (SMS_DEFAULT),
        mTerrainShadowsOn (false),
        mScriptingUI (this)
{
    mMouse->setEventCallback (this);
    mKeyboard->setEventCallback (this);
    initGui ();
    initCaelum ();
}

CaelumLabFrameListener::~CaelumLabFrameListener() {
    destroyCaelum ();
}

static void logUnsupportedException (Caelum::UnsupportedException& ex) {
    Ogre::LogManager::getSingletonPtr()->logMessage("CaelumLab: UnsupportedException: " + ex.getFullDescription ());
}

void CaelumLabFrameListener::initCaelum ()
{
    // Initialise Caelum
    mCaelumSystem.reset(new CaelumSystem (
            Root::getSingletonPtr(),
            mSceneMgr,
            CaelumSystem::CAELUM_COMPONENTS_NONE));

    // Create subcomponents (and allow individual subcomponents to fail).
    try {
        mCaelumSystem->setSkyDome (new SkyDome (mSceneMgr, mCaelumSystem->getCaelumCameraNode ()));
    } catch (Caelum::UnsupportedException& ex) {
        logUnsupportedException (ex);
    }
    try {
        mCaelumSystem->setSun (new Caelum::SphereSun(mSceneMgr, mCaelumSystem->getCaelumCameraNode ()));
    } catch (Caelum::UnsupportedException& ex) {
        logUnsupportedException (ex);
    }
    try {
        mCaelumSystem->setMoon (new Caelum::Moon(mSceneMgr, mCaelumSystem->getCaelumCameraNode ()));
    } catch (Caelum::UnsupportedException& ex) {
        logUnsupportedException (ex);
    }
    try {
        mCaelumSystem->setCloudSystem (new Caelum::CloudSystem (mSceneMgr, mCaelumSystem->getCaelumGroundNode ()));
    } catch (Caelum::UnsupportedException& ex) {
        logUnsupportedException (ex);
    }
    try {
        mCaelumSystem->setPointStarfield (new PointStarfield (mSceneMgr, mCaelumSystem->getCaelumCameraNode ()));
    } catch (Caelum::UnsupportedException& ex) {
        logUnsupportedException (ex);
    }

    // Register caelum.
    // Don't make it a frame listener; update it by hand.
    //Root::getSingletonPtr ()->addFrameListener (mCaelumSystem.get ());
    mCaelumSystem->attachViewport (getViewport ());

    try {
        mCaelumSystem->setPrecipitationController (new PrecipitationController (mSceneMgr));
    } catch (Caelum::UnsupportedException& ex) {
        logUnsupportedException (ex);
    }

    mCaelumSystem->setSceneFogDensityMultiplier (0.0015);
    mCaelumSystem->setManageAmbientLight (true);
    mCaelumSystem->setMinimumAmbientLight (Ogre::ColourValue (0.1, 0.1, 0.1));

    // Test spinning the caelum root node. Looks wrong in the demo;
    // but at least the sky components are aligned with each other.
    if (false) {
        mCaelumSystem->getCaelumCameraNode ()->setOrientation(
                Ogre::Quaternion (Ogre::Radian (Ogre::Math::PI), Ogre::Vector3::UNIT_Z) *
                Ogre::Quaternion (Ogre::Radian (Ogre::Math::PI / 2), Ogre::Vector3::UNIT_X));
        mCaelumSystem->getCaelumCameraNode ()->_update (true, true);
    }

    // Setup sun options
    if (mCaelumSystem->getSun ()) {
        // Make the sun very obvious:
        //mCaelumSystem->getSun ()->setDiffuseMultiplier (Ogre::ColourValue (1, 10, 1));

        mCaelumSystem->getSun ()->setAutoDisableThreshold (0.05);
        mCaelumSystem->getSun ()->setAutoDisable (false);
    }

    if (mCaelumSystem->getMoon ()) {
        // Make the moon very obvious:
        //mCaelumSystem->getMoon ()->setDiffuseMultiplier (Ogre::ColourValue (1, 1, 10));

        mCaelumSystem->getMoon ()->setAutoDisableThreshold (0.05);
        mCaelumSystem->getMoon ()->setAutoDisable (false);
    }

    if (mCaelumSystem->getCloudSystem ()) {
        try {
            mCaelumSystem->getCloudSystem ()->createLayerAtHeight(2000);
            mCaelumSystem->getCloudSystem ()->createLayerAtHeight(5000);
            mCaelumSystem->getCloudSystem ()->getLayer(0)->setCloudSpeed(Ogre::Vector2(0.000005, -0.000009));
            mCaelumSystem->getCloudSystem ()->getLayer(1)->setCloudSpeed(Ogre::Vector2(0.0000045, -0.0000085));
        } catch (Caelum::UnsupportedException& ex) {
            logUnsupportedException (ex);
        }
    }

    if (mCaelumSystem->getPrecipitationController ()) {
        mCaelumSystem->getPrecipitationController ()->setIntensity (0);
    }

    // Set time acceleration.
    //mCaelumSystem->getUniversalClock ()->setTimeScale (0);

    // Sunrise with visible moon.
    mCaelumSystem->getUniversalClock ()->setGregorianDateTime (2007, 4, 9, 9, 33, 0);
}

void CaelumLabFrameListener::destroyCaelum ()
{
	mCaelumSystem.reset();
}

void CaelumLabFrameListener::initGui () {
    mGuiRenderer.reset(new CEGUI::OgreCEGUIRenderer (
            mWindow, Ogre::RENDER_QUEUE_OVERLAY, false, 3000, mSceneMgr));
    mGuiSystem.reset(new CEGUI::System (mGuiRenderer.get()));

    CEGUI::Logger::getSingleton ().setLoggingLevel (CEGUI::Informative);

    // load scheme and set up defaults
    CEGUI::SchemeManager::getSingleton ().loadScheme((CEGUI::utf8*)"TaharezLookSkin.scheme");
    mGuiSystem->setDefaultMouseCursor ((CEGUI::utf8*)"TaharezLook", (CEGUI::utf8*)"MouseArrow");
    // BlueHighway-10 does not rescale with resolution. This is desirable.
    mGuiSystem->setDefaultFont ((CEGUI::utf8*)"BlueHighway-10");

    CEGUI::WindowManager* wmgr = CEGUI::WindowManager::getSingletonPtr ();
    CEGUI::Window* sheet = wmgr->loadWindowLayout (CEGUI::String("CaelumLab.layout")); 
    mGuiSystem->setGUISheet (sheet);

    CEGUI::Window* wnd = CEGUI::WindowManager::getSingleton().getWindow("CaelumLab");
    wnd->subscribeEvent(CEGUI::Window::EventMouseMove, 
            CEGUI::Event::Subscriber(&CaelumLabFrameListener::handleMouseMove, this));
    wnd->subscribeEvent(CEGUI::Window::EventMouseButtonUp, 
            CEGUI::Event::Subscriber(&CaelumLabFrameListener::handleMouseUp, this));
    wnd->subscribeEvent(CEGUI::Window::EventMouseButtonDown, 
            CEGUI::Event::Subscriber(&CaelumLabFrameListener::handleMouseDown, this));
    (getWidget<CEGUI::Scrollbar> ("CaelumLab/TimeScaleScrollbar"))->getThumb ()->subscribeEvent(
            CEGUI::Window::EventMouseDoubleClick,
            CEGUI::Event::Subscriber(&CaelumLabFrameListener::handleTimeScaleDoubleClick, this));
    (getWidget<CEGUI::Scrollbar> ("CaelumLab/DumpButton"))->subscribeEvent(
            CEGUI::Window::EventMouseClick,
            CEGUI::Event::Subscriber(&CaelumLabFrameListener::handleDumpClick, this));
    (getWidget<CEGUI::Scrollbar> ("CaelumLab/ResetButton"))->subscribeEvent(
            CEGUI::Window::EventMouseClick,
            CEGUI::Event::Subscriber(&CaelumLabFrameListener::handleResetClick, this));
    (getWidget<CEGUI::Scrollbar> ("CaelumLab/QuitButton"))->subscribeEvent(
            CEGUI::Window::EventMouseClick,
            CEGUI::Event::Subscriber(&CaelumLabFrameListener::handleQuitClick, this));

    mScriptingUI.init ();
    initMaterialSchemeUI ();
}

CEGUI::MouseButton convertOISMouseButtonToCegui (int buttonID) {
    switch (buttonID) {
        case 0: return CEGUI::LeftButton;
        case 1: return CEGUI::RightButton;
        case 2:	return CEGUI::MiddleButton;
        case 3: return CEGUI::X1Button;
        default: return CEGUI::LeftButton;
    }
}

bool CaelumLabFrameListener::mouseMoved (const OIS::MouseEvent &arg) {
    CEGUI::System::getSingleton ().injectMouseMove (arg.state.X.rel, arg.state.Y.rel);
    return true;
}

bool CaelumLabFrameListener::mousePressed (const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
    CEGUI::System::getSingleton ().injectMouseButtonDown (convertOISMouseButtonToCegui (id));
    return true;
}

bool CaelumLabFrameListener::mouseReleased (const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
    CEGUI::System::getSingleton ().injectMouseButtonUp (convertOISMouseButtonToCegui (id));
    return true;
}

bool CaelumLabFrameListener::keyPressed (const OIS::KeyEvent &arg)
{
    if (arg.key == OIS::KC_ESCAPE) {
        mShutdownRequested = true;
    }
    CEGUI::System::getSingleton ().injectKeyDown (arg.key);
    CEGUI::System::getSingleton ().injectChar (arg.text);
    return true;
}

bool CaelumLabFrameListener::keyReleased (const OIS::KeyEvent &arg)
{
    CEGUI::System::getSingleton ().injectKeyUp (arg.key);
    return true;
}

bool CaelumLabFrameListener::handleMouseMove(const CEGUI::EventArgs& evt) {
    const CEGUI::MouseEventArgs& me = static_cast<const CEGUI::MouseEventArgs&>(evt);

    // Rotate camera
    if (mMouse->getMouseState ().buttonDown (OIS::MB_Right)) {
        // Timing does not matter for RotX/RotY; only mouse motion.
        mRotX += Ogre::Degree(-me.moveDelta.d_x * 0.5);
        mRotY += Ogre::Degree(-me.moveDelta.d_y * 0.5);
        CEGUI::MouseCursor::getSingleton ().setPosition (mLastMousePosition);
    }

    return true;
}

bool CaelumLabFrameListener::handleMouseUp(const CEGUI::EventArgs& evt) {
    //const CEGUI::MouseEventArgs& me = static_cast<const CEGUI::MouseEventArgs&>(evt);

    if (!rightMouseDown()) {
        CEGUI::MouseCursor::getSingleton ().show ();
        if (mLastMousePositionSet) {
            CEGUI::MouseCursor::getSingleton ().setPosition (mLastMousePosition);
            mLastMousePositionSet = false;
        }
        CEGUI::WindowManager::getSingleton ().getWindow("CaelumLab")->releaseInput ();
    }

    return true;
}

bool CaelumLabFrameListener::handleMouseDown(const CEGUI::EventArgs& evt) {
    //const CEGUI::MouseEventArgs& me = static_cast<const CEGUI::MouseEventArgs&>(evt);

    if (rightMouseDown()) {
        CEGUI::MouseCursor::getSingleton ().hide ();
        if (!mLastMousePositionSet) {
            mLastMousePosition = CEGUI::MouseCursor::getSingleton ().getPosition ();
            mLastMousePositionSet = true;
        }
        CEGUI::WindowManager::getSingleton ().getWindow("CaelumLab")->captureInput ();
    }

    return true;
}

const String formatTime(int year, int month, int day, int hour, int minute, int second) {
    return StringConverter::toString(year, 4, '0') + "-" +
           StringConverter::toString(month, 2, '0') + "-" +
           StringConverter::toString(day, 2, '0') + "T" +
           StringConverter::toString(hour, 2, '0') + ":" +
           StringConverter::toString(minute, 2, '0') + ":" +
           StringConverter::toString(second, 2, '0');
}

const String formatTimeFromJDay (LongReal jday) {
    int year, month, day, hour, minute;
    LongReal second;
    Astronomy::getGregorianDateTimeFromJulianDay(jday, year, month, day, hour, minute, second);
    return formatTime(year, month, day, hour, minute, static_cast<int>(second));
}

const String formatDegMinSec (Ogre::Degree angle) {
    Real aa = Math::Abs(angle.valueDegrees());
    String sign = (angle.valueDegrees() < 0) ? "-" : "";
    int sec = (int)(aa * 3600);
    return sign +
            StringConverter::toString(sec / 3600, 2, '0') +  "\xB0" +
            StringConverter::toString(sec / 60 % 60, 2, '0') + "`" + 
            StringConverter::toString(sec % 60, 2, '0') + "``";
}

const String formatHorizDirection (Ogre::Degree azm, Ogre::Degree alt) {
    //Real az = azm.valueDegrees();
    //Real al = alt.valueDegrees();
    Real az = fmod(fmod(azm.valueDegrees(), 360) + 360, 360);
    assert(0 <= az && az <= 360);
    Real al = fmod(fmod(alt.valueDegrees() + 180, 360) - 180, 360);
    assert(-180 <= al && al <= 180);
    return "azm " + formatDegMinSec(Degree(az)) + " alt " + formatDegMinSec(Degree(al));
}

const String formatHorizDirection (Vector3 dir) {
    LongReal theta, phi, r;
    Astronomy::convertRectangularToSpherical(
                -dir.z, dir.x, -dir.y,
                theta, phi, r);
    return formatHorizDirection(Degree(theta), Degree(phi));
}

bool CaelumLabFrameListener::updateInput (const FrameEvent& evt)
{
    // Keyboard input; should probably replace ExampleFrameListener calls
    if (evt.timeSinceLastFrame == 0) {
        mMoveScale = 1;
    } else {
        mMoveScale = mMoveSpeed * evt.timeSinceLastFrame;
    }
    if (!processUnbufferedKeyInput (evt)) {
        return false;
    }

    /*
    // Precise per-frame magic.
    LogManager::getSingleton().getDefaultLog()->logMessage(
			"Caelum lab"
			" update seconds: " + StringConverter::toString(evt.timeSinceLastFrame, 10) +
            " move scale = " + StringConverter::toString(mMoveScale, 10) +
            " move speed = " + StringConverter::toString(mMoveSpeed, 10) +
            " move relative = " + StringConverter::toString(mTranslateVector));
    */

    // Do the actual motion.
    mCamera->yaw(mRotX);
    mCamera->pitch(mRotY);
    mCamera->moveRelative(mTranslateVector);

    // Clear motion.
    mRotX = 0;
    mRotY = 0;
    mTranslateVector = Ogre::Vector3::ZERO;

    return true;
}

void CaelumLabFrameListener::dumpRenderTargets ()
{
    // This does not include RTT strangely
    Ogre::RenderSystem::RenderTargetIterator rtit =
            Ogre::Root::getSingletonPtr()->getRenderSystem()->getRenderTargetIterator();
    while (rtit.hasMoreElements()) {
        Ogre::RenderTarget* rt = rtit.getNext();
        Ogre::String name = rt->getName();
        std::replace(name.begin(), name.end(), '/', '_');
        Ogre::String fileName = rt->writeContentsToTimestampedFile("RTDump ", " " + name + ".png");
        Ogre::LogManager::getSingletonPtr()->logMessage("Written render target " + rt->getName() + " to " + fileName);
    }
}

void CaelumLabFrameListener::dumpRenderQueueShadows ()
{
    RenderQueue::QueueGroupIterator it = mSceneMgr->getRenderQueue()->_getQueueGroupIterator();
    while (it.hasMoreElements()) {
        int key = it.peekNextKey();
        RenderQueueGroup* rqg = it.getNext();
        Ogre::LogManager::getSingletonPtr()->logMessage(
                "Render queue group " + StringConverter::toString(key) +
                " shadows " + StringConverter::toString(rqg->getShadowsEnabled()));
    }
    Ogre::LogManager::getSingletonPtr()->logMessage(
            "World geometry render queue group " + 
            StringConverter::toString(mSceneMgr->getWorldGeometryRenderQueue()));
}

void CaelumLabFrameListener::dumpMaterialNames ()
{
    MaterialManager::ResourceMapIterator it = Ogre::MaterialManager::getSingleton ().getResourceIterator ();
    while (it.hasMoreElements()) {
        Ogre::ResourcePtr res = it.getNext();
        Ogre::MaterialPtr mat = static_cast<Ogre::MaterialPtr>(res);
        Ogre::LogManager::getSingletonPtr()->logMessage(
                "Material name " + mat->getName());
    }
}

bool CaelumLabFrameListener::handleDumpClick (const CEGUI::EventArgs& args)
{
    //dumpRenderTargets ();
    dumpMaterialNames ();
    //dumpRenderQueueShadows ();

    return true;
}


bool CaelumLabFrameListener::handleResetClick (const CEGUI::EventArgs& args)
{
    destroyCaelum ();
    initCaelum ();

    return true;
}

bool CaelumLabFrameListener::handleQuitClick (const CEGUI::EventArgs& args)
{
    mShutdownRequested = true;

    return true;
}

template <typename T>
inline T clamp(const T& value, const T& minValue, const T& maxValue) {
    return std::min(maxValue, std::max(minValue, value));
}

struct ExpSliderMapper
{
    Real MinValue;
    Real MaxValue;
    Real MagicValue;
    Real MagicPosition;
    Real Exponent;

    Real valueToPosition (Real value)
    {
        assert(MinValue <= MagicValue && MagicValue <= MaxValue);
        assert(0 <= MagicPosition && MagicPosition <= 1);

        value = clamp (value, MinValue, MaxValue);

        Real relPosition;
        if (value < MagicValue) {
            relPosition = pow ((MagicValue - value) / (MagicValue - MinValue), 1 / Exponent);
        } else {
            relPosition = pow ((value - MagicValue) / (MaxValue - MagicValue), 1 / Exponent);
        }
        relPosition = clamp<Real> (relPosition, 0, 1);

        Real position;
        if (value < MagicValue) {
            position = (1 - relPosition) * MagicPosition;
        } else {
            position = relPosition * (1 - MagicPosition) + MagicPosition;
        }
        position = clamp<Real> (position, 0, 1);

        return position;
    }

    Real positionToValue (Real position)
    {
        assert(MinValue <= MagicValue && MagicValue <= MaxValue);
        assert(0 <= MagicPosition && MagicPosition <= 1);

        position = clamp<Real> (position, 0, 1);

        Real relValue;
        if (position < MagicPosition) {
            relValue = pow ((MagicPosition - position) / MagicPosition, Exponent);
        } else {
            relValue = pow ((position - MagicPosition) / (1 - MagicPosition), Exponent);
        }
        relValue = clamp<Real> (relValue, 0, 1);

        Real value;
        if (position < MagicPosition) {
            value = MagicValue + (MinValue - MagicValue) * relValue;
        } else {
            value = MagicValue + (MaxValue - MagicValue) * relValue;
        }
        value = clamp (value, MinValue, MaxValue);

        return value;
    }
};

bool CaelumLabFrameListener::updateScrollbarTweak (
        const Ogre::String& editboxName,
        const Ogre::String& scrollbarName,
        Real& value,
        Real minValue,
        Real maxValue,
        Real magicValue,
        Real magicPosition,
        Real exponent)
{
    CEGUI::Editbox* editbox = getWidget<CEGUI::Editbox> (editboxName.c_str());
    CEGUI::Scrollbar* scrollbar = getWidget<CEGUI::Scrollbar> (scrollbarName.c_str());

    ExpSliderMapper mapper = { minValue, maxValue, magicValue, magicPosition, exponent };
    if (magicPosition < 0) {
        magicValue = minValue + maxValue / 2;
        magicPosition = 0.5;
        exponent = 1;
    }
    if (editbox->hasInputFocus ()) {
        value = StringConverter::parseReal(String(editbox->getText ().c_str()));
        value = std::max(minValue, std::min(maxValue, value));
        scrollbar->setScrollPosition (mapper.valueToPosition(value));
        return true;
    } else if (scrollbar->getThumb ()->isPushed ()) {
        assert(scrollbar->getPageSize() == 0);
        assert(scrollbar->getDocumentSize() == 1);
        value = mapper.positionToValue (scrollbar->getScrollPosition ());
        editbox->setText (StringConverter::toString (value));
        return true;
    } else {
        editbox->setText (StringConverter::toString (value));
        scrollbar->setScrollPosition (mapper.valueToPosition(value));
        return false;
    }
}

bool CaelumLabFrameListener::updateScrollbarTweak (
        const Ogre::String& editboxName,
        const Ogre::String& scrollbarName,
        Real& value,
        Real minValue,
        Real maxValue)
{
    // Pass through with dummy exponential parameters.
    return updateScrollbarTweak(
            editboxName, scrollbarName,
            value, minValue, maxValue,
            (minValue + maxValue) / 2, 0.5, 1);
}

bool CaelumLabFrameListener::handleTimeScaleDoubleClick (const CEGUI::EventArgs& args)
{
    mCaelumSystem->getUniversalClock ()->setTimeScale (1);
    getWidget<CEGUI::Editbox> ("CaelumLab/TimeScaleEditbox")->setText ("1");
    getWidget<CEGUI::Scrollbar> ("CaelumLab/TimeScaleScrollbar")->setScrollPosition (0.5f);

    return true;
}

void CaelumLabFrameListener::updateWidgets ()
{
    LongReal jday = mCaelumSystem->getUniversalClock ()->getJulianDay ();

    // Time
    { 
        // Bind julian day. The precission of ogre's string converter is not enough here.
        CEGUI::Editbox* julianDayEditbox = getWidget<CEGUI::Editbox> ("CaelumLab/JulianDayEditbox");
        if (julianDayEditbox->hasInputFocus ()) {
            std::stringstream jdaystr (std::string(julianDayEditbox->getText ().c_str()));
            jdaystr >> jday;
            jday = std::max(-10000000.0, std::min(10000000.0, jday));
            mCaelumSystem->getUniversalClock ()->setJulianDay (jday);
        } else {
            std::stringstream jdaystr;
            jdaystr.precision(15);
            jdaystr << jday;
            julianDayEditbox->setText (jdaystr.str());
        }
        getWidget<CEGUI::Editbox> ("CaelumLab/UtcTimeEditbox")->setText (formatTimeFromJDay(jday));

        // Bind time scale
        Real mul = mCaelumSystem->getUniversalClock ()->getTimeScale ();
        if (updateScrollbarTweak(
                "CaelumLab/TimeScaleEditbox",
                "CaelumLab/TimeScaleScrollbar",
                mul, -10000, 10000, 1, 0.5, 3))
        {
            mCaelumSystem->getUniversalClock ()->setTimeScale (mul);
        }
    }

    // Camera
    {
        getWidget<CEGUI::Editbox> ("CaelumLab/CameraPositionEditbox")->setText (
                StringConverter::toString (mCamera->getDerivedPosition ()));
        getWidget<CEGUI::Editbox> ("CaelumLab/CameraDirectionEditbox")->setText (
                formatHorizDirection (-mCamera->getDerivedDirection()));

        updateScrollbarTweak("CaelumLab/CameraSpeedEditbox", "CaelumLab/CameraSpeedScrollbar",
                mMoveSpeed, 0.05, 500, 0.05, 0, 3);

        Real fov = mCamera->getFOVy ().valueDegrees ();
        updateScrollbarTweak("CaelumLab/CameraFovEditbox", "CaelumLab/CameraFovScrollbar",
                fov, 1, 120);
        mCamera->setFOVy (Ogre::Degree(fov));

        bool clipDistancesModified = false;

        Real nearClip = mCamera->getNearClipDistance ();
        clipDistancesModified |= updateScrollbarTweak(
                "CaelumLab/CameraNearClipEditbox",
                "CaelumLab/CameraNearClipScrollbar",
                nearClip, 0.00001, 500, 0.00001, 0, 5);
        mCamera->setNearClipDistance (nearClip);

        Real farClip = mCamera->getFarClipDistance ();
        clipDistancesModified |= updateScrollbarTweak(
                "CaelumLab/CameraFarClipEditbox",
                "CaelumLab/CameraFarClipScrollbar",
                farClip, 0, 50000, 0, 0, 4);
        mCamera->setFarClipDistance (farClip);

        // See the effect of clip distances on PSSM in real-time.
        if (clipDistancesModified) {
#if OGRE_VERSION >= 0x010600
            if (mMaterialScheme == SMS_PSSM) {
                updatePSSMCamera();
            }
#endif
        }
    }

    // Sun stuff. Add more here.
    if (mCaelumSystem->getSun ()) {
        getWidget<CEGUI::Editbox> ("CaelumLab/SunDirectionEditbox")->setText (
                formatHorizDirection (mCaelumSystem->getSunDirection (jday)));
        if (getWidget<CEGUI::PushButton> ("CaelumLab/SunDirectionFocusButton")->isPushed()) {
            mCamera->setDirection (-mCaelumSystem->getSun ()->getLightDirection ());
        }
    }

    // Moon stuff
    if (mCaelumSystem->getMoon ()) {
        Ogre::Vector3 moondir = mCaelumSystem->getMoonDirection (jday);
        getWidget<CEGUI::Editbox> ("CaelumLab/MoonDirectionEditbox")->setText (
                formatHorizDirection (moondir));
        getWidget<CEGUI::Editbox> ("CaelumLab/MoonPhaseEditbox")->setText (
                StringConverter::toString (mCaelumSystem->getMoonPhase (jday)));
        getWidget<CEGUI::Editbox> ("CaelumLab/MoonLightEditbox")->setText (
                StringConverter::toString (mCaelumSystem->getMoonLightColour (moondir)));
        if (getWidget<CEGUI::PushButton> ("CaelumLab/MoonDirectionFocusButton")->isPushed()) {
            mCamera->setDirection (-mCaelumSystem->getMoon ()->getLightDirection ());
        }
    }

    // Clouds
	if (mCaelumSystem->getCloudSystem ())
    {
        if (mCaelumSystem->getCloudSystem ()->getLayerCount() > 0) {
            Real cover1 = mCaelumSystem->getCloudSystem ()->getLayer (0)->getCloudCover ();
            updateScrollbarTweak("CaelumLab/CloudLayer1CoverEditbox", "CaelumLab/CloudLayer1CoverScrollbar",
                    cover1, 0, 1);
            mCaelumSystem->getCloudSystem ()->getLayer (0)->setCloudCover (cover1);
            Real height1 = mCaelumSystem->getCloudSystem ()->getLayer (0)->getHeight ();
            updateScrollbarTweak("CaelumLab/CloudLayer1HeightEditbox", "CaelumLab/CloudLayer1HeightScrollbar",
                    height1, 100, 15000);
            mCaelumSystem->getCloudSystem ()->getLayer (0)->setHeight (height1);
        }

        if (mCaelumSystem->getCloudSystem ()->getLayerCount() > 1) {
            Real cover2 = mCaelumSystem->getCloudSystem ()->getLayer (1)->getCloudCover ();
            updateScrollbarTweak("CaelumLab/CloudLayer2CoverEditbox", "CaelumLab/CloudLayer2CoverScrollbar",
                    cover2, 0, 1);
            mCaelumSystem->getCloudSystem ()->getLayer (1)->setCloudCover (cover2);
            Real height2 = mCaelumSystem->getCloudSystem ()->getLayer (1)->getHeight ();
            updateScrollbarTweak("CaelumLab/CloudLayer2HeightEditbox", "CaelumLab/CloudLayer2HeightScrollbar",
                    height2, 100, 15000);
            mCaelumSystem->getCloudSystem ()->getLayer (1)->setHeight (height2);
        }
	}	

    // Starfield
    if (mCaelumSystem->getPointStarfield ())
    {
        Real magScale = mCaelumSystem->getPointStarfield ()->getMagnitudeScale ();
        updateScrollbarTweak("CaelumLab/StarMagScaleEditbox", "CaelumLab/StarMagScaleScrollbar",
                magScale, 1, 5);
        mCaelumSystem->getPointStarfield ()->setMagnitudeScale (magScale);

        Real mag0size = mCaelumSystem->getPointStarfield ()->getMag0PixelSize ();
        updateScrollbarTweak("CaelumLab/StarMag0SizeEditbox", "CaelumLab/StarMag0SizeScrollbar",
                mag0size, 0, 50);
        mCaelumSystem->getPointStarfield ()->setMag0PixelSize (mag0size);

        Real minSize = mCaelumSystem->getPointStarfield ()->getMinPixelSize ();
        updateScrollbarTweak("CaelumLab/StarMinSizeEditbox", "CaelumLab/StarMinSizeScrollbar",
                minSize, 0, 50);
        mCaelumSystem->getPointStarfield ()->setMinPixelSize (minSize);

        Real maxSize = mCaelumSystem->getPointStarfield ()->getMaxPixelSize ();
        updateScrollbarTweak("CaelumLab/StarMaxSizeEditbox", "CaelumLab/StarMaxSizeScrollbar",
                maxSize, 0, 50);
        mCaelumSystem->getPointStarfield ()->setMaxPixelSize (maxSize);
    }

    // Precipitation.
	if (PrecipitationController* prec = mCaelumSystem->getPrecipitationController ())
    {
        // A combo would be much better here.
		Real typeReal = prec->getPresetType ();
		updateScrollbarTweak(
                "CaelumLab/PrecipitationTypeEditbox",
                "CaelumLab/PrecipitationTypeScrollbar",
                typeReal, 0, 8);
        Caelum::PrecipitationType typeEnum = static_cast<Caelum::PrecipitationType>(static_cast<int>(typeReal));
        if (prec->getPresetType () != typeEnum && PrecipitationController::isPresetType(typeEnum)) {
			prec->setPresetType (typeEnum);
        }

		Real speed = prec->getSpeed ();
		if (updateScrollbarTweak (
                "CaelumLab/PrecipitationSpeedEditbox",
                "CaelumLab/PrecipitationSpeedScrollbar",
                speed, 0, 1))
        {
		    prec->setSpeed (speed);
        }

	    Real intensity = prec->getIntensity ();
		updateScrollbarTweak (
				"CaelumLab/PrecipitationIntensityEditbox",
				"CaelumLab/PrecipitationIntensityScrollbar",
                intensity, 0, 1);
		prec->setIntensity (intensity);
	}

    updateDepthComposerWidgets ();
    updateMaterialSchemeUI ();
}

void CaelumLabFrameListener::initDepthComposer ()
{
    mCaelumSystem->setDepthComposer (new DepthComposer (mSceneMgr));

    DepthComposerInstance* inst = mCaelumSystem->getDepthComposer ()->getViewportInstance (getViewport ());
    //inst->getDepthRenderer()->setRenderGroupRangeFilter (20, 80);
    inst->getDepthRenderer()->setViewportVisibilityMask (~0x00001000);
    mCaelumSystem->forceSubcomponentVisibilityFlags (0x00001000);

    mCaelumSystem->setGroundFogDensityMultiplier (0.03);
    mCaelumSystem->getDepthComposer ()->setGroundFogVerticalDecay (0.06);
    mCaelumSystem->getDepthComposer ()->setGroundFogBaseLevel (0);

    initDepthComposerWidgets ();
    updateDepthComposerWidgets ();
}

void CaelumLabFrameListener::setupDepthComposerFlags ()
{
    assert (mCaelumSystem->getDepthComposer () && "No depth composer");
    DepthComposerInstance* inst = mCaelumSystem->getDepthComposer ()->getViewportInstance (getViewport ());
    //inst->getDepthRenderer()->setRenderGroupRangeFilter (20, 80);
    inst->getDepthRenderer()->setViewportVisibilityMask (~0x00001000);
    mCaelumSystem->forceSubcomponentVisibilityFlags (0x00001000);
}

void CaelumLabFrameListener::disableDepthComposer ()
{
    mCaelumSystem->setDepthComposer (0);
}

void CaelumLabFrameListener::initDepthComposerWidgets ()
{
    CEGUI::Checkbox* cbEnabled = getWidget<CEGUI::Checkbox> (
            "CaelumLab/DepthComposer/EnabledCheckbox");
    CEGUI::Checkbox* cbDebug = getWidget<CEGUI::Checkbox> (
            "CaelumLab/DepthComposer/DebugDepthRenderCheckbox");
    CEGUI::Checkbox* cbSkyDomeHaze = getWidget<CEGUI::Checkbox> (
            "CaelumLab/DepthComposer/SkyDomeHazeCheckbox");
    CEGUI::Checkbox* cbExpGroundFog = getWidget<CEGUI::Checkbox> (
            "CaelumLab/DepthComposer/GroundFogCheckbox");

    Caelum::DepthComposer* composer = mCaelumSystem->getDepthComposer();
    cbEnabled->setSelected (composer != 0);
    cbDebug->setSelected (composer != 0 && composer->getDebugDepthRender ());
    cbSkyDomeHaze->setSelected (composer != 0 && composer->getSkyDomeHazeEnabled ());
    cbExpGroundFog->setSelected (composer != 0 && composer->getGroundFogEnabled ());
}

void CaelumLabFrameListener::updateDepthComposerWidgets ()
{
    CEGUI::Checkbox* cbEnabled = getWidget<CEGUI::Checkbox> (
            "CaelumLab/DepthComposer/EnabledCheckbox");
    CEGUI::Checkbox* cbDebug = getWidget<CEGUI::Checkbox> (
            "CaelumLab/DepthComposer/DebugDepthRenderCheckbox");
    CEGUI::Checkbox* cbSkyDomeHaze = getWidget<CEGUI::Checkbox> (
            "CaelumLab/DepthComposer/SkyDomeHazeCheckbox");
    CEGUI::Checkbox* cbExpGroundFog = getWidget<CEGUI::Checkbox> (
            "CaelumLab/DepthComposer/GroundFogCheckbox");

    if (cbEnabled->isSelected () == true && mCaelumSystem->getDepthComposer () == 0) {
        initDepthComposer ();
    }
    if (cbEnabled->isSelected () != true && mCaelumSystem->getDepthComposer () != 0) {
        disableDepthComposer ();
    }

    bool enableCompositor = mCaelumSystem->getDepthComposer () != 0;
    cbDebug->setEnabled (enableCompositor);
    bool debugDepthRender = (cbDebug->isSelected() && !cbDebug->isDisabled());
    cbSkyDomeHaze->setEnabled (enableCompositor && !debugDepthRender);
    cbExpGroundFog->setEnabled (enableCompositor && !debugDepthRender);
    bool enableExpGroundFog = (cbExpGroundFog->isSelected() && !cbExpGroundFog->isDisabled());
    bool enableSkyDomeHaze = (cbSkyDomeHaze->isSelected() && !cbSkyDomeHaze->isDisabled());

    getWindow ("CaelumLab/DepthComposer/GroundFogDensityLabel")->setEnabled (enableExpGroundFog);
    getWindow ("CaelumLab/DepthComposer/GroundFogDensityEditbox")->setEnabled (enableExpGroundFog);
    getWindow ("CaelumLab/DepthComposer/GroundFogDensityScrollbar")->setEnabled (enableExpGroundFog);
    getWindow ("CaelumLab/DepthComposer/GroundFogVerticalDecayLabel")->setEnabled (enableExpGroundFog);
    getWindow ("CaelumLab/DepthComposer/GroundFogVerticalDecayEditbox")->setEnabled (enableExpGroundFog);
    getWindow ("CaelumLab/DepthComposer/GroundFogVerticalDecayScrollbar")->setEnabled (enableExpGroundFog);
    getWindow ("CaelumLab/DepthComposer/GroundFogBaseLevelLabel")->setEnabled (enableExpGroundFog);
    getWindow ("CaelumLab/DepthComposer/GroundFogBaseLevelEditbox")->setEnabled (enableExpGroundFog);
    getWindow ("CaelumLab/DepthComposer/GroundFogBaseLevelScrollbar")->setEnabled (enableExpGroundFog);

    if (mCaelumSystem->getDepthComposer ())
    {
        mCaelumSystem->getDepthComposer ()->setDebugDepthRender (debugDepthRender);
        mCaelumSystem->getDepthComposer ()->setSkyDomeHazeEnabled (enableSkyDomeHaze);
        mCaelumSystem->getDepthComposer ()->setGroundFogEnabled (enableExpGroundFog);
    }

    if (mCaelumSystem->getDepthComposer ())
    {
        Real tmp;

        // HACK: Control density through CaelumSystem.
        tmp = mCaelumSystem->getGroundFogDensityMultiplier ();
        updateScrollbarTweak(
                "CaelumLab/DepthComposer/GroundFogDensityEditbox",
                "CaelumLab/DepthComposer/GroundFogDensityScrollbar",
                tmp, 0, 1.5, 0, 0, 5);
        mCaelumSystem->setGroundFogDensityMultiplier (tmp);

        // Vertical decay
        tmp = mCaelumSystem->getDepthComposer ()->getGroundFogVerticalDecay ();
        updateScrollbarTweak(
                "CaelumLab/DepthComposer/GroundFogVerticalDecayEditbox",
                "CaelumLab/DepthComposer/GroundFogVerticalDecayScrollbar",
                tmp, 0, 5, 0, 0, 5);
        mCaelumSystem->getDepthComposer ()->setGroundFogVerticalDecay (tmp);

        // Base level
        tmp = mCaelumSystem->getDepthComposer ()->getGroundFogBaseLevel ();
        updateScrollbarTweak(
                "CaelumLab/DepthComposer/GroundFogBaseLevelEditbox",
                "CaelumLab/DepthComposer/GroundFogBaseLevelScrollbar",
                tmp, -50, 200);
        mCaelumSystem->getDepthComposer ()->setGroundFogBaseLevel (tmp);
    }
}

// Recursive implementation of setTerrainShadowsOn
void forceTerrainShadows(Ogre::SceneNode* node, bool enabled)
{
    // Look at child nodes.
    SceneNode::ChildNodeIterator childIterator = node->getChildIterator();
    while (childIterator.hasMoreElements()) {
        SceneNode* childNode = static_cast<SceneNode*>(childIterator.getNext());
        forceTerrainShadows(childNode, enabled);
    }

    // Look at attached objects.
    SceneNode::ObjectIterator objectIterator = node->getAttachedObjectIterator();
    while (objectIterator.hasMoreElements()) {
        MovableObject* object = objectIterator.getNext();
        //LogManager::getSingleton().logMessage("Found object of type " + object->getMovableType());
        if (object->getMovableType() == "TerrainMipMap") {
            object->setCastShadows(enabled);
        }
    }
}

void CaelumLabFrameListener::setTerrainShadowsOn(bool value) {
    if (mTerrainShadowsOn != value) {
        mTerrainShadowsOn = value;
        forceTerrainShadows (mSceneMgr->getRootSceneNode(), value);
    }
}

CaelumLabFrameListener::ScriptingUI::ScriptingUI (CaelumLabFrameListener* parent): mParent(parent)
{
}

CaelumLabFrameListener::ScriptingUI::~ScriptingUI ()
{
}

void CaelumLabFrameListener::ScriptingUI::initLoadableScriptList ()
{
    mLoadableScriptList.clear();
#if CAELUM_SCRIPT_SUPPORT
    PropScriptResourceManager* mgr = CaelumPlugin::getSingleton ().getPropScriptResourceManager ();
    for (Ogre::ResourceManager::ResourceMapIterator it = mgr->getResourceIterator(); it.hasMoreElements(); it.moveNext ())
    {
        Ogre::ResourcePtr resource = it.peekNextValue ();
        LoadableScript item;
        item.objectName = resource->getName ();
        mLoadableScriptList.push_back (item);
    }
#endif
}

static void sizeComboList(CEGUI::Combobox* cb)
{
    // get the size of the attached items
    float items_size = 0.0f;
    for (size_t i = 0; i < cb->getItemCount(); ++i) {
        items_size += cb->getListboxItemFromIndex(i)->getPixelSize().d_height;
    }

    // get size of editbox portion
    float edit_size = cb->getEditbox()->getPixelSize().d_height;

    // calculate how much size is taken by frame imagery & scrollbars
    CEGUI::ComboDropList* dl = cb->getDropList();
    CEGUI::String render_area_name(dl->getHorzScrollbar()->isVisible() ?
        "ItemRenderingAreaHScroll" : "ItemRenderingArea" );

    const CEGUI::WidgetLookFeel& wl =
        CEGUI::WidgetLookManager::getSingleton().getWidgetLook(dl->getLookNFeel());

    float images_size = dl->getPixelSize().d_height -
        wl.getNamedArea(render_area_name).getArea().getPixelRect(*dl).getHeight();

    // consider the possibility of an offset list position
    float droplist_offset =
        dl->getYPosition().asAbsolute(cb->getPixelSize().d_height) - edit_size;

    // set final size
    cb->setHeight( CEGUI::UDim( 0.0f,
                   items_size + edit_size + images_size + droplist_offset) );
}

void CaelumLabFrameListener::ScriptingUI::init ()
{
    CEGUI::Combobox* loadScriptCombo = mParent->getWidget<CEGUI::Combobox> ("CaelumLab/LoadScriptCombo");

#if CAELUM_SCRIPT_SUPPORT
    loadScriptCombo->resetList ();
    loadScriptCombo->setSortingEnabled (false);

    initLoadableScriptList ();
    for (LoadableScriptList::const_iterator it = mLoadableScriptList.begin(), end = mLoadableScriptList.end(); it != end; ++it) {
        int index = it - mLoadableScriptList.begin ();
        //String text = it->objectName; + " (file " + it->fileName + " group " + it->groupName + ")";
        String text = it->objectName;
        loadScriptCombo->addItem (new CEGUI::ListboxTextItem (CEGUI::String(text), index + 1));
    }
    sizeComboList (loadScriptCombo);

    clearSelection ();

    loadScriptCombo->subscribeEvent (
            CEGUI::Combobox::EventListSelectionAccepted,
            CEGUI::Event::Subscriber (&CaelumLabFrameListener::ScriptingUI::handleLoadScript, this));
#else
    loadScriptCombo->clearAllSelections ();
    loadScriptCombo->setText ("No script support");
    loadScriptCombo->setEnabled (false);
#endif
}

void CaelumLabFrameListener::ScriptingUI::clearSelection ()
{
    CEGUI::Combobox* loadScriptCombo = mParent->getWidget<CEGUI::Combobox> ("CaelumLab/LoadScriptCombo");

    loadScriptCombo->clearAllSelections ();
    loadScriptCombo->getEditbox ()->setText ("Pick item to load");
    //loadScriptCombo->setItemSelectState (static_cast<size_t> (0), true);
}

bool CaelumLabFrameListener::ScriptingUI::handleLoadScript (const CEGUI::EventArgs& args)
{
#if CAELUM_SCRIPT_SUPPORT
    CEGUI::Combobox* loadScriptCombo = mParent->getWidget<CEGUI::Combobox> ("CaelumLab/LoadScriptCombo");

    int scriptIndex = loadScriptCombo->getSelectedItem ()->getID () - 1;
    const LoadableScript& script = mLoadableScriptList[scriptIndex];

    CaelumPlugin::getSingleton ().loadCaelumSystemFromScript (
            mParent->mCaelumSystem.get(),
            script.objectName);

    clearSelection();

    // Refresh all widgets with data from CaelumSystem.
    mParent->initDepthComposerWidgets();
    mParent->updateWidgets();

    // Special setup for depth composer flags.
    if (mParent->mCaelumSystem->getDepthComposer ()) {
        mParent->setupDepthComposerFlags();
    }
#endif

    return true;
}

void CaelumLabFrameListener::initMaterialSchemeUI ()
{
    CEGUI::Combobox* materialSchemeCombo = getWidget<CEGUI::Combobox> ("CaelumLab/MaterialSchemeCombo");

    materialSchemeCombo->resetList ();
    materialSchemeCombo->setSortingEnabled (false);

    materialSchemeCombo->addItem (new CEGUI::ListboxTextItem (
                CEGUI::String("Default"), 0));
    materialSchemeCombo->addItem (new CEGUI::ListboxTextItem (
                CEGUI::String("PSSM"), 1));

    materialSchemeCombo->clearAllSelections ();
    materialSchemeCombo->getEditbox ()->setText ("Default");
    materialSchemeCombo->setItemSelectState (static_cast<size_t> (0), true);

    CEGUI::Checkbox* forceTerrainShadowsCheckbox = getWidget<CEGUI::Checkbox> ("CaelumLab/ForceTerrainShadowsCheckbox");
    forceTerrainShadowsCheckbox->setSelected (this->getTerrainShadowsOn());
}

void CaelumLabFrameListener::updateMaterialSchemeUI ()
{
    CEGUI::Combobox* materialSchemeCombo = getWidget<CEGUI::Combobox> ("CaelumLab/MaterialSchemeCombo");
    CEGUI::Checkbox* forceTerrainShadowsCheckbox = getWidget<CEGUI::Checkbox> ("CaelumLab/ForceTerrainShadowsCheckbox");

    // Check for a material scheme change
    SampleMaterialScheme scheme = mMaterialScheme;
    if (materialSchemeCombo->getSelectedItem ()) {
        scheme = static_cast<SampleMaterialScheme>(materialSchemeCombo->getSelectedItem ()->getID ());
    }
    if (scheme != mMaterialScheme) {
        mMaterialScheme = scheme;
        if (mMaterialScheme == SMS_PSSM) {
#if OGRE_VERSION >= 0x010600
            setupPSSM();
#else
            LogManager::getSingleton().logMessage("PSSM Requires Ogre 1.6");
#endif
        } else {
            mSceneMgr->setShadowTechnique(SHADOWTYPE_NONE);
            forceTerrainShadows(mSceneMgr->getRootSceneNode(), false);
            getViewport()->setMaterialScheme ("Default:");
        }
    }

    // Update terrain shadows on/off
    this->setTerrainShadowsOn(forceTerrainShadowsCheckbox->isSelected());
}

#if OGRE_VERSION >= 0x010600
void CaelumLabFrameListener::setupPSSM ()
{
    mSceneMgr->setShadowTechnique(SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED);

    // 3 textures per directional light
    mSceneMgr->setShadowTextureCountPerLightType(Ogre::Light::LT_DIRECTIONAL, 3);
    // We have two lights (sun and moon) so we request 6 textures.
    // Textures don't get reused here; 3 textures will not work.

    String deviceName = Root::getSingleton().getRenderSystem()->getCapabilities()->getDeviceName();
    String rsName = Root::getSingleton().getRenderSystem()->getName();
    if (rsName.find("OpenGL") != String::npos &&
        deviceName.find("GeForce 7") != String::npos)
    {
        LogManager::getSingleton().logMessage("Warning: Downgrading shadow texture quality for GeForce7 and OpenGL");
        mSceneMgr->setShadowTextureSettings(1024, 6, PF_FLOAT16_R);
    } else {
        mSceneMgr->setShadowTextureSettings(2048, 6, PF_FLOAT32_R);
    }
    mSceneMgr->setShadowTextureSelfShadow(true);
    mSceneMgr->setShadowTextureCasterMaterial("CaelumSample/PSSM/ShadowCaster");

    // Shadow camera setup
    PSSMShadowCameraSetup* pssmSetup = new PSSMShadowCameraSetup();

    mSceneMgr->setShadowCameraSetup(ShadowCameraSetupPtr(pssmSetup));

    updatePSSMCamera();

    getViewport()->setMaterialScheme ("PSSM");
}

void CaelumLabFrameListener::updatePSSMCamera ()
{
    PSSMShadowCameraSetup* pssmSetup = static_cast<PSSMShadowCameraSetup*>(mSceneMgr->getShadowCameraSetup().get());

    Real nearClip = getCamera()->getNearClipDistance();
    Real farClip = getCamera()->getFarClipDistance();
    // It seems that PSSM fails with infinite clip distances.
    if (farClip < nearClip) {
        farClip = 100 * nearClip;
    }
    pssmSetup->calculateSplitPoints(3, nearClip, farClip);
    pssmSetup->setSplitPadding(10);
    pssmSetup->setOptimalAdjustFactor(0, 2);
    pssmSetup->setOptimalAdjustFactor(1, 1);
    pssmSetup->setOptimalAdjustFactor(2, 0.5);

    // Send split point to actual materials.
    // This must be done manually.
    {
        Vector4 splitPoints;
        const PSSMShadowCameraSetup::SplitPointList& splitPointList = pssmSetup->getSplitPoints();
        for (int i = 0; i < 3; ++i)
        {
            splitPoints[i] = splitPointList[i];
        }

        const char *pssmMaterials[]  = {
            "Examples/TudorHouse",
            "CaelumSample/Terrain",
            "CaelumSample/AlphaRejectionTest",
            "CaelumSample/AlphaBlendTest",
            "3D-Diggers/fir01",
            "3D-Diggers/fir02",
            "SOLID/TEX/wood7.jpg",
            "SOLID/TEX/Leaves.png",
            "SOLID/TEX/PineLeaves.png",
            0
        };

        for (const char** matIter = pssmMaterials; *matIter; ++matIter) {
            MaterialPtr mat = MaterialManager::getSingleton().getByName(*matIter);
            if (mat.isNull()) {
                LogManager::getSingleton().logMessage(String("Couldn't find pssm material") + *matIter);
                continue;
            }
            GpuProgramParametersSharedPtr fpParams = mat->getTechnique("PSSM")->getPass("Directional")->getFragmentProgramParameters();
            fpParams->setIgnoreMissingParams (true);
            fpParams->setNamedConstant ("pssmSplitPoints", splitPoints);
        }
    }
}
#endif

bool CaelumLabFrameListener::frameStarted (const FrameEvent& evt)
{
    if (mShutdownRequested || mWindow->isClosed()) {
        return false;
    }
    mGuiSystem->injectTimePulse (evt.timeSinceLastFrame);

    mKeyboard->capture ();
    mMouse->capture ();

    if (!updateInput (evt)) {
        return false;
    }

    mCaelumSystem->notifyCameraChanged (mCamera);
    mCaelumSystem->updateSubcomponents (evt.timeSinceLastFrame);
    updateWidgets ();

    return true;
}

void CaelumLabApplication::createFrameListener () {
    mFrameListener = new CaelumLabFrameListener (mWindow, mCamera);
    mRoot->addFrameListener (mFrameListener);
}

void CaelumLabApplication::chooseSceneManager () {
    mSceneMgr = mRoot->createSceneManager ("TerrainSceneManager");
}

void CaelumLabApplication::createCamera () {
    // Create the camera
    mCamera = mSceneMgr->createCamera ("PlayerCam");

    mCamera->setPosition (Vector3 (775, 100, 997));
    mCamera->lookAt (Vector3 (775, 100, 1000));

    // Start with very generous settings (precision-wise)
    mCamera->setNearClipDistance (10);
    mCamera->setFarClipDistance (50000);
	mCamera->setFOVy(Ogre::Degree(45));
}

void CaelumLabApplication::installCaelumPlugin () {
    if (CaelumPlugin::getSingletonPtr ()) {
        LogManager::getSingleton ().logMessage(
                "CaelumLabApplication: Caelum plugin seems to be already installed");
    } else {
        Root::getSingleton().installPlugin (new CaelumPlugin ());
    }
}

void CaelumLabApplication::setupResources () {
    installCaelumPlugin ();
    ExampleApplication::setupResources ();
}

Real CaelumLabApplication::getHeightAtPoint (Real x, Real z)
{
    RaySceneQuery* rayQuery = mSceneMgr->createRayQuery(Ray(Vector3::ZERO, Vector3::ZERO));
    rayQuery->setRay(Ray(Vector3(x, 0, z), Vector3::UNIT_Y));

    Real result = -1;
    RaySceneQueryResult queryResult = rayQuery->execute();
    for (Ogre::uint i = 0; i < queryResult.size(); ++i) {
        if (queryResult[i].worldFragment) {
            result = queryResult[i].distance;
        }
    }

    mSceneMgr->destroyQuery (rayQuery);
    return result;
}

struct RandomModelInfo
{
    RandomModelInfo(String mesh, String mat, Vector3 trans, Quaternion rot, Vector3 scale):
            mesh(mesh), mat(mat), trans(trans), rot(rot), scale(scale)
    {
    }
    Ogre::String mesh;
    Ogre::String mat;
    Ogre::Vector3 trans;
    Ogre::Quaternion rot;
    Ogre::Vector3 scale;
};

void CaelumLabApplication::createScene () {
    std::string terrain_cfg("CaelumDemoTerrain.cfg");
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
    terrain_cfg = mResourcePath + terrain_cfg;
#endif
    mSceneMgr->setWorldGeometry (terrain_cfg);

    typedef std::vector<RandomModelInfo> RandomModelVector;
    RandomModelVector randomModels;

    // Tudor house; from ogre resources.
    randomModels.push_back(RandomModelInfo("TudorHouse.mesh", "",
                    Vector3(0, 4.5, 0), Quaternion::IDENTITY, Vector3::UNIT_SCALE * 0.01));

    // Create a random plane mesh.
    Ogre::String planeMeshName = "CaelumSample/FlatPlane";
    Ogre::MeshManager::getSingleton().createPlane(
            planeMeshName, Caelum::RESOURCE_GROUP_NAME,
            Ogre::Plane(Ogre::Vector3::UNIT_X, 0),
            20, 20, 4, 4, true, 1, 2, 2,
            Ogre::Vector3::UNIT_Z);
    randomModels.push_back(RandomModelInfo("CaelumSample/FlatPlane", "CaelumSample/AlphaRejectionTest",
                    Vector3(0, 10, 0),
                    Quaternion::IDENTITY,
                    Vector3::UNIT_SCALE));
    randomModels.push_back(RandomModelInfo("CaelumSample/FlatPlane", "CaelumSample/AlphaBlendTest",
                    Vector3(0, 10, 0),
                    Quaternion::IDENTITY,
                    Vector3::UNIT_SCALE));

    // Models from pages geometry
    randomModels.push_back(RandomModelInfo("tree.mesh", "",
                    Vector3::ZERO, Quaternion::IDENTITY, Vector3::UNIT_SCALE * 0.5));
    randomModels.push_back(RandomModelInfo("tree2.mesh", "",
                    Vector3::ZERO, Quaternion::IDENTITY, Vector3::UNIT_SCALE * 0.5));
    randomModels.push_back(RandomModelInfo("Pine1.mesh", "",
                    Vector3::ZERO, Quaternion::IDENTITY, Vector3::UNIT_SCALE * 0.5));
    randomModels.push_back(RandomModelInfo("fir05_30.mesh", "",
                    Vector3::ZERO, Quaternion::IDENTITY, Vector3::UNIT_SCALE * 0.05));
    randomModels.push_back(RandomModelInfo("fir06_30.mesh", "",
                    Vector3::ZERO, Quaternion::IDENTITY, Vector3::UNIT_SCALE * 0.05));
    randomModels.push_back(RandomModelInfo("fir14_25.mesh", "",
                    Vector3::ZERO, Quaternion::IDENTITY, Vector3::UNIT_SCALE * 0.05));

    // Create 100 random models.
    for (int i = 0; i < 100; ++i)
    {
        // Pick model.
        const RandomModelInfo& model = randomModels[rand() % randomModels.size()];

        Entity *entity = mSceneMgr->createEntity (
                "CaelumSample/RandomScene/Entity" + StringConverter::toString(i), model.mesh);
        SceneNode *sceneNode = mSceneMgr->getRootSceneNode ()->createChildSceneNode(
                "CaelumSample/RandomScene/Node" + StringConverter::toString (i));

        if (!model.mat.empty ()) {
            entity->setMaterialName (model.mat);
        }
 
        // Place randomly on the terrain around the camera.
        Vector3 randPos(300 * Math::SymmetricRandom (), 0, 300 * Math::SymmetricRandom ());
        Vector3 pos = randPos + mCamera->getPosition ();
        pos.y = getHeightAtPoint (pos.x, pos.z);

        // Add a random rotation around the vertical.
        Quaternion randRot(Ogre::Degree(0.01 * (rand() % 36000)), Vector3::UNIT_Y);
        //Quaternion randRot = Quaternion::IDENTITY;

        // Enforce placement
        sceneNode->attachObject (entity);
        sceneNode->translate (model.trans + pos);
        sceneNode->rotate (randRot * model.rot);
        sceneNode->scale (model.scale);
    }
}

extern "C" int main(int argc, char **argv)
{
    try {
        chdirExePath(argv[0]);

        CaelumLabApplication app;
        app.go ();

        // Normal shutdown.
        return 0;
    } catch (Ogre::Exception& e) {
        reportException (e.getFullDescription ().c_str ());
    } catch (CEGUI::Exception& e) {
        reportException (e.getMessage ().c_str ());
    } catch (std::exception& e) {
        reportException (e.what ());
    }

    // Error.
    return 1;
}

