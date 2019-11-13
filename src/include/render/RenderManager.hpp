#ifndef MSCFINALPROJECT_RENDER_RENDERMANAGER_HPP
#define MSCFINALPROJECT_RENDER_RENDERMANAGER_HPP

#include <string>
#include <vml/mat4.hpp>

namespace render::RenderManager {
    void init();

    bool createGraphicsPipeline(const std::string& name);
    uint32_t getPipelineID(const std::string& name);

    void bindPipeline(uint32_t id);

    void resetPushConstants();
    void setPerspective(const vml::mat4& pers);
    void setView(const vml::mat4& view);
    void setModel(const vml::mat4& mode);
    void setTextureTransform(const vml::mat3& tt);

    void draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance);
    void drawRect2D();

    bool loadShaders();
    void unloadShaders();
    bool reloadShaders();


    void terminate();
}

#endif//MSCFINALPROJECT_RENDER_RENDERMANAGER_HPP
