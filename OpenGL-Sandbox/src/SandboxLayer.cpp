#include "SandboxLayer.h"
#include <stb_image.h>
#include <array>

using namespace GLCore;
using namespace GLCore::Utils;

struct Vec4
{
  float x, y, z, w;
};

struct Vec3
{
  float x, y, z;
};

struct Vec2
{
  float x, y;
};

struct Vertex
{
  Vec3  Position;
  Vec4  Color;
  Vec2  TexCoord;
  float TexID;
};

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

static std::array<Vertex, 4> CreateQuad(float x, float y, float textureID)
{
  float size = 1.0f;

  Vertex v0;
  v0.Position = { x, y, 0.0f };
  v0.Color    = { 0.18f, 0.6f, 0.96f, 1.0f };
  v0.TexCoord = { 0.0f, 0.0f };
  v0.TexID    = textureID;

  Vertex v1;
  v1.Position = { x + size, y, 0.0f };
  v1.Color    = { 0.18f, 0.6f, 0.96f, 1.0f };
  v1.TexCoord = { 1.0f, 0.0f };
  v1.TexID    = textureID;

  Vertex v2;
  v2.Position = { x + size, y + size, 0.0f };
  v2.Color    = { 0.18f, 0.6f, 0.96f, 1.0f };
  v2.TexCoord = { 1.0f, 1.0f };
  v2.TexID    = textureID;

  Vertex v3;
  v3.Position = { x, y + size, 0.0f };
  v3.Color    = { 0.18f, 0.6f, 0.96f, 1.0f };
  v3.TexCoord = { 0.0f, 1.0f };
  v3.TexID    = textureID;

  return { v0, v1, v2, v3 };
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

  glCreateBuffers(1, &m_QuadVB);
  glBindBuffer(GL_ARRAY_BUFFER, m_QuadVB);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 1000, nullptr, GL_DYNAMIC_DRAW);

  glEnableVertexArrayAttrib(m_QuadVB, 0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *) offsetof(Vertex, Position));

  glEnableVertexArrayAttrib(m_QuadVB, 1);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *) offsetof(Vertex, Color));

  glEnableVertexArrayAttrib(m_QuadVB, 2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *) offsetof(Vertex, TexCoord));

  glEnableVertexArrayAttrib(m_QuadVB, 3);
  glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *) offsetof(Vertex, TexID));

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

  auto q0 = CreateQuad(m_QuadPosition[0], m_QuadPosition[1], 0.0f);
  auto q1 = CreateQuad(0.5f, -0.5f, 1.0f);

  Vertex vertices[8];
  memcpy(vertices, q0.data(), sizeof(Vertex) * q0.size());
  memcpy(vertices + q0.size(), q1.data(), sizeof(Vertex) * q1.size());

  // Set dynamic vertex buffer
  glBindBuffer(GL_ARRAY_BUFFER, m_QuadVB);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glUseProgram(m_Shader->GetRendererID());

  glBindTextureUnit(0, m_Logo1Tex);
  glBindTextureUnit(1, m_Logo2Tex);

  int location = glGetUniformLocation(m_Shader->GetRendererID(), "u_ViewProjection");
  glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(m_CameraController.GetCamera().GetViewProjectionMatrix()));

  glBindVertexArray(m_QuadVA);
  glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, nullptr);
}

void SandboxLayer::OnImGuiRender()
{
  ImGui::Begin("Controls");
  ImGui::DragFloat2("Quad Position", m_QuadPosition, 0.01f);
  ImGui::End();

}
