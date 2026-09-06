#pragma once
#include "Common/Info.h"
#include "Common/Singleton.h"
#include "Common/Vector2D.h"

struct FVertex
{
    FVector3D _position;
    FVector4D _color;
};

class Device : public Singleton<Device>
{
    friend Singleton<Device>;

protected:
    Device() = default;
    ~Device() = default;
    Device(const Device&) = delete;
    Device(Device&&) = delete;
    Device& operator=(const Device&) = delete;
    Device& operator=(Device&&) = delete;

private:
    ComPtr<ID3D11Device> _device = nullptr;
    ComPtr<ID3D11DeviceContext> _context = nullptr;
    ComPtr<IDXGISwapChain> _swapChain = nullptr;
    ComPtr<ID3D11RenderTargetView> _targetView = nullptr;
    ComPtr<ID3D11Texture2D> _backbufferTexture;     // 최종 화면에 보이는 swapchain backbuffer
    D3D11_VIEWPORT _viewport = { 0 };
    ComPtr<ID3D11DepthStencilView> _depthStencil = nullptr;
    HWND _hWnd = nullptr;
    FResolution _resolution = {};
    bool _windowMode = false;

    ComPtr<ID2D1RenderTarget> _target2D;
    ComPtr<ID2D1Factory> _factory2D;
private:
    ComPtr<ID3D11Texture2D>             _sceneTexture;          // 게임 월드를 그리는 오프스크린 텍스쳐
    ComPtr<ID3D11RenderTargetView>      _sceneRTV;
    ComPtr<ID3D11ShaderResourceView>    _sceneSRV;
private:
    ComPtr<ID3D11Texture2D>             _grabTexture;
    ComPtr<ID3D11ShaderResourceView>    _grabSRV;
private:
    ComPtr<ID3D11Texture2D>             _activeRenderTexture;
public:
    ComPtr<ID2D1RenderTarget> GetTarget2D() const { return _target2D; }
    ComPtr<ID3D11Device> GetDevice() const { return _device; }
    ComPtr<ID3D11DeviceContext> GetContext() const { return _context; }
    ComPtr<IDXGISwapChain> GetSwapChain() const { return _swapChain; }
    ComPtr<ID3D11RenderTargetView> GetTargetView() const { return _targetView; }
    ComPtr<ID3D11DepthStencilView> GetDepthStencil() const { return _depthStencil; }
    ComPtr<ID3D11ShaderResourceView> GetSceneSRV() const;
    ComPtr<ID3D11ShaderResourceView> GetGrabSRV() const;

    bool GetWindowMode() { return _windowMode; }
    const FResolution& GetRS() const { return _resolution; }
    FVector2D GetRSRate() const;
    D3D11_TEXTURE2D_DESC GetBackBufferDesc() const;
public:
    virtual void Destroy() override;
    bool Init(HWND hwnd, uint32 width, uint32 height, bool windowMode);
    void ClearSceneTarget(float clearColor[4]);
    void ClearBackBuffer(float clearColor[4]);
    void ClearDepthStencil(float depth, uint8 stencil);
    void SetSceneTarget();
    void SetBackBufferTarget();
    void SetActiveRenderSource(ComPtr<ID3D11Texture2D> texture);
public:
    void PresentScene();
    void Present();
private:
    void CreateGrabRT();
public:
    void GrabPass();
};

#define DEVICE Device::Instance().GetDevice()
#define CONTEXT Device::Instance().GetContext()