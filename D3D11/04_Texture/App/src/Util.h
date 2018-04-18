#pragma once


// ユーティリティ


template<class T>
using ComPtr = Microsoft::WRL::ComPtr<T>;



class Scalar
{
public:
    inline static float DegToRad(float value)
    {
        return value * 0.01745329251994329576923690768489f;
    }

    inline static float RadToDeg(float value)
    {
        return value * 57.295779513082320876798154814105f;
    }

    inline static float ClampRad(float value)
    {
        uint32_t ofs = (*(uint32_t*)&value & 0x80000000) | 0x3F000000;
        return value - ((int32_t)(value * 0.159154943f + *(float*)&ofs) * PI2);
    }

    inline static float Clamp(float value, float min, float max)
    {
        if (value<min)return min;
        if (value>max)return max;
        return value;
    }

    inline static float Min(float value1, float value2)
    {
        return value1 > value2 ? value2 : value1;
    }

    inline static float Max(float value1, float value2)
    {
        return value1 < value2 ? value2 : value1;
    }

    inline static float Saturate(float value)
    {
        if (value > 1.0f) return 1.0f;
        if (value < 0.0f) return 0.0f;
        return value;
    }

    inline static float Lerp(float value1, float value2, float t)
    {
        return value1 + (value2 - value1) * t;
    }
    inline static bool NearEqual(float S1, float S2, float Epsilon)
    {
        float Delta = S1 - S2;
        return (fabsf(Delta) <= Epsilon);
    }
    inline static void SinCos(float* pSin, float* pCos, float fValue)
    {
        ThrowIfFalse(pSin != nullptr);
        ThrowIfFalse(pCos != nullptr);

        // Map Value to y in [-pi,pi], x = 2*pi*quotient + remainder.
        float quotient = DIV_PI2 * fValue;
        if (fValue >= 0.0f)
        {
            quotient = (float)((int)(quotient + 0.5f));
        }
        else
        {
            quotient = (float)((int)(quotient - 0.5f));
        }
        float y = fValue - PI2 * quotient;

        // Map y to [-pi/2,pi/2] with sin(y) = sin(Value).
        float sign;
        if (y > PIH)
        {
            y = PI - y;
            sign = -1.0f;
        }
        else if (y < -PIH)
        {
            y = -PI - y;
            sign = -1.0f;
        }
        else
        {
            sign = +1.0f;
        }

        float y2 = y * y;

        // 11-degree minimax approximation
        *pSin = (((((-2.3889859e-08f * y2 + 2.7525562e-06f) * y2 - 0.00019840874f) * y2 + 0.0083333310f) * y2 - 0.16666667f) * y2 + 1.0f) * y;

        // 10-degree minimax approximation
        float p = ((((-2.6051615e-07f * y2 + 2.4760495e-05f) * y2 - 0.0013888378f) * y2 + 0.041666638f) * y2 - 0.5f) * y2 + 1.0f;
        *pCos = sign * p;
    }
    inline static void SinCos(float* pSin, float* pCos, float fValue, float fScale)
    {
        Scalar::SinCos(pSin, pCos, fValue);
        *pSin *= fScale;
        *pCos *= fScale;
    }
    static constexpr float PIH = 1.5707963267948966192313216916398f;
    static constexpr float PI = 3.1415926535897932384626433832795f;
    static constexpr float PI2 = 6.2831853071795864769252867665590f;
    static constexpr float DIV_PI = 0.31830988618379067153776752674503f;
    static constexpr float DIV_PI2 = 0.15915494309189533576888376337251f;
};


class MatrixUtil
{
public:
    //! 平衡投影行列を設定
    inline static Eigen::Matrix4f& SetOrthographicMatrix(
        Eigen::Matrix4f& dst,
        float fViewWidth,
        float fViewHeight,
        float fNearZ,
        float fFarZ)
    {
        ThrowIfFalse(!Scalar::NearEqual(fViewWidth, 0.0f, 0.00001f));
        ThrowIfFalse(!Scalar::NearEqual(fViewHeight, 0.0f, 0.00001f));
        ThrowIfFalse(!Scalar::NearEqual(fFarZ, fNearZ, 0.00001f));

        float fRange = 1.0f / (fFarZ - fNearZ);

        dst <<
            2.0f / fViewWidth, 0.0f, 0.0f, 0.0f,
            0.0f, 2.0f / fViewHeight, 0.0f, 0.0f,
            0.0f, 0.0f, fRange, -fRange * fNearZ,
            0.0f, 0.0f, 0.0f, 1.0f;
        return dst;
    }
    inline static Eigen::Matrix4f OrthographicMatrix(
        float fViewWidth,
        float fViewHeight,
        float fNearZ,
        float fFarZ)
    {
        Eigen::Matrix4f m;
        return SetOrthographicMatrix(m, fViewWidth, fViewHeight, fNearZ, fFarZ);
    }

    //! 透視投影行列を生成 左手座標系
    static inline Eigen::Matrix4f& SetPerspectiveMatrix(
        Eigen::Matrix4f& dst,
        float fFovAngleY,
        float fAspectRatio,
        float fNearZ,
        float fFarZ)
    {
        ThrowIfFalse(fNearZ > 0.f && fFarZ > 0.f);
        ThrowIfFalse(!Scalar::NearEqual(fFovAngleY, 0.0f, 0.00001f * 2.0f));
        ThrowIfFalse(!Scalar::NearEqual(fAspectRatio, 0.0f, 0.00001f));
        ThrowIfFalse(!Scalar::NearEqual(fFarZ, fNearZ, 0.00001f));

        float    fSinFov;
        float    fCosFov;
        Scalar::SinCos(&fSinFov, &fCosFov, 0.5f * fFovAngleY);

        float fHeight = fCosFov / fSinFov;
        float fWidth = fHeight / fAspectRatio;
        float fRange = fFarZ / (fFarZ - fNearZ);

        dst <<
            -fWidth, 0.0f, 0.0f, 0.0f,
            0.0f, fHeight, 0.0f, 0.0f,
            0.0f, 0.0f, fRange, -fRange * fNearZ,
            0.0f, 0.0f, 1.0f, 0.0f;
        return dst;
    }
    static inline Eigen::Matrix4f PerspectiveMatrix(
        float fFovAngleY,
        float fAspectRatio,
        float fNearZ,
        float fFarZ)
    {
        Eigen::Matrix4f m;
        return SetPerspectiveMatrix(m, fFovAngleY, fAspectRatio, fNearZ, fFarZ);
    }

    //! 対象へ向く行列を生成
    static inline Eigen::Matrix4f& SetLookAtMatrix(
        Eigen::Matrix4f& dst,
        const Eigen::Vector3f& eye,
        const Eigen::Vector3f& target,
        const Eigen::Vector3f& up)
    {
        Eigen::Vector3f f = (target - eye).normalized();
        Eigen::Vector3f u = up.normalized();
        Eigen::Vector3f s = u.cross(f).normalized();
        u = f.cross(s);

        dst <<
            s.x(), s.y(), s.z(), -s.dot(eye),
            u.x(), u.y(), u.z(), -u.dot(eye),
            f.x(), f.y(), f.z(), -f.dot(eye),
            0.0f, 0.0f, 0.0f, 1.0f;
        return dst;
    }
    static inline Eigen::Matrix4f LookAtMatrix(
        const Eigen::Vector3f& eye,
        const Eigen::Vector3f& target,
        const Eigen::Vector3f& up)
    {
        Eigen::Matrix4f m;
        return SetLookAtMatrix(m, eye, target, up);
    }
    static inline Eigen::Affine3f& SetLookAtMatrix(
        Eigen::Affine3f& dst,
        const Eigen::Vector3f& eye,
        const Eigen::Vector3f& target,
        const Eigen::Vector3f& up)
    {
        Eigen::Matrix4f m;
        SetLookAtMatrix(m, eye, target, up);
        dst = m;
        return dst;
    }

    //! 対象へ向く転置行列を生成
    static inline Eigen::Matrix4f& SetLookAtMatrixTranspose(
        Eigen::Matrix4f& dst,
        const Eigen::Vector3f& dir,
        const Eigen::Vector3f& up)
    {
        Eigen::Vector3f f = dir.normalized();
        Eigen::Vector3f u = up.normalized();
        Eigen::Vector3f s = u.cross(f).normalized();
        u = f.cross(s);

        dst <<
            s.x(), u.x(), f.x(), 0.0f,
            s.y(), u.y(), f.y(), 0.0f,
            s.z(), u.z(), f.z(), 0.0f,
            0, 0, 0, 1.0f;

        return dst;
    }
    static inline Eigen::Matrix4f LookAtMatrixTranspose(
        const Eigen::Vector3f& dir,
        const Eigen::Vector3f& up)
    {
        Eigen::Matrix4f m;
        return SetLookAtMatrixTranspose(m, dir, up);
    }
    static inline Eigen::Affine3f& SetLookAtMatrixTranspose(
        Eigen::Affine3f& dst,
        const Eigen::Vector3f& dir,
        const Eigen::Vector3f& up)
    {
        Eigen::Matrix4f m;
        SetLookAtMatrixTranspose(m, dir, up);
        dst = m;
        return dst;
    }

    //! 回転行列を設定
    inline static Eigen::Affine3f& SetRotateMatrixY(
        Eigen::Affine3f& dst,
        float fRot)
    {
        float c, s;
        Scalar::SinCos(&s, &c, fRot);
        Eigen::Matrix4f a;
        a <<
            c, 0.0f, -s, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            s, 0.0f, c, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f;
        dst = Eigen::Affine3f(a);
        return dst;
    }
    inline static Eigen::Affine3f RotateMatrixY(
        float fRot)
    {
        Eigen::Affine3f m;
        return SetRotateMatrixY(m, fRot);
    }
    inline static Eigen::Affine3f& SetRotateMatrixZ(
        Eigen::Affine3f& dst,
        float fRot)
    {
        float c, s;
        Scalar::SinCos(&s, &c, fRot);
        Eigen::Matrix4f a;
        a <<
            c, -s, 0.0f, 0.0f,
            s, c, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f;
        dst = Eigen::Affine3f(a);
        return dst;
    }
    inline static Eigen::Affine3f RotateMatrixZ(
        float fRot)
    {
        Eigen::Affine3f m;
        return SetRotateMatrixZ(m, fRot);
    }
};


//! DirectX ピクセルフォーマットからビット数を返す
inline UINT DXGIFormatBitsPerPixel(DXGI_FORMAT fmt)
{
    switch (static_cast<int>(fmt))
    {
    case DXGI_FORMAT_R32G32B32A32_TYPELESS:
    case DXGI_FORMAT_R32G32B32A32_FLOAT:
    case DXGI_FORMAT_R32G32B32A32_UINT:
    case DXGI_FORMAT_R32G32B32A32_SINT:
        return 128;

    case DXGI_FORMAT_R32G32B32_TYPELESS:
    case DXGI_FORMAT_R32G32B32_FLOAT:
    case DXGI_FORMAT_R32G32B32_UINT:
    case DXGI_FORMAT_R32G32B32_SINT:
        return 96;

    case DXGI_FORMAT_R16G16B16A16_TYPELESS:
    case DXGI_FORMAT_R16G16B16A16_FLOAT:
    case DXGI_FORMAT_R16G16B16A16_UNORM:
    case DXGI_FORMAT_R16G16B16A16_UINT:
    case DXGI_FORMAT_R16G16B16A16_SNORM:
    case DXGI_FORMAT_R16G16B16A16_SINT:
    case DXGI_FORMAT_R32G32_TYPELESS:
    case DXGI_FORMAT_R32G32_FLOAT:
    case DXGI_FORMAT_R32G32_UINT:
    case DXGI_FORMAT_R32G32_SINT:
    case DXGI_FORMAT_R32G8X24_TYPELESS:
    case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
    case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
    case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
    case DXGI_FORMAT_Y416:
    case DXGI_FORMAT_Y210:
    case DXGI_FORMAT_Y216:
        return 64;

    case DXGI_FORMAT_R10G10B10A2_TYPELESS:
    case DXGI_FORMAT_R10G10B10A2_UNORM:
    case DXGI_FORMAT_R10G10B10A2_UINT:
    case DXGI_FORMAT_R11G11B10_FLOAT:
    case DXGI_FORMAT_R8G8B8A8_TYPELESS:
    case DXGI_FORMAT_R8G8B8A8_UNORM:
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
    case DXGI_FORMAT_R8G8B8A8_UINT:
    case DXGI_FORMAT_R8G8B8A8_SNORM:
    case DXGI_FORMAT_R8G8B8A8_SINT:
    case DXGI_FORMAT_R16G16_TYPELESS:
    case DXGI_FORMAT_R16G16_FLOAT:
    case DXGI_FORMAT_R16G16_UNORM:
    case DXGI_FORMAT_R16G16_UINT:
    case DXGI_FORMAT_R16G16_SNORM:
    case DXGI_FORMAT_R16G16_SINT:
    case DXGI_FORMAT_R32_TYPELESS:
    case DXGI_FORMAT_D32_FLOAT:
    case DXGI_FORMAT_R32_FLOAT:
    case DXGI_FORMAT_R32_UINT:
    case DXGI_FORMAT_R32_SINT:
    case DXGI_FORMAT_R24G8_TYPELESS:
    case DXGI_FORMAT_D24_UNORM_S8_UINT:
    case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
    case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
    case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
    case DXGI_FORMAT_R8G8_B8G8_UNORM:
    case DXGI_FORMAT_G8R8_G8B8_UNORM:
    case DXGI_FORMAT_B8G8R8A8_UNORM:
    case DXGI_FORMAT_B8G8R8X8_UNORM:
    case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
    case DXGI_FORMAT_B8G8R8A8_TYPELESS:
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
    case DXGI_FORMAT_B8G8R8X8_TYPELESS:
    case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
    case DXGI_FORMAT_AYUV:
    case DXGI_FORMAT_Y410:
    case DXGI_FORMAT_YUY2:
        return 32;

    case DXGI_FORMAT_P010:
    case DXGI_FORMAT_P016:
        return 24;

    case DXGI_FORMAT_R8G8_TYPELESS:
    case DXGI_FORMAT_R8G8_UNORM:
    case DXGI_FORMAT_R8G8_UINT:
    case DXGI_FORMAT_R8G8_SNORM:
    case DXGI_FORMAT_R8G8_SINT:
    case DXGI_FORMAT_R16_TYPELESS:
    case DXGI_FORMAT_R16_FLOAT:
    case DXGI_FORMAT_D16_UNORM:
    case DXGI_FORMAT_R16_UNORM:
    case DXGI_FORMAT_R16_UINT:
    case DXGI_FORMAT_R16_SNORM:
    case DXGI_FORMAT_R16_SINT:
    case DXGI_FORMAT_B5G6R5_UNORM:
    case DXGI_FORMAT_B5G5R5A1_UNORM:
    case DXGI_FORMAT_A8P8:
    case DXGI_FORMAT_B4G4R4A4_UNORM:
        return 16;

    case DXGI_FORMAT_NV12:
    case DXGI_FORMAT_420_OPAQUE:
    case DXGI_FORMAT_NV11:
        return 12;

    case DXGI_FORMAT_R8_TYPELESS:
    case DXGI_FORMAT_R8_UNORM:
    case DXGI_FORMAT_R8_UINT:
    case DXGI_FORMAT_R8_SNORM:
    case DXGI_FORMAT_R8_SINT:
    case DXGI_FORMAT_A8_UNORM:
    case DXGI_FORMAT_AI44:
    case DXGI_FORMAT_IA44:
    case DXGI_FORMAT_P8:
        return 8;

    case DXGI_FORMAT_R1_UNORM:
        return 1;

    case DXGI_FORMAT_BC1_TYPELESS:
    case DXGI_FORMAT_BC1_UNORM:
    case DXGI_FORMAT_BC1_UNORM_SRGB:
    case DXGI_FORMAT_BC4_TYPELESS:
    case DXGI_FORMAT_BC4_UNORM:
    case DXGI_FORMAT_BC4_SNORM:
        return 4;

    case DXGI_FORMAT_BC2_TYPELESS:
    case DXGI_FORMAT_BC2_UNORM:
    case DXGI_FORMAT_BC2_UNORM_SRGB:
    case DXGI_FORMAT_BC3_TYPELESS:
    case DXGI_FORMAT_BC3_UNORM:
    case DXGI_FORMAT_BC3_UNORM_SRGB:
    case DXGI_FORMAT_BC5_TYPELESS:
    case DXGI_FORMAT_BC5_UNORM:
    case DXGI_FORMAT_BC5_SNORM:
    case DXGI_FORMAT_BC6H_TYPELESS:
    case DXGI_FORMAT_BC6H_UF16:
    case DXGI_FORMAT_BC6H_SF16:
    case DXGI_FORMAT_BC7_TYPELESS:
    case DXGI_FORMAT_BC7_UNORM:
    case DXGI_FORMAT_BC7_UNORM_SRGB:
        return 8;

    default:
        return 0;
    }
}

//! DirectX ピクセルフォーマットからSysMemPitchを返す
inline UINT DXGIFormatSystemPitch(DXGI_FORMAT fmt, int iWidth)
{
    switch (fmt)
    {
    case DXGI_FORMAT_BC1_TYPELESS:
    case DXGI_FORMAT_BC1_UNORM:
    case DXGI_FORMAT_BC1_UNORM_SRGB:
    case DXGI_FORMAT_BC4_TYPELESS:
    case DXGI_FORMAT_BC4_UNORM:
    case DXGI_FORMAT_BC4_SNORM:
    case DXGI_FORMAT_BC2_TYPELESS:
    case DXGI_FORMAT_BC2_UNORM:
    case DXGI_FORMAT_BC2_UNORM_SRGB:
    case DXGI_FORMAT_BC3_TYPELESS:
    case DXGI_FORMAT_BC3_UNORM:
    case DXGI_FORMAT_BC3_UNORM_SRGB:
    case DXGI_FORMAT_BC5_TYPELESS:
    case DXGI_FORMAT_BC5_UNORM:
    case DXGI_FORMAT_BC5_SNORM:
    case DXGI_FORMAT_BC6H_TYPELESS:
    case DXGI_FORMAT_BC6H_UF16:
    case DXGI_FORMAT_BC6H_SF16:
    case DXGI_FORMAT_BC7_TYPELESS:
    case DXGI_FORMAT_BC7_UNORM:
    case DXGI_FORMAT_BC7_UNORM_SRGB:
        return static_cast<UINT>(DXGIFormatBitsPerPixel(fmt) * 2 * (iWidth / 4));
    default:
        return static_cast<UINT>(DXGIFormatBitsPerPixel(fmt) / 8 * iWidth);
    }
}

