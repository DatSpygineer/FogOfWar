#include "fow/Shared/MathHelper.hpp"

namespace fow {
#if !FOW_CONSTEXPR_ENABLED
    const Vector2 Vector2Constants::Up    = { 0.0f, 1.0f };
    const Vector2 Vector2Constants::Down  = { 0.0f, -1.0f };
    const Vector2 Vector2Constants::Left  = { -1.0f, 0.0f };
    const Vector2 Vector2Constants::Right = { 1.0f, 0.0f };
    const Vector2 Vector2Constants::Zero  = { 0.0f, 0.0f };
    const Vector2 Vector2Constants::One   = { 1.0f, 1.0f };

    const Vector2 Vector2Constants::UnitX = { 1.0f, 0.0f };
    const Vector2 Vector2Constants::UnitY = { 0.0f, 1.0f };

    const Vector3 Vector3Constants::Up       = { 0.0f, 1.0f, 0.0f };
    const Vector3 Vector3Constants::Down     = { 0.0f, -1.0f, 0.0f };
    const Vector3 Vector3Constants::Forward  = { 0.0f, 0.0f, -1.0f };
    const Vector3 Vector3Constants::Backward = { 0.0f, 0.0f, 1.0f };
    const Vector3 Vector3Constants::Right    = { 1.0f, 0.0f, 0.0f };
    const Vector3 Vector3Constants::Left     = { -1.0f, 0.0f, 0.0f };
    const Vector3 Vector3Constants::Zero     = { 0.0f, 0.0f, 0.0f };
    const Vector3 Vector3Constants::One      = { 1.0f, 1.0f, 1.0f };

    const Vector3 Vector3Constants::UnitX   = { 1.0f, 0.0f, 0.0f };
    const Vector3 Vector3Constants::UnitY   = { 0.0f, 1.0f, 0.0f };
    const Vector3 Vector3Constants::UnitZ   = { 0.0f, 0.0f, 1.0f };

    const Vector4 Vector4Constants::Zero = { 0.0f, 0.0f, 0.0f, 0.0f };
    const Vector4 Vector4Constants::One  = { 1.0f, 1.0f, 1.0f, 1.0f };

    const Vector4 Vector4Constants::UnitX = { 1.0f, 0.0f, 0.0f, 0.0f };
    const Vector4 Vector4Constants::UnitY = { 0.0f, 1.0f, 0.0f, 0.0f };
    const Vector4 Vector4Constants::UnitZ = { 0.0f, 0.0f, 1.0f, 0.0f };
    const Vector4 Vector4Constants::UnitW = { 0.0f, 0.0f, 0.0f, 1.0f };

    const Quat QuatConstants::Identity = { 1.0f, 0.0f, 0.0f, 0.0f };

    const Matrix4 Matrix4Constants::Identity = { 1.0f };

    const Color ColorConstants::White       = { 1.0f, 1.0f, 1.0f, 1.0f };
    const Color ColorConstants::Black       = { 0.0f, 0.0f, 0.0f, 1.0f };
    const Color ColorConstants::Red         = { 1.0f, 0.0f, 0.0f, 1.0f };
    const Color ColorConstants::Green       = { 0.0f, 1.0f, 0.0f, 1.0f };
    const Color ColorConstants::Blue        = { 0.0f, 0.0f, 1.0f, 1.0f };
    const Color ColorConstants::Yellow      = { 1.0f, 1.0f, 0.0f, 1.0f };
    const Color ColorConstants::Cyan        = { 0.0f, 1.0f, 1.0f, 1.0f };
    const Color ColorConstants::Magenta     = { 1.0f, 0.0f, 1.0f, 1.0f };
    const Color ColorConstants::Transparent = { 0.0f, 0.0f, 0.0f, 0.0f };

    const Color ColorConstants::Gray        = { 0.5f, 0.5f, 0.5f, 1.0f };
    const Color ColorConstants::Grey        = { 0.5f, 0.5f, 0.5f, 1.0f };
    const Color ColorConstants::DarkGray    = { 0.25f, 0.25f, 0.25f, 1.0f };
    const Color ColorConstants::DarkGrey    = { 0.25f, 0.25f, 0.25f, 1.0f };
    const Color ColorConstants::LightGray   = { 0.75f, 0.75f, 0.75f, 1.0f };
    const Color ColorConstants::LightGrey   = { 0.75f, 0.75f, 0.75f, 1.0f };

    const Color ColorConstants::NormalMap   = { 0.5f, 0.5f, 1.0f, 1.0f };
#endif
}