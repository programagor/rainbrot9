#ifndef MODEL_HPP
#define MODEL_HPP

#include <string>
#include <vector>

class ModelRegistry {
public:
    static const std::vector<std::string>& list();
};

#endif // MODEL_HPP
