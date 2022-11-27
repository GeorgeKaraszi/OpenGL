#include "GLCore.h"
#include "SandboxLayer.h"
#include "ParticleLayer.hpp"

using namespace GLCore;

class Sandbox : public Application
{
public:
  Sandbox() :  Application("Sandbox", 1920, 1080)
  {
    PushLayer(new ParticleLayer());
  }
};

int main()
{
  std::unique_ptr<Sandbox> app = std::make_unique<Sandbox>();
  app->Run();
}
