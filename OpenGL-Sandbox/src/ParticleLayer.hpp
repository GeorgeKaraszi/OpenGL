#ifndef OPENGL_CHONO_PARTICLELAYER_HPP
#define OPENGL_CHONO_PARTICLELAYER_HPP

#include <GLCore.h>
#include <GLCoreUtils.h>
#include "ParticleSystem.hpp"

class ParticleLayer : public GLCore::Layer
{
public:
  ParticleLayer();
  virtual ~ParticleLayer() = default;

  virtual void OnAttach() override;
  virtual void OnDetach() override;
  virtual void OnEvent(GLCore::Event &event) override;
  virtual void OnUpdate(GLCore::Timestep ts) override;
  virtual void OnImGuiRender() override;

private:
  GLCore::Utils::OrthographicCameraController m_CameraController;
  ParticleSystem                              m_ParticleSystem;
  ParticleProps                               m_Particle;
};

#endif //OPENGL_CHONO_PARTICLELAYER_HPP
