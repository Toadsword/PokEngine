#include <GraphicsEngine/Models/material_export_data.h>

namespace poke {
namespace graphics {
MaterialExportData::MaterialExportData(const float f)
    : type_(Type::FLOAT),
      float_(f) { }

MaterialExportData::MaterialExportData(const int i)
    : type_(Type::INT),
      integer_(i) { }

MaterialExportData::MaterialExportData(const Image2d* image2d)
    : type_(Type::IMAGE_2D),
      image2d_(image2d) { }

MaterialExportData::MaterialExportData(
    const ImageCube* imageCube)
    : type_(Type::IMAGE_CUBE),
      imageCube_(imageCube) { }

MaterialExportData::MaterialExportData(const Color color)
    : type_(Type::COLOR),
      color_(color) { }

const void* MaterialExportData::operator&() const
{
    switch (type_) {
    case Type::FLOAT:
        return &float_;
    case Type::INT:
        return &integer_;
    case Type::IMAGE_2D:
        return image2d_;
    case Type::IMAGE_CUBE:
        return imageCube_;
    case Type::COLOR:
        return &color_;
    default: ;
        return nullptr;
    }
}
} //namespace graphics
} //namespace poke
