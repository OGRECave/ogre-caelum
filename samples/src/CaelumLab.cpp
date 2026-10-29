// This file is part of the Caelum project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution.

#include "CaelumLab.h"

#include <OgreOverlayManager.h>
#include <OgreOverlaySystem.h>
#include <OgreShaderGenerator.h>
#include <OgreShaderSubRenderState.h>
#include <OgreTerrainMaterialGeneratorA.h>

bool CaelumLabInputListener::keyPressed (const OgreBites::KeyboardEvent& evt)
{
    if (evt.keysym.sym == OgreBites::SDLK_ESCAPE) {
        mApp->getRoot ()->queueEndRendering ();
        return true;
    }
    return mCameraMan->keyPressed (evt);
}

bool CaelumLabInputListener::mouseMoved (const OgreBites::MouseMotionEvent& evt)
{
    return mMouseLook && mCameraMan->mouseMoved (evt);
}

bool CaelumLabInputListener::mousePressed (const OgreBites::MouseButtonEvent& evt)
{
    if (evt.button == OgreBites::BUTTON_RIGHT) {
        mMouseLook = true;
        mApp->setWindowGrab (true);
    }
    return true;
}

bool CaelumLabInputListener::mouseReleased (const OgreBites::MouseButtonEvent& evt)
{
    if (evt.button == OgreBites::BUTTON_RIGHT) {
        mMouseLook = false;
        mApp->setWindowGrab (false);
    }
    return true;
}

CaelumLabApplication::CaelumLabApplication ():
        mFollowSun (false),
        mFollowMoon (false),
        mShadowsEnabled (false)
{
}

static void logUnsupportedException (Caelum::UnsupportedException& ex) {
    Ogre::LogManager::getSingletonPtr()->logMessage("CaelumLab: UnsupportedException: " + ex.getFullDescription ());
}

void CaelumLabApplication::initCaelum ()
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
    mCaelumSystem->attachViewport (getViewport ());
    getRenderWindow ()->addListener (mCaelumSystem.get ());
    mRoot->addFrameListener (mCaelumSystem.get ());

    try {
        mCaelumSystem->setPrecipitationController (new PrecipitationController (mSceneMgr));
    } catch (Caelum::UnsupportedException& ex) {
        logUnsupportedException (ex);
    }

    mCaelumSystem->setSceneFogDensityMultiplier (0.0015);
    mCaelumSystem->setManageAmbientLight (true);
    mCaelumSystem->setMinimumAmbientLight (Ogre::ColourValue (0.1, 0.1, 0.1));

    // Setup sun options
    if (mCaelumSystem->getSun ()) {
        mCaelumSystem->getSun ()->setAutoDisableThreshold (0.05);
        mCaelumSystem->getSun ()->setAutoDisable (false);
    }

    if (mCaelumSystem->getMoon ()) {
        mCaelumSystem->getMoon ()->setAutoDisableThreshold (0.05);
        mCaelumSystem->getMoon ()->setAutoDisable (false);
        // Only the sun casts shadows.
        mCaelumSystem->getMoon ()->getMainLight ()->setCastShadows (false);
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

    // Sunrise with visible moon.
    mCaelumSystem->getUniversalClock ()->setGregorianDateTime (2007, 4, 9, 9, 33, 0);
}

void CaelumLabApplication::destroyCaelum ()
{
    if (!mCaelumSystem) {
        return;
    }
    mRoot->removeFrameListener (mCaelumSystem.get ());
    getRenderWindow ()->removeListener (mCaelumSystem.get ());
    mCaelumSystem.reset();
}

void CaelumLabApplication::initDepthComposer ()
{
    mCaelumSystem->setDepthComposer (new DepthComposer (mSceneMgr));

    mCaelumSystem->setGroundFogDensityMultiplier (0.03);
    mCaelumSystem->getDepthComposer ()->setGroundFogVerticalDecay (0.06);
    mCaelumSystem->getDepthComposer ()->setGroundFogBaseLevel (0);
}

void CaelumLabApplication::setShadowsEnabled (bool enabled)
{
    mShadowsEnabled = enabled;

    auto matProfile = static_cast<TerrainMaterialGeneratorA::SM2Profile*>(
            mTerrainGlobals->getDefaultMaterialGenerator ()->getActiveProfile ());
    matProfile->setReceiveDynamicShadowsEnabled (enabled);

    RTShader::RenderState* renderState = mShaderGenerator->getRenderState (MSN_SHADERGEN);
    if (auto srs = renderState->getSubRenderState (RTShader::SRS_SHADOW_MAPPING)) {
        renderState->removeSubRenderState (srs);
    }

    if (enabled) {
        // Use the integrated PSSM of the RTSS for the sun light.
        mSceneMgr->setShadowTechnique (SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED);
        mSceneMgr->setShadowFarDistance (3000);
        mSceneMgr->setShadowTextureCountPerLightType (Ogre::Light::LT_DIRECTIONAL, 3);
        mSceneMgr->setShadowTextureCount (3);
        mSceneMgr->setShadowTextureConfig (0, 2048, 2048, PF_DEPTH16);
        mSceneMgr->setShadowTextureConfig (1, 1024, 1024, PF_DEPTH16);
        mSceneMgr->setShadowTextureConfig (2, 1024, 1024, PF_DEPTH16);
        mSceneMgr->setShadowTextureSelfShadow (true);
        mSceneMgr->setShadowCasterRenderBackFaces (true);

        if (!mPSSMSetup) {
            mPSSMSetup = PSSMShadowCameraSetup::create ();
        }
        mSceneMgr->setShadowCameraSetup (mPSSMSetup);
        updatePSSMSplitPoints ();

        auto pssm = static_cast<PSSMShadowCameraSetup*> (mPSSMSetup.get ());
        auto subRenderState = mShaderGenerator->createSubRenderState (RTShader::SRS_SHADOW_MAPPING);
        subRenderState->setParameter ("split_points", pssm->getSplitPoints ());
        renderState->addTemplateSubRenderState (subRenderState);
        matProfile->setReceiveDynamicShadowsPSSM (pssm);
    } else {
        mSceneMgr->setShadowTechnique (SHADOWTYPE_NONE);
    }

    mShaderGenerator->invalidateScheme (MSN_SHADERGEN);
}

void CaelumLabApplication::updatePSSMSplitPoints ()
{
    auto pssm = static_cast<PSSMShadowCameraSetup*> (mPSSMSetup.get ());
    Real nearClip = mCamera->getNearClipDistance ();
    pssm->setSplitPadding (nearClip * 2);
    pssm->calculateSplitPoints (3, nearClip, mSceneMgr->getShadowFarDistance ());
    pssm->setOptimalAdjustFactor (0, 2);
    pssm->setOptimalAdjustFactor (1, 1);
    pssm->setOptimalAdjustFactor (2, 0.5);

    if (auto srs = mShaderGenerator->getRenderState (MSN_SHADERGEN)->getSubRenderState (RTShader::SRS_SHADOW_MAPPING)) {
        srs->setParameter ("split_points", pssm->getSplitPoints ());
    }
}

static String formatTime (int year, int month, int day, int hour, int minute, int second) {
    return StringUtil::format ("%04d-%02d-%02dT%02d:%02d:%02d", year, month, day, hour, minute, second);
}

static String formatTimeFromJDay (LongReal jday) {
    int year, month, day, hour, minute;
    LongReal second;
    Astronomy::getGregorianDateTimeFromJulianDay(jday, year, month, day, hour, minute, second);
    return formatTime(year, month, day, hour, minute, static_cast<int>(second));
}

static String formatDegMinSec (Ogre::Degree angle) {
    Real aa = Math::Abs(angle.valueDegrees());
    int sec = (int)(aa * 3600);
    return StringUtil::format ("%s%02dd%02d'%02d\"",
            (angle.valueDegrees() < 0) ? "-" : "", sec / 3600, sec / 60 % 60, sec % 60);
}

static String formatHorizDirection (Ogre::Degree azm, Ogre::Degree alt) {
    Real az = fmod(fmod(azm.valueDegrees(), 360) + 360, 360);
    Real al = fmod(fmod(alt.valueDegrees() + 180, 360) - 180, 360);
    return "azm " + formatDegMinSec(Degree(az)) + " alt " + formatDegMinSec(Degree(al));
}

static String formatHorizDirection (Vector3 dir) {
    LongReal theta, phi, r;
    Astronomy::convertRectangularToSpherical(
                -dir.z, dir.x, -dir.y,
                theta, phi, r);
    return formatHorizDirection(Degree(theta), Degree(phi));
}

static void labelText (const char* label, const String& text) {
    ImGui::LabelText (label, "%s", text.c_str ());
}

/// Logarithmic slider for a Real; returns if the value was changed.
static bool sliderReal (const char* label, Real& value, Real minValue, Real maxValue, bool logarithmic = false) {
    float tmp = value;
    bool changed = ImGui::SliderFloat (label, &tmp, minValue, maxValue, logarithmic ? "%.5g" : "%.3f",
            logarithmic ? ImGuiSliderFlags_Logarithmic : ImGuiSliderFlags_None);
    if (changed) {
        value = tmp;
    }
    return changed;
}

void CaelumLabApplication::drawTimeUI ()
{
    UniversalClock* clock = mCaelumSystem->getUniversalClock ();

    // The precision of float is not enough here.
    double jday = clock->getJulianDay ();
    if (ImGui::InputDouble ("Julian day", &jday, 0, 0, "%.6f", ImGuiInputTextFlags_EnterReturnsTrue)) {
        clock->setJulianDay (Math::Clamp (jday, -10000000.0, 10000000.0));
    }
    labelText ("UTC time", formatTimeFromJDay (jday));

    Real timeScale = clock->getTimeScale ();
    if (sliderReal ("Time scale", timeScale, -10000, 10000, true)) {
        clock->setTimeScale (timeScale);
    }
    if (ImGui::Button ("Pause")) {
        clock->setTimeScale (0);
    }
    ImGui::SameLine ();
    if (ImGui::Button ("Real time")) {
        clock->setTimeScale (1);
    }
}

void CaelumLabApplication::drawCameraUI ()
{
    labelText ("Position", StringConverter::toString (mCamera->getDerivedPosition ()));
    labelText ("Direction", formatHorizDirection (-mCamera->getDerivedDirection ()));

    Real speed = mCameraMan->getTopSpeed ();
    if (sliderReal ("Speed", speed, 0.05, 500, true)) {
        mCameraMan->setTopSpeed (speed);
    }

    Real fov = mCamera->getFOVy ().valueDegrees ();
    if (sliderReal ("FOV", fov, 1, 120)) {
        mCamera->setFOVy (Ogre::Degree (fov));
    }

    Real nearClip = mCamera->getNearClipDistance ();
    if (sliderReal ("Near clip", nearClip, 0.00001, 500, true)) {
        mCamera->setNearClipDistance (nearClip);
        // See the effect of clip distances on PSSM in real-time.
        if (mShadowsEnabled) {
            updatePSSMSplitPoints ();
        }
    }

    Real farClip = mCamera->getFarClipDistance ();
    if (sliderReal ("Far clip (0 = inf)", farClip, 0, 50000, true)) {
        mCamera->setFarClipDistance (farClip);
    }
}

void CaelumLabApplication::drawSunMoonUI ()
{
    LongReal jday = mCaelumSystem->getUniversalClock ()->getJulianDay ();

    if (BaseSkyLight* sun = mCaelumSystem->getSun ()) {
        labelText ("Sun direction", formatHorizDirection (mCaelumSystem->getSunDirection (jday)));
        ImGui::Checkbox ("Follow sun", &mFollowSun);
        if (mFollowSun) {
            mCameraNode->setDirection (-sun->getLightDirection (), Node::TS_WORLD);
        }
    }

    if (Moon* moon = mCaelumSystem->getMoon ()) {
        Ogre::Vector3 moondir = mCaelumSystem->getMoonDirection (jday);
        labelText ("Moon direction", formatHorizDirection (moondir));
        labelText ("Moon phase", StringConverter::toString (mCaelumSystem->getMoonPhase (jday)));
        labelText ("Moon light", StringConverter::toString (mCaelumSystem->getMoonLightColour (moondir)));
        ImGui::Checkbox ("Follow moon", &mFollowMoon);
        if (mFollowMoon) {
            mCameraNode->setDirection (-moon->getLightDirection (), Node::TS_WORLD);
        }
    }
}

void CaelumLabApplication::drawCloudsUI ()
{
    CloudSystem* clouds = mCaelumSystem->getCloudSystem ();
    if (!clouds) {
        ImGui::TextDisabled ("No cloud system");
        return;
    }

    for (int i = 0; i < clouds->getLayerCount (); ++i) {
        ImGui::PushID (i);
        FlatCloudLayer* layer = clouds->getLayer (i);
        ImGui::Text ("Layer %d", i + 1);

        Real cover = layer->getCloudCover ();
        if (sliderReal ("Cover", cover, 0, 1)) {
            layer->setCloudCover (cover);
        }
        Real height = layer->getHeight ();
        if (sliderReal ("Height", height, 100, 15000)) {
            layer->setHeight (height);
        }
        ImGui::PopID ();
    }
}

void CaelumLabApplication::drawStarfieldUI ()
{
    PointStarfield* stars = mCaelumSystem->getPointStarfield ();
    if (!stars) {
        ImGui::TextDisabled ("No point starfield");
        return;
    }

    Real magScale = stars->getMagnitudeScale ();
    if (sliderReal ("Magnitude scale", magScale, 1, 5)) {
        stars->setMagnitudeScale (magScale);
    }
    Real mag0size = stars->getMag0PixelSize ();
    if (sliderReal ("Mag0 pixel size", mag0size, 0, 50)) {
        stars->setMag0PixelSize (mag0size);
    }
    Real minSize = stars->getMinPixelSize ();
    if (sliderReal ("Min pixel size", minSize, 0, 50)) {
        stars->setMinPixelSize (minSize);
    }
    Real maxSize = stars->getMaxPixelSize ();
    if (sliderReal ("Max pixel size", maxSize, 0, 50)) {
        stars->setMaxPixelSize (maxSize);
    }
}

void CaelumLabApplication::drawPrecipitationUI ()
{
    PrecipitationController* prec = mCaelumSystem->getPrecipitationController ();
    if (!prec) {
        ImGui::TextDisabled ("No precipitation controller");
        return;
    }

    static const char* typeNames[] = {
        "Drizzle", "Rain", "Snow", "Snow grains", "Ice crystals", "Ice pellets", "Hail", "Small hail", "Custom"
    };
    int type = prec->getPresetType ();
    if (ImGui::Combo ("Type", &type, typeNames, IM_ARRAYSIZE(typeNames))) {
        Caelum::PrecipitationType typeEnum = static_cast<Caelum::PrecipitationType> (type);
        if (PrecipitationController::isPresetType (typeEnum)) {
            prec->setPresetType (typeEnum);
        }
    }

    Real speed = prec->getSpeed ();
    if (sliderReal ("Speed", speed, 0, 1)) {
        prec->setSpeed (speed);
    }
    Real intensity = prec->getIntensity ();
    if (sliderReal ("Intensity", intensity, 0, 1)) {
        prec->setIntensity (intensity);
    }
}

void CaelumLabApplication::drawDepthComposerUI ()
{
    bool enabled = mCaelumSystem->getDepthComposer () != 0;
    if (ImGui::Checkbox ("Enabled", &enabled)) {
        mDeferredActions.push_back ([this, enabled] () {
            if (enabled) {
                initDepthComposer ();
            } else {
                mCaelumSystem->setDepthComposer (0);
            }
        });
    }

    DepthComposer* composer = mCaelumSystem->getDepthComposer ();
    ImGui::BeginDisabled (!composer);

    bool debugDepthRender = composer && composer->getDebugDepthRender ();
    if (ImGui::Checkbox ("Debug depth render", &debugDepthRender)) {
        composer->setDebugDepthRender (debugDepthRender);
    }

    ImGui::BeginDisabled (debugDepthRender);
    bool skyDomeHaze = composer && composer->getSkyDomeHazeEnabled ();
    if (ImGui::Checkbox ("Sky dome haze", &skyDomeHaze)) {
        composer->setSkyDomeHazeEnabled (skyDomeHaze);
    }
    bool groundFog = composer && composer->getGroundFogEnabled ();
    if (ImGui::Checkbox ("Ground fog", &groundFog)) {
        composer->setGroundFogEnabled (groundFog);
    }

    ImGui::BeginDisabled (!groundFog);
    if (composer) {
        // Density is controlled through CaelumSystem.
        Real density = mCaelumSystem->getGroundFogDensityMultiplier ();
        if (sliderReal ("Fog density", density, 0, 1.5, true)) {
            mCaelumSystem->setGroundFogDensityMultiplier (density);
        }
        Real decay = composer->getGroundFogVerticalDecay ();
        if (sliderReal ("Fog vertical decay", decay, 0, 5, true)) {
            composer->setGroundFogVerticalDecay (decay);
        }
        Real baseLevel = composer->getGroundFogBaseLevel ();
        if (sliderReal ("Fog base level", baseLevel, -50, 200)) {
            composer->setGroundFogBaseLevel (baseLevel);
        }
    }
    ImGui::EndDisabled ();
    ImGui::EndDisabled ();
    ImGui::EndDisabled ();
}

void CaelumLabApplication::drawShadowsUI ()
{
    bool shadows = mShadowsEnabled;
    if (ImGui::Checkbox ("PSSM shadows", &shadows)) {
        setShadowsEnabled (shadows);
    }
}

void CaelumLabApplication::drawScriptsUI ()
{
#if CAELUM_SCRIPT_SUPPORT
    PropScriptResourceManager* mgr = CaelumPlugin::getSingleton ().getPropScriptResourceManager ();
    if (ImGui::BeginCombo ("Load script", "Pick item to load")) {
        String selected;
        for (const auto& it : mgr->getResourceIterator ()) {
            if (ImGui::Selectable (it.second->getName ().c_str ())) {
                selected = it.second->getName ();
            }
        }
        ImGui::EndCombo ();

        if (!selected.empty ()) {
            mDeferredActions.push_back ([this, selected] () {
                CaelumPlugin::getSingleton ().loadCaelumSystemFromScript (mCaelumSystem.get (), selected);
            });
        }
    }
#else
    ImGui::TextDisabled ("No script support");
#endif
}

void CaelumLabApplication::dumpRenderTargets ()
{
    std::vector<Ogre::RenderTarget*> targets = {getRenderWindow ()};
    for (const auto& it : Ogre::TextureManager::getSingleton ().getResourceIterator ()) {
        auto tex = static_pointer_cast<Ogre::Texture> (it.second);
        if (tex->isLoaded () && (tex->getUsage () & TU_RENDERTARGET)) {
            targets.push_back (tex->getBuffer ()->getRenderTarget ());
        }
    }

    for (Ogre::RenderTarget* rt : targets) {
        Ogre::String name = rt->getName ();
        std::replace (name.begin (), name.end (), '/', '_');
        Ogre::String fileName = rt->writeContentsToTimestampedFile ("RTDump ", " " + name + ".png");
        Ogre::LogManager::getSingletonPtr ()->logMessage ("Written render target " + rt->getName () + " to " + fileName);
    }
}

void CaelumLabApplication::dumpMaterialNames ()
{
    for (const auto& it : Ogre::MaterialManager::getSingleton ().getResourceIterator ()) {
        Ogre::LogManager::getSingletonPtr ()->logMessage ("Material name " + it.second->getName ());
    }
}

void CaelumLabApplication::drawDebugUI ()
{
    if (ImGui::Button ("Dump material names")) {
        dumpMaterialNames ();
    }
    ImGui::SameLine ();
    if (ImGui::Button ("Dump render targets")) {
        dumpRenderTargets ();
    }
}

void CaelumLabApplication::drawUI ()
{
    float vpScale = OverlayManager::getSingleton ().getPixelRatio ();
    ImGui::SetNextWindowPos (ImVec2 (10 * vpScale, 10 * vpScale), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize (ImVec2 (380 * vpScale, getViewport ()->getActualHeight () - 20 * vpScale), ImGuiCond_FirstUseEver);
    ImGui::Begin ("CaelumLab");

    ImGui::TextDisabled ("Right mouse button: look around, WASD: move");

    if (ImGui::CollapsingHeader ("Time", ImGuiTreeNodeFlags_DefaultOpen)) {
        drawTimeUI ();
    }
    if (ImGui::CollapsingHeader ("Camera")) {
        drawCameraUI ();
    }
    if (ImGui::CollapsingHeader ("Sun & Moon")) {
        drawSunMoonUI ();
    }
    if (ImGui::CollapsingHeader ("Clouds")) {
        drawCloudsUI ();
    }
    if (ImGui::CollapsingHeader ("Starfield")) {
        drawStarfieldUI ();
    }
    if (ImGui::CollapsingHeader ("Precipitation")) {
        drawPrecipitationUI ();
    }
    if (ImGui::CollapsingHeader ("Depth composer")) {
        drawDepthComposerUI ();
    }
    if (ImGui::CollapsingHeader ("Shadows")) {
        drawShadowsUI ();
    }
    if (ImGui::CollapsingHeader ("Scripts")) {
        drawScriptsUI ();
    }
    if (ImGui::CollapsingHeader ("Debug")) {
        drawDebugUI ();
    }

    ImGui::Separator ();
    if (ImGui::Button ("Reset")) {
        mDeferredActions.push_back ([this] () {
            destroyCaelum ();
            initCaelum ();
        });
    }
    ImGui::SameLine ();
    if (ImGui::Button ("Quit")) {
        mRoot->queueEndRendering ();
    }

    ImGui::End ();
}

bool CaelumLabApplication::frameEnded (const Ogre::FrameEvent& evt)
{
    for (auto& action : mDeferredActions) {
        action ();
    }
    mDeferredActions.clear ();
    return true;
}

void CaelumLabApplication::preViewportUpdate (const Ogre::RenderTargetViewportEvent& evt)
{
    if (!evt.source->getOverlaysEnabled ()) {
        return;
    }
    ImGuiOverlay::NewFrame ();
    drawUI ();
}

void CaelumLabApplication::loadResources ()
{
    new CaelumPlugin ();
    CaelumPlugin::getSingleton ().initialise ();
    ExampleApplication::loadResources ();
}

void CaelumLabApplication::createCamera ()
{
    // Create the camera
    mCamera = mSceneMgr->createCamera ("PlayerCam");

    mCameraNode = mSceneMgr->getRootSceneNode ()->createChildSceneNode ();
    mCameraNode->setPosition (Vector3 (775, 100, 997));
    mCameraNode->setFixedYawAxis (true);
    mCameraNode->lookAt (Vector3 (775, 100, 1000), Node::TS_PARENT);
    mCameraNode->attachObject (mCamera);

    // The DepthComposer uses a 16 bit depth buffer; keep the near clip distance large enough
    mCamera->setNearClipDistance (10);
    mCamera->setFarClipDistance (10000);
    mCamera->setFOVy (Ogre::Degree (45));
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

void CaelumLabApplication::createScene ()
{
    mTerrainGlobals.reset (new TerrainGlobalOptions ());
    mTerrainGlobals->setCastsDynamicShadows (true);
    mTerrainGroup.reset (new TerrainGroup (mSceneMgr));
    mTerrainGroup->loadLegacyTerrain ("CaelumDemoTerrain.cfg");

    typedef std::vector<RandomModelInfo> RandomModelVector;
    RandomModelVector randomModels;

    // Tudor house; from ogre resources.
    randomModels.push_back(RandomModelInfo("TudorHouse.mesh", "",
                    Vector3(0, 4.5, 0), Quaternion::IDENTITY, Vector3::UNIT_SCALE * 0.01));

    // Create a random plane mesh.
    Ogre::String planeMeshName = "CaelumSample/FlatPlane";
    Ogre::MeshManager::getSingleton().createPlane(
            planeMeshName, RGN_DEFAULT,
            Ogre::Plane(Ogre::Vector3::UNIT_X, 0),
            20, 20, 4, 4, true, 1, 2, 2,
            Ogre::Vector3::UNIT_Z);
    randomModels.push_back(RandomModelInfo(planeMeshName, "CaelumSample/AlphaRejectionTest",
                    Vector3(0, 10, 0),
                    Quaternion::IDENTITY,
                    Vector3::UNIT_SCALE));
    randomModels.push_back(RandomModelInfo(planeMeshName, "CaelumSample/AlphaBlendTest",
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

        Entity *entity = mSceneMgr->createEntity (model.mesh);
        SceneNode *sceneNode = mSceneMgr->getRootSceneNode ()->createChildSceneNode ();

        if (!model.mat.empty ()) {
            entity->setMaterialName (model.mat);
        }

        // Place randomly on the terrain around the camera.
        Vector3 randPos(300 * Math::SymmetricRandom (), 0, 300 * Math::SymmetricRandom ());
        Vector3 pos = randPos + mCameraNode->getPosition ();
        pos.y = mTerrainGroup->getHeightAtWorldPosition (pos);

        // Add a random rotation around the vertical.
        Quaternion randRot(Ogre::Degree(0.01 * (rand() % 36000)), Vector3::UNIT_Y);

        // Enforce placement
        sceneNode->attachObject (entity);
        sceneNode->translate (model.trans + pos);
        sceneNode->rotate (randRot * model.rot);
        sceneNode->scale (model.scale);
    }

    initCaelum ();
}

void CaelumLabApplication::createFrameListener ()
{
    // The overlay system renders ImGui.
    mSceneMgr->addRenderQueueListener (getOverlaySystem ());
    // DPI scaling; initialiseImGui scales the style by the pixel ratio
    float vpScale = getDisplayDPI () / 96;
    OverlayManager::getSingleton ().setPixelRatio (vpScale);
    ImGuiOverlay* imguiOverlay = initialiseImGui ();
    ImGui::GetStyle ().FontScaleMain = vpScale;
    imguiOverlay->show ();
    // Ogre up to 14.6 applies the scene fog, which Caelum manages, to the UI
    MaterialManager::getSingleton ().getByName ("ImGui/material", RGN_INTERNAL)->getTechnique (0)->getPass (0)->setFog (true);
    getRenderWindow ()->addListener (this);

    mCameraMan.reset (new OgreBites::CameraMan (mCameraNode));
    mCameraMan->setStyle (OgreBites::CS_FREELOOK);
    mCameraMan->setTopSpeed (50);
    mLabInputListener.reset (new CaelumLabInputListener (this, mCameraMan.get ()));

    // ImGui gets the events first; deleted by ExampleApplication::shutdown.
    mFrameListener = new OgreBites::InputListenerChain ({getImGuiInputListener (), mLabInputListener.get ()});
    addInputListener (mFrameListener);
}

void CaelumLabApplication::destroyScene ()
{
    getRenderWindow ()->removeListener (this);
    destroyCaelum ();
    mTerrainGroup.reset ();
    mTerrainGlobals.reset ();
}

int main (int argc, char **argv)
{
    try {
        chdirExePath (argv[0]);

        CaelumLabApplication app;
        app.go ();
        return 0;

    } catch (Exception& e) {
        reportException (e.getFullDescription ().c_str ());
    } catch (std::exception& e) {
        reportException (e.what ());
    }

    return 1;
}
