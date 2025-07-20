#include <iostream>
#include "ResourcePool.h"
std::unordered_map<std::string, std::shared_ptr<Texture>> TexturePool;
std::unordered_map<std::string, std::shared_ptr<VertexShader>> VertexShaderPool;
std::unordered_map<std::string, std::shared_ptr<FragmentShader>> FragmentShaderPool;
std::unordered_map<std::string, std::shared_ptr<ShaderProgram>> ShaderProgramPool;