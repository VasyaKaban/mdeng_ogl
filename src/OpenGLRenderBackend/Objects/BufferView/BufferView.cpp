#include "BufferView.h"
#include "../../Context/Context.h"
#include "../Buffer/Buffer.h"
#include <stdexcept>

namespace OpenGL
{
    BufferView::BufferView(Context* _parent, const Render::BufferViewInfo& info)
        : parent(_parent)
    {
        GLHandle _handle;
        parent->GetLoader().GenTextures(1, &_handle);
        if(_handle == OGL_NULL_HANDLE)
            throw std::runtime_error("Failed to createe image view");

        parent->GetLoader().TextureBufferRange(_handle,
                                               FormatToNative(info.format),
                                               static_cast<Buffer*>(info.buffer)->GetHandle(),
                                               info.offset,
                                               info.size);

        handle = _handle;
    }

    BufferView::~BufferView()
    {
        parent->GetLoader().DeleteTextures(1, &handle);
    }

    GLHandle BufferView::GetHandle() const noexcept
    {
        return handle;
    }

    Render::Context* BufferView::GetContext() const noexcept
    {
        return parent;
    }
};