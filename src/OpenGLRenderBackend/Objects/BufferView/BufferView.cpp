#include "BufferView.h"
#include "../Buffer/Buffer.h"
#include "../Device/Device.h"
#include <stdexcept>

namespace OpenGL
{
    BufferView::BufferView(Device* _parent, const Render::BufferViewInfo& info)
        : parent(_parent)
    {
        GLHandle _handle;
        parent->GetLoader().GenTextures(1, &_handle);
        if(_handle == OGL_NULL_HANDLE)
            throw std::runtime_error("Failed to createe image view");

        parent->GetLoader().TextureBufferRange(_handle,
                                               *FormatToNative(info.format),
                                               static_cast<Buffer*>(info.buffer)->GetHandle(),
                                               info.offset,
                                               info.size);

        handle = _handle;
    }

    BufferView::~BufferView()
    {
        parent->GetLoader().DeleteTextures(1, &handle);
    }

    Render::Device* BufferView::GetParent() const noexcept
    {
        return parent;
    }

    GLHandle BufferView::GetHandle() const noexcept
    {
        return handle;
    }
};