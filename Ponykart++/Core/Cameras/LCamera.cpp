#include <string>
#include "Core/Cameras/LCamera.h"

using namespace Ponykart::Core;

LCamera::LCamera(const std::string& Name)
 : name(Name)
{
}

const Ogre::Camera* const LCamera::getCamera() const
{
	return camera;
}

const Ogre::SceneNode* const LCamera::getCameraNode() const
{
	return cameraNode;
}
std::string LCamera::getName() const
{
	return name;
}