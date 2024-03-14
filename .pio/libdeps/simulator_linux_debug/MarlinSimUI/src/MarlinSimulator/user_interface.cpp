#include <SDL2/SDL.h>
#include <gl.h>
#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>
#include "user_interface.h"

std::map<std::string, std::shared_ptr<UiWindow>> UserInterface::ui_elements;

void DockSpace() {
  ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
  ImGuiViewport* viewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowPos(viewport->WorkPos);
  ImGui::SetNextWindowSize(viewport->WorkSize);
  ImGui::SetNextWindowViewport(viewport->ID);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
  window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
  window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

  if (ImGuiDockNodeFlags_None & ImGuiDockNodeFlags_PassthruCentralNode)
      window_flags |= ImGuiWindowFlags_NoBackground;

  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
  ImGui::Begin("DockSpaceWindwow", nullptr, window_flags);
  ImGui::PopStyleVar();
  ImGui::PopStyleVar(2);

  ImGuiID dockspace_id = ImGui::GetID("DockSpace");
  ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

  ImGui::End();
}

UserInterface::UserInterface() {
  ImGui::LoadIniSettingsFromMemory(ImGuiDefaultLayout, strlen(ImGuiDefaultLayout));
  ImGui::LoadIniSettingsFromDisk("imgui.ini"); // Load early
}

UserInterface::~UserInterface() {
  ui_elements.clear();
}

void UserInterface::show() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL2_NewFrame(SDL_GL_GetCurrentWindow());
  ImGui::NewFrame();

  DockSpace();
  for (auto element : ui_elements) {
    element.second->show();
  }
  ImGui::EndFrame();

  if (!post_init_complete) {
    if (post_init) post_init();
    post_init_complete = true;
  }
}

void UserInterface::render() {
  ImGui::Render();
  {
    ImGuiIO& io = ImGui::GetIO();
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
      SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
      SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
      ImGui::UpdatePlatformWindows();
      ImGui::RenderPlatformWindowsDefault();
      SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
    }
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  }
}

