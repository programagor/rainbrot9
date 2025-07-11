#include "external/imgui/imgui.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include <memory>
#include <thread>
#include <mutex>
#include <stdio.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_mutex.h>
// GMP and MPFR
#include <gmp.h>
#include <mpfr.h>



#include <vector>
#include <string>

#include <memory> // for std::unique_ptr and std::shared_ptr

// Quaternion struct
// Constructor can take either 4 strings or 4 doubles, or nothing for all zeros.
// Copy constructor and assignment operator are defined.
// .set(4*str), .set(4*double), 
#include "quaternion.hpp"
#include "beam.hpp"
#include "plate.hpp"
#include "model.hpp"

// C++11 make_unique and make_shared
template <typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

// Helper function to create a shared pointer ()
template <typename T, typename... Args>
std::shared_ptr<T> make_shared(Args&&... args) {
    return std::shared_ptr<T>(new T(std::forward<Args>(args)...));
}



// Global state

// Beams section
bool show_beams_window = true;
bool show_beam_modal = false;
std::vector<std::unique_ptr<Beam>> beams;
int cpu_threads = 1;
bool beams_on = false;


// Model section
bool show_model_window = true;
std::vector<std::string> models = ModelRegistry::list();
int item_current = 0;
int max_iterations = 100;
float escape_radius = 64.0f;

// Plates section
bool show_plates_window = true;
bool show_plate_modal = false;
std::vector<std::unique_ptr<Plate>> plates;
// color map
const char* colormaps[] = { "Rainbow", "Grayscale", "Hot", "Cool", "Spring", "Summer", "Autumn", "Winter", "Bone", "Copper", "Pink", "Jet", "Hsv", "Flag", "Prism", "Ocean", "Cubehelix", "Turbo", "Viridis", "Plasma", "Inferno", "Magma", "Cividis" };

bool done = false;

bool show_preferences_window = false;


bool show_imgui_demo = false;

void InitializeData() {
    // Add one beam with default values
    //beams.push_back({0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1, {"seed1", "seed2"}});

    // Add one plate with default values
    //plates.push_back({1024, 768, 0, std::vector<std::vector<int64_t>>(1024, std::vector<int64_t>(768, 0))});
}

static bool VectorGetter(void* data, int idx, const char** out_text)
{
    auto& vec = *static_cast<std::vector<std::string>*>(data);
    if (idx < 0 || idx >= (int)vec.size()) return false;
    *out_text = vec[idx].c_str();
    return true;
}

static void ShowMainMenuBar()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New scene", "CTRL+N"))
            {
            }
            if (ImGui::MenuItem("Open scene", "CTRL+O"))
            {
            }
            if (ImGui::MenuItem("Save scene", "CTRL+S"))
            {
            }
            if (ImGui::MenuItem("Save scene as...", "CTRL+SHIFT+S"))
            {
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit", "ALT+F4"))
            {
                done = true;
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit", true))
        {
            if (ImGui::MenuItem("Undo", "CTRL+Z", false, false)) {}
            if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}
            ImGui::Separator();
            ImGui::MenuItem("Preferences", NULL, &show_preferences_window);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View"))
        {
            ImGui::MenuItem("Beams", NULL, &show_beams_window);
            ImGui::MenuItem("Model", NULL, &show_model_window);
            ImGui::MenuItem("Plates", NULL, &show_plates_window);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help"))
        {
            ImGui::MenuItem("Show ImGui Demo", NULL, &show_imgui_demo);
            ImGuiIO& io = ImGui::GetIO();
            ImGui::Text("Dear ImGui %s", IMGUI_VERSION);
            ImGui::Separator();
            ImGui::Text("GUI loop: %.3f ms/frm (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate); // Should this be ImGui::Text or ImGui::MenuItem?
            ImGui::Text("Beams on: %s", beams_on ? "yes" : "no");
            ImGui::Separator();
            
            ImGui::Separator();
            if (ImGui::MenuItem("About", NULL))
            {
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

// Main code
int main(int, char**)
{
    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    // Decide GL+GLSL versions
    #if defined(IMGUI_IMPL_OPENGL_ES2)
        // GL ES 2.0 + GLSL 100
        const char* glsl_version = "#version 100";
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    #elif defined(__APPLE__)
        // GL 3.2 Core + GLSL 150
        const char* glsl_version = "#version 150";
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    #else
        // GL 3.0 + GLSL 130
        const char* glsl_version = "#version 130";
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    #endif

        // From 2.0.18: Enable native IME.
    #ifdef SDL_HINT_IME_SHOW_UI
        SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
    #endif

    // Create window with graphics context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window* window = SDL_CreateWindow("Rainbrot n+1", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
    if (window == nullptr)
    {
        printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
        return -1;
    }

    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    // Setup Dear ImGui style
    //ImGui::StyleColorsDark();
    ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);


    // Our state

    InitializeData();
    int edit_beam_index = -1;
    int edit_plate_index = -1;

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);



    // Main loop


    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);

    while (!done)
    {
        // Poll and handle events (inputs, window resize, etc.)
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                done = true;
        }
        if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED)
        {
            SDL_Delay(10);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        ShowMainMenuBar();

        // Enable docking
        ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());

        // Beams window
        int to_delete = -1;
        if (show_beams_window)
        {
            ImGui::SetNextWindowDockID(0, ImGuiCond_FirstUseEver);
            ImGui::Begin("Beams", &show_beams_window);
            ImGui::Text("Total number of beams: %lu", beams.size());
            ImGui::BeginChild("BeamsList", ImVec2(0, 300), true);
            for (size_t i = 0; i < beams.size(); i++)
            {
                char label[128];
                sprintf(label, "Beam %zu", i);  // Corrected to use %zu for size_t
                if (ImGui::CollapsingHeader(label))
                {
                    ImGui::Text("Mu:    %f + %f i + %f j + %f k", mpfr_get_d(beams[i]->mu.r, MPFR_RNDN), mpfr_get_d(beams[i]->mu.i, MPFR_RNDN), mpfr_get_d(beams[i]->mu.j, MPFR_RNDN), mpfr_get_d(beams[i]->mu.k, MPFR_RNDN));
                    ImGui::Text("Sigma: %f + %f i + %f j + %f k", mpfr_get_d(beams[i]->sigma.r, MPFR_RNDN), mpfr_get_d(beams[i]->sigma.i, MPFR_RNDN), mpfr_get_d(beams[i]->sigma.j, MPFR_RNDN), mpfr_get_d(beams[i]->sigma.k, MPFR_RNDN));
                    ImGui::Text("N: %d / %d", beams[i]->samples_current, beams[i]->samples_total);
                    ImGui::ProgressBar(beams[i]->samples_total ? (float)beams[i]->samples_current / (float)beams[i]->samples_total : 0);
                    ImGui::Text("Seed: %s", mpz_get_str(NULL, 10, beams[i]->seed_start));
                    
                    // create string for label ("Edit" + i):
                    std::string edit_label = "Edit##beam_" + std::to_string(i);
                    if (ImGui::Button(edit_label.c_str()))
                    {
                        show_beam_modal = true;
                        edit_beam_index = i;  // Store the current index for editing
                    }

                    ImGui::SameLine();

                    std::string remove_label = "Remove##beam_" + std::to_string(i);
                    if (ImGui::Button(remove_label.c_str()))
                    {
                        to_delete = i;
                    }
                }
            }
            if (to_delete != -1)
            {
                beams.erase(beams.begin() + to_delete);
                to_delete = -1;
            }
            ImGui::EndChild();
            if (ImGui::Button("Add Beam"))
            {
                show_beam_modal = true;
                edit_beam_index = -1; // New beam
            }
            ImGui::End();
        }


        
        // Modal for adding/editing beam
        
        std::string beam_modal_name = edit_beam_index != -1 ? "Edit Beam " + std::to_string(edit_beam_index) : "Add Beam";

        if (show_beam_modal)
        {
            if (!ImGui::IsPopupOpen(beam_modal_name.c_str())){
                ImGui::OpenPopup(beam_modal_name.c_str());
            }
        }
        if(ImGui::BeginPopupModal(beam_modal_name.c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            static int samples_total = 0;
            static char seed_start[128] = "0";
            static char mu_r[128] = "0.0";
            static char mu_i[128] = "0.0";
            static char mu_j[128] = "0.0";
            static char mu_k[128] = "0.0";
            static char sigma_r[128] = "0.0";
            static char sigma_i[128] = "0.0";
            static char sigma_j[128] = "0.0";
            static char sigma_k[128] = "0.0";

            static bool preload_variables_from_vector = true;

            if (edit_beam_index != -1 && beams.size() > edit_beam_index && preload_variables_from_vector)
            {
                // Pre-fill with existing data if editing
                Beam* beam = beams[edit_beam_index].get();
                samples_total = beam->samples_total;
                beam->mu.get(mu_r, mu_i, mu_j, mu_k);
                beam->sigma.get(sigma_r, sigma_i, sigma_j, sigma_k);
                mpz_get_str(seed_start, 10, beam->seed_start);
                preload_variables_from_vector = false;
            }

            ImGui::InputInt("Total Samples", &samples_total);
            ImGui::InputText("Seed Start", seed_start, IM_ARRAYSIZE(seed_start));
            ImGui::InputText("Mu r", mu_r, IM_ARRAYSIZE(mu_r));
            ImGui::InputText("Mu i", mu_i, IM_ARRAYSIZE(mu_i));
            ImGui::InputText("Mu j", mu_j, IM_ARRAYSIZE(mu_j));
            ImGui::InputText("Mu k", mu_k, IM_ARRAYSIZE(mu_k));
            ImGui::InputText("Sigma r", sigma_r, IM_ARRAYSIZE(sigma_r));
            ImGui::InputText("Sigma i", sigma_i, IM_ARRAYSIZE(sigma_i));
            ImGui::InputText("Sigma j", sigma_j, IM_ARRAYSIZE(sigma_j));
            ImGui::InputText("Sigma k", sigma_k, IM_ARRAYSIZE(sigma_k));

            if (ImGui::Button("OK", ImVec2(120, 0))) 
            {
                preload_variables_from_vector = true;
                printf("Saving beam data: %d %s %s %s %s %s %s %s %s %s\n", samples_total, seed_start, mu_r, mu_i, mu_j, mu_k, sigma_r, sigma_i, sigma_j, sigma_k);
                if (edit_beam_index == -1)
                {
                    // Creating a new beam
                    printf("Creating new beam\n");
                    std::unique_ptr<Beam> new_beam = make_unique<Beam>();
                    beams.push_back(std::move(new_beam));
                    edit_beam_index = beams.size() - 1; // Set the new index
                }

                // Set the beam data based on user input
                Beam* beam = beams[edit_beam_index].get();
                beam->samples_total = samples_total;
                mpz_set_str(beam->seed_start, seed_start, 10);
                beam->mu.set(mu_r, mu_i, mu_j, mu_k);
                beam->sigma.set(sigma_r, sigma_i, sigma_j, sigma_k);

                
                ImGui::CloseCurrentPopup();
                show_beam_modal = false;
                edit_beam_index = -1;
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0)))
            {
                preload_variables_from_vector = true;
                ImGui::CloseCurrentPopup();
                show_beam_modal = false;
                edit_beam_index = -1;
            }
            
            ImGui::EndPopup();
        }


        // Model window
        if (show_model_window)
        {
            ImGui::Begin("Model", &show_model_window);
            // Drop down for model
            ImGui::Combo("Model", &item_current, VectorGetter, &models, models.size());
            // Max Iterations
            ImGui::InputInt("Max Iterations", &max_iterations);
            // Escape Radius
            ImGui::InputFloat("Escape Radius", &escape_radius);
            
            ImGui::End();
        }

        // Plates window
        if (show_plates_window)
        {
            ImGui::Begin("Plates", &show_plates_window);
            ImGui::Text("Total number of plates: %lu", plates.size());
            ImGui::BeginChild("PlatesList", ImVec2(0, 300), true);
            for (size_t i = 0; i < plates.size(); i++)
            {
                char label[128];
                sprintf(label, "Plate %zu", i);  // Corrected to use %zu for size_t
                if (ImGui::CollapsingHeader(label))
                {
                    ImGui::Text("Width: %d", plates[i]->width);
                    ImGui::Text("Height: %d", plates[i]->height);

                    // create string for label ("Edit" + i):
                    std::string edit_label = "Edit##plate_" + std::to_string(i);
                    if (ImGui::Button(edit_label.c_str()))
                    {
                        show_plate_modal = true;
                        edit_plate_index = i;  // Store the current index for editing
                    }

                    ImGui::SameLine();

                    std::string remove_label = "Remove##plate_" + std::to_string(i);
                    if (ImGui::Button(remove_label.c_str()))
                    {
                        to_delete = i;
                    }
                }
            }
            if (to_delete != -1)
            {
                plates.erase(plates.begin() + to_delete);
                to_delete = -1;
            }
            ImGui::EndChild();
            if (ImGui::Button("Add Plate"))
            {
                show_plate_modal = true;
                edit_plate_index = -1; // New plate
            }
            ImGui::End();
        }

        // Modal for adding/editing plate
        std::string plate_modal_name = edit_plate_index != -1 ? "Edit Plate " + std::to_string(edit_plate_index) : "Add Plate";

        if (show_plate_modal)
        {
            if (!ImGui::IsPopupOpen(plate_modal_name.c_str())){
                ImGui::OpenPopup(plate_modal_name.c_str());
            }
        }
        if(ImGui::BeginPopupModal(plate_modal_name.c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            static int width = 1024;
            static int height = 1024;

            static bool preload_variables_from_vector = true;

            if (edit_plate_index != -1 && plates.size() > edit_plate_index && preload_variables_from_vector)
            {
                // Pre-fill with existing data if editing
                Plate* plate = plates[edit_plate_index].get();
                width = plate->width;
                height = plate->height;
                preload_variables_from_vector = false;
            }

            ImGui::InputInt("Width", &width);
            ImGui::InputInt("Height", &height);

            if (ImGui::Button("OK", ImVec2(120, 0))) 
            {
                preload_variables_from_vector = true;
                printf("Saving plate data: %d %d\n", width, height);
                if (edit_plate_index == -1)
                {
                    // Creating a new plate
                    printf("Creating new plate\n");
                    std::unique_ptr<Plate> new_plate = make_unique<Plate>(width, height);
                    plates.push_back(std::move(new_plate));
                    edit_plate_index = plates.size() - 1; // Set the new index
                }

                // Set the plate data based on user input
                Plate* plate = plates[edit_plate_index].get();
                plate->width = width;
                plate->height = height;

                
                ImGui::CloseCurrentPopup();
                show_plate_modal = false;
                edit_plate_index = -1;
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0)))
            {
                preload_variables_from_vector = true;
                ImGui::CloseCurrentPopup();
                show_plate_modal = false;
                edit_plate_index = -1;
            }
            
            ImGui::EndPopup();
        }

        // Modal for the Preferences window
        if (show_preferences_window)
        {
            if (!ImGui::IsPopupOpen("Preferences")){
                ImGui::OpenPopup("Preferences");
            }
        }
        if(ImGui::BeginPopupModal("Preferences", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            static bool dummyCheck = false;
            ImGui::Checkbox("what",&dummyCheck);


            if (ImGui::Button("Ok", ImVec2(120, 0)))
            {
                ImGui::CloseCurrentPopup();
                show_preferences_window = false;
            }

            ImGui::EndPopup();
        }

        if (show_imgui_demo)
        {
            ImGui::ShowDemoWindow(&show_imgui_demo);
        }

        // Rendering
        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}