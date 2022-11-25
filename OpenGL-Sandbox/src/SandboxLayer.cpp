#include "SandboxLayer.h"
#include <stb_image.h>

using namespace GLCore;
using namespace GLCore::Utils;

static GLuint LoadTexture(const std::string &path)
{
  int w, h, bits;
  stbi_set_flip_vertically_on_load(1);

  auto   *pixels = stbi_load(path.c_str(), &w, &h, &bits, 4);
  GLuint textureID;

  glCreateTextures(GL_TEXTURE_2D, 1, &textureID);
  glBindTexture(GL_TEXTURE_2D, textureID);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

  stbi_image_free(pixels);

  return textureID;
}

SandboxLayer::SandboxLayer() : m_CameraController(16.0f / 9.0f)
{
}

SandboxLayer::~SandboxLayer()
{
}

void SandboxLayer::OnAttach()
{
  EnableGLDebugging();

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  m_Shader = Shader::FromGLSLTextFiles(
      "assets/shaders/test.vert.glsl",
      "assets/shaders/test.frag.glsl"
                                      );

  glUseProgram(m_Shader->GetRendererID());
  auto location    = glGetUniformLocation(m_Shader->GetRendererID(), "u_Textures");
  int  samplers[2] = { 0, 1 };
  glUniform1iv(location, 2, samplers);

  glCreateVertexArrays(1, &m_QuadVA);
  glBindVertexArray(m_QuadVA);

  // @formatter:off
  float vertices[] = {
      // X |   Y  |  Z  |   R  |  G   |  B  |  A   | Tex X | Tex Y | Tex IDX |IDX for Indices
      -0.5f, -0.5f, 0.0f, 0.18f, 0.6f, 0.96f, 1.0f,  0.0f,    0.0f,   0.0f,      // 0
       0.5f, -0.5f, 0.0f, 0.18f, 0.6f, 0.96f, 1.0f,  1.0f,    0.0f,   0.0f,      // 1
       0.5f,  0.5f, 0.0f, 0.18f, 0.6f, 0.96f, 1.0f,  1.0f,    1.0f,   0.0f,      // 2
      -0.5f,  0.5f, 0.0f, 0.18f, 0.6f, 0.96f, 1.0f,  0.0f,    1.0f,   0.0f,      // 3

      // Other Square Spaced out correctly
      1.5f, -0.5f, 0.0f, 1.0f, 0.93f, 0.24f, 1.0f,  0.0f,    0.0f,   1.0f,      // 4
      2.5f, -0.5f, 0.0f, 1.0f, 0.93f, 0.24f, 1.0f,  1.0f,    0.0f,   1.0f,      // 5
      2.5f,  0.5f, 0.0f, 1.0f, 0.93f, 0.24f, 1.0f,  1.0f,    1.0f,   1.0f,      // 6
      1.5f,  0.5f, 0.0f, 1.0f, 0.93f, 0.24f, 1.0f,  0.0f,    1.0f,   1.0f,      // 7
  };
  // @formatter:on

  glCreateBuffers(1, &m_QuadVB);
  glBindBuffer(GL_ARRAY_BUFFER, m_QuadVB);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glEnableVertexArrayAttrib(m_QuadVB, 0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 10, 0);

  glEnableVertexArrayAttrib(m_QuadVB, 1);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 10, (const void *) 12);

  glEnableVertexArrayAttrib(m_QuadVB, 2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 10, (const void *) 28);

  glEnableVertexArrayAttrib(m_QuadVB, 3);
  glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(float) * 10, (const void *) 36);

  uint32_t indices[] = {
      0, 1, 2, 2, 3, 0,
      4, 5, 6, 6, 7, 4
  };
  glCreateBuffers(1, &m_QuadIB);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_QuadIB);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  m_Logo1Tex = LoadTexture("assets/textures/logo1.png");
  m_Logo2Tex = LoadTexture("assets/textures/logo2.png");
}

void SandboxLayer::OnDetach()
{
  glDeleteVertexArrays(1, &m_QuadVA);
  glDeleteBuffers(1, &m_QuadVB);
  glDeleteBuffers(1, &m_QuadIB);
}

void SandboxLayer::OnEvent(Event &event)
{
  m_CameraController.OnEvent(event);

  EventDispatcher dispatcher(event);
  dispatcher.Dispatch<MouseButtonPressedEvent>(
      [&](MouseButtonPressedEvent &e) {
        m_SquareColor = m_SquareAlternateColor;
        return false;
      });
  dispatcher.Dispatch<MouseButtonReleasedEvent>(
      [&](MouseButtonReleasedEvent &e) {
        m_SquareColor = m_SquareBaseColor;
        return false;
      });
}

void SandboxLayer::OnUpdate(Timestep ts)
{
  m_CameraController.OnUpdate(ts);

  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glUseProgram(m_Shader->GetRendererID());

  glBindTextureUnit(0, m_Logo1Tex);
  glBindTextureUnit(1, m_Logo2Tex);

  int location = glGetUniformLocation(m_Shader->GetRendererID(), "u_ViewProjection");
  glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(m_CameraController.GetCamera().GetViewProjectionMatrix()));

  location = glGetUniformLocation(m_Shader->GetRendererID(), "u_Color");
  glUniform4fv(location, 1, glm::value_ptr(m_SquareColor));

  glBindVertexArray(m_QuadVA);
  glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, nullptr);
}

void SandboxLayer::OnImGuiRender()
{
  ImGui::Begin("Controls");
  if (ImGui::ColorEdit4("Square Base Color", glm::value_ptr(m_SquareBaseColor)))
  {
    m_SquareColor = m_SquareBaseColor;
  }
  ImGui::ColorEdit4("Square Alternate Color", glm::value_ptr(m_SquareAlternateColor));
  ImGui::End();
}
