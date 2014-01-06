#include "ik_ESoundEngineOptions.h"
#include "ik_ESoundOutputDrivers.h"
#include "irrKlang.h"
#include "Actors/Components/SoundComponent.h"
#include "Core/Cameras/CameraManager.h"
#include "Core/Options.h"
#include "Core/Pauser.h"
#include "Kernel/LKernel.h"
#include "Levels/LevelManager.h"
#include "Players/PlayerManager.h"
#include "Sound/SoundMain.h"

using namespace std;
using namespace irrklang;
using namespace Ogre;
using namespace Ponykart::Actors;
using namespace Ponykart::Core;
using namespace Ponykart::Levels;
using namespace Ponykart::LKernel;
using namespace Ponykart::Players;
using namespace Ponykart::Sound;

SoundMain::SoundMain()
{
	log("[Loading] Creating IrrKlang and SoundMain...");

	enableMusic = Options::getBool("Music");
	enableSounds = Options::getBool("Sounds");

	playerManager = LKernel::getG<PlayerManager>();
	cameraManager = LKernel::getG<CameraManager>();

	playerManager->onPostPlayerCreation.push_back(onPostPlayerCreation);
	LevelManager::onLevelUnload.push_back(onLevelUnload);
	LevelManager::onLevelLoad.push_back(onLevelLoad);
	LKernel::getG<Pauser>()->pauseEvent.push_back(bind(&SoundMain::pauseEvent,this,placeholders::_1));

	E_SOUND_ENGINE_OPTIONS flags = (E_SOUND_ENGINE_OPTIONS) (ESEO_DEFAULT_OPTIONS | ESEO_MUTE_IF_NOT_FOCUSED | ESEO_MULTI_THREADED);
	engine = createIrrKlangDevice(E_SOUND_OUTPUT_DRIVER::ESOD_AUTO_DETECT, flags);
	engine->setDefault3DSoundMinDistance(50);

	log("[Loading] IrrKlang and SoundMain initialised!");
}

irrklang::ISoundSource* SoundMain::getSource(std::string filename)
{
	string _path=filename;
	auto filenamePos=filename.rfind("/");
	if (filenamePos!=filename.size())
		filename=string(filename,filenamePos);

	auto fullpathIt = fileList.find(_path);
	if (fullpathIt != fileList.end())
		return engine->getSoundSource(fullpathIt->second.c_str(), true);
	else
		throw string("SoundMain::getSource: "+_path + " was not found!");
}

float SoundMain::getEngineDefault3DSoundMinDistance()
{
	return engine->getDefault3DSoundMinDistance();
}

void SoundMain::addSoundComponent(SoundComponent* sc)
{
	components.insert(sc);
}

ISound* SoundMain::play3D(ISoundSource* source, const Vector3& pos, bool looping, bool startPaused, bool sfx)
{
	log(string("[Sounds] Creating 3D sound: ") + source->getName() + " Looping: " + (looping ? "true" : "false"));

	ISound* sound = engine->play3D(source, vec3df(pos.x, pos.y, pos.z), looping, startPaused, sfx);
	sounds.push_back(sound);

	if (!enableSounds)
	{
		sound->setIsPaused(true);
		sound->setVolume(0);
	}
	else if (startPaused)
		sound->setIsPaused(true);

	return sound;
}

void SoundMain::pauseEvent(PausingState state)
{
	engine->setAllSoundsPaused(state == PausingState::Pausing);
}