#include "model.hpp"

static std::vector<std::string> g_models = {
    "Mandelbrot", "Julia", "Burning Ship", "Tricorn", "Mandelbar", "Phoenix"
};

const std::vector<std::string>& ModelRegistry::list() {
    return g_models;
}
