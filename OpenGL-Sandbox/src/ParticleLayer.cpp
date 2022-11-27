#include "ParticleLayer.hpp"

using namespace GLCore;
using namespace GLCore::Utils;

ParticleLayer::ParticleLayer() : m_CameraController(16.0f / 9.0f)
{

}

void ParticleLayer::OnAttach()
{
  GLCore::Utils::EnableGLDebugging();

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  m_Particle.ColorBegin        = { 254 / 255.0f, 212 / 255.0f, 123 / 255.0f, 1.0f };
  m_Particle.ColorEnd          = { 254 / 255.0f, 109 / 255.0f, 41 / 255.0f, 0.0f };
  m_Particle.SizeBegin         = 0.5f;
  m_Particle.SizeVariation     = 0.3f;
  m_Particle.SizeEnd           = 0.0f;
  m_Particle.LifeTime          = 1.0f;
  m_Particle.Velocity          = { 0.0f, 0.0f };
  m_Particle.VelocityVariation = { 3.0f, 1.0f };
  m_Particle.Position          = { 0.0f, 0.0f };
}

void ParticleLayer::OnDetach()
{
}

void ParticleLayer::OnEvent(GLCore::Event &event)
{
  m_CameraController.OnEvent(event);

  if (event.GetEventType() == GLCore::EventType::WindowResize)
  {
    GLCore::WindowResizeEvent &e = (GLCore::WindowResizeEvent &) event;
    glViewport(0, 0, e.GetWidth(), e.GetHeight());
  }
}

void ParticleLayer::OnUpdate(GLCore::Timestep ts)
{
  m_CameraController.OnUpdate(ts);

  glClearColor(0, 0, 0, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  if (GLCore::Input::IsMouseButtonPressed(HZ_MOUSE_BUTTON_LEFT))
  {
    auto [x, y] = Input::GetMousePosition();
    auto width  = Application::Get().GetWindow().GetWidth();
    auto height = Application::Get().GetWindow().GetHeight();

    auto bounds = m_CameraController.GetBounds();
    auto pos    = m_CameraController.GetCamera().GetPosition();
    x = (x / width) * bounds.GetWidth() - bounds.GetWidth() / 2 + pos.x;
    y = bounds.GetHeight() * 0.5f - (y / height) * bounds.GetHeight();
    m_Particle.Position = { x + pos.x, y + pos.y };
    for (int i = 0; i < 5; i++)
    {
      m_ParticleSystem.Emit(m_Particle);
    }

  }

  m_ParticleSystem.OnUpdate(ts);
  m_ParticleSystem.OnRender(m_CameraController.GetCamera());
}

void ParticleLayer::OnImGuiRender()
{
  ImGui::Begin("Settings");
  ImGui::ColorEdit4("Birth Color", glm::value_ptr(m_Particle.ColorBegin));
  ImGui::ColorEdit4("Death Color", glm::value_ptr(m_Particle.ColorEnd));
  ImGui::DragFloat("Life Time", &m_Particle.LifeTime, 0.1f, 0.0f, 1000.0f);
  ImGui::End();
}
