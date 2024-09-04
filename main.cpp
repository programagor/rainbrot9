#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
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

// Quaternion structure to hold four double components
struct Quaternion {
    mpfr_t r, i, j, k;  // Real and imaginary components
};



// Beam structure with Quaternion for mu and sigma
//class Beam {
//    public:
//        Quaternion mu;                  // Quaternion for mu parameters
//        Quaternion sigma;               // Quaternion for sigma parameters
//        int samples_total;
//        int samples_current;
//        std::string seed_start; // String seed
//        //MPFR_PRNG_state state_current;   // State of the PRNG
//        void get_sample(&result);        // Method to get a sample
//};




// Plate structure
struct Plate {
    /* 4D to 3D projection matrix */
    mpfr_t projection4[5][4];        // Projection matrix
    /*  3D to 2D projection matrix */
    mpfr_t projection3[4][3];      // Projection matrix
    // ColorMap colormap;              // Color map
    int width, height;                // Dimensions
    std::vector<std::vector<int64_t>> data; // Data
    std::vector<std::mutex> row_locks; // Locks for each row (TODO: make ReadWriteLock)

};
// except it needs to be a class so that we can just send it a quaternion and its iter count and it will do the rest.
// It should contain methods for loading from file, saving to file (do we need protobuf for this? Or can we just print floats in hexadecimal plus some header?),
// method to receive a quaternion for plotting, method for returning the 2D array scaled to 0-255 integers, ...

// Global state

// Beams section
bool show_beams_window = true;
//std::vector<Beam> beams;
int cpu_threads = 1;
bool beams_on = false;

// Model section
bool show_model_window = true;

// Plates section
bool show_plates_window = true;
std::vector<Plate> plates;


bool show_preferences_window = false;


bool show_imgui_demo = false;

void InitializeData() {
    // Add one beam with default values
    //beams.push_back({0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1, {"seed1", "seed2"}});

    // Add one plate with default values
    //plates.push_back({1024, 768, 0, std::vector<std::vector<int64_t>>(1024, std::vector<int64_t>(768, 0))});
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
            ImGuiIO& io = ImGui::GetIO(); // What does `ImGuiIO& io = ...` do? Answer: It creates a reference to the ImGuiIO struct. 
            ImGui::Separator();
            ImGui::Text("Dear ImGui %s", IMGUI_VERSION);
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

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);



    // Main loop
    bool done = false;

    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);

    while (!done)
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
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
        ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());


        // Controls window
        /*{
            ImGuiWindowFlags root_window_flags = //ImGuiWindowFlags_NoTitleBar |
                                //ImGuiWindowFlags_NoResize |
                                //ImGuiWindowFlags_NoMove |
                                //ImGuiWindowFlags_NoCollapse |
                                //ImGuiWindowFlags_NoBringToFrontOnFocus;
                                0;

            ImGui::Begin("Rainbrot n+1", NULL, root_window_flags);

            
            ImGui::End();
        }*/

        // Beams window
        if (show_beams_window)
        {
            // Dock to the left of the root window

            ImGui::Begin("Beams", &show_beams_window);
            if (ImGui::Button("Add Beam"))
            {
                //beams.push_back({
                
            }
            ImGui::End();
        }

        // Model window
        if (show_model_window)
        {
            ImGui::Begin("Model", &show_model_window);
            ImGui::End();
        }

        // Plates window
        if (show_plates_window)
        {
            ImGui::Begin("Plates", &show_plates_window);
            // Total number of plates:
            ImGui::Text("Total number of plates: %lu", plates.size());
            // List of plates:
            // Bounding scrolling box:
            ImGui::BeginChild("PlatesList", ImVec2(0, 300), true);
            for (size_t i = 0; i < plates.size(); i++)
            {
                char label[128];
                sprintf(label, "Plate %lx", i);
                if (ImGui::CollapsingHeader(label))
                {
                    ImGui::Text("Width: %d", plates[i].width);
                    ImGui::Text("Height: %d", plates[i].height);
                    //ImGui::Text("Data: %d x %d", plates[i].data.size(), plates[i].data[0].size());
                }
            }
            ImGui::EndChild();
            // Add plate button:
            if (ImGui::Button("Add Plate"))
            {
                //plates.push_back({1024, 1024, std::vector<std::vector<int64_t>>(1024, std::vector<int64_t>(1024, 0))});
            }
            ImGui::End();
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