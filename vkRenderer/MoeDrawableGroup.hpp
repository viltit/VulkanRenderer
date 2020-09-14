
/*
 * Very wip:
 * In order to draw different kind of shapes, for example textured vs. colored, we need different pipelines with
 * different shaders.
 * The idea of this class is to group a pipeline and the objects it draws together. The renderer then takes in
 * an array of pipelines.
*/

#include "wrapper/MoeVkArrayBuffer.hpp"
#include "wrapper/MoeVkDescriptorSet.hpp"
#include "wrapper/MoeTexture.hpp"
#include "MoeDrawable.hpp"
#include "wrapper/MoeVkPipeline.hpp"
#include "MoeVkTexturedDrawable.hpp"

namespace moe {
/*
 * Different shapes need different vertices (not all need uv-coordiantes, not all need normals, etc)
 * Right now, we use a template to solve this, because at this point we are relativly low-level and
 * an interface might be too much
 */



template <class T>
class WipDrawable {

public:

    // update the descriptor sets
    void update();

    MoeVkArrayBuffer<T>*   vertexBuffer() { return  _vertexBuffer; }
    // vs:
    // MoeVkArrayBuffer<IVertex>
    MoeVkArrayBuffer<uint32_t>*  indexBuffer()  { return _indexBuffer; }
    MoeVkDescriptorSet& descriptors() { return *_descriptors; }

private:
    MoeVkArrayBuffer<T>*            _vertexBuffer;  // TODO: fetch from a cache
    MoeVkArrayBuffer<uint32_t>*     _indexBuffer;
    MoeTexture                      _texture;       // TODO: Not all shapes need a texture. Also fetch from a cache

    MoeVkDescriptorSet*     _descriptors;
    Drawable*               _drawable;
};


template <class T>
class MoeDrawableGroup {
public:
    MoeDrawableGroup(MoeVkPipeline& pipeline, std::vector<WipDrawable<T>>&)
        : _pipeline { pipeline },
          _drawables { _drawables }
    {}
private:
    MoeVkPipeline&                   _pipeline;
    std::vector<WipDrawable<T>>      _drawables;
};

}
