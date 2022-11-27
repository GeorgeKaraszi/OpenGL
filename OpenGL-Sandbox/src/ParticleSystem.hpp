#ifndef OPENGL_CHONO_PARTICLESYSTEM_HPP
#define OPENGL_CHONO_PARTICLESYSTEM_HPP

#include <GLCore.h>
#include <GLCoreUtils.h>

#define PARTICLE_MAX_COUNT 1000

struct ParticleProps
{
  glm::vec2 Position;
  glm::vec2 Velocity;
  glm::vec2 VelocityVariation;
  glm::vec4 ColorBegin;
  glm::vec4 ColorEnd;
  float     SizeBegin;
  float     SizeEnd;
  float     SizeVariation;
  float     LifeTime = 1.0f;
};

class ParticleSystem
{
public:
  ParticleSystem();

  void OnUpdate(GLCore::Timestep ts);
  void OnRender(GLCore::Utils::OrthographicCamera &camera);
  void Emit(const ParticleProps &particleProps);

private:
  struct Particle
  {
    glm::vec2 Position;
    glm::vec2 Velocity;
    glm::vec4 ColorBegin, ColorEnd;
    float Rotation = 0.0f;
    float SizeBegin, SizeEnd;

    float LifeTime = 1.0f;
    float LifeRemaining = 0.0f;

    bool Active = false;
  };

  std::vector<Particle> m_ParticlePool;
  int32_t              m_PoolIndex = (PARTICLE_MAX_COUNT - 1);
  GLuint                m_QuadVA    = 0;

  std::unique_ptr<GLCore::Utils::Shader> m_ParticleShader;

  GLuint m_ParticleShaderViewProj;
  GLuint m_ParticleShaderTransform;
  GLuint m_ParticleShaderColor;
};

#endif //OPENGL_CHONO_PARTICLESYSTEM_HPP
