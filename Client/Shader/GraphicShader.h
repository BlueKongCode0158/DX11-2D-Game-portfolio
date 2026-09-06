#pragma once
#include "Shader.h"
#include <vector>

class GraphicShader : public Shader
{
public:
	GraphicShader();
	~GraphicShader();

public:	
	// Shader을(를) 통해 상속됨
	void Destroy() override;
	bool Init() override;
	void SetShader() final;
public:
	void AddInputLayoutDesc(const char* semetic, uint32 sementicIndex, DXGI_FORMAT fmt,
		uint32 inputSlot, uint32 size, D3D11_INPUT_CLASSIFICATION inputSlotClass, uint32 instanceDatastepRate);
	bool CreateInputLayout();
	bool LoadVertexShader(const char* entryName, const char* fileName);
	bool LoadPixelShader(const char* entryName, const char* fileName);

private:
	ComPtr<ID3D11InputLayout> _inputLayout = nullptr;
	std::vector<D3D11_INPUT_ELEMENT_DESC> _descs;
	uint32 _inputLayoutSize = 0;

	ComPtr<ID3D11VertexShader> _vs = nullptr;	// 버텍스 쉐이더 객체 
	ComPtr<ID3DBlob> _vsBlob = nullptr;			// 쉐이더 컴파일 후 나온 바이트 파일 저장

	ComPtr<ID3D11PixelShader> _ps = nullptr;	// 픽셀 쉐이더 객체
	ComPtr<ID3DBlob> _psBlob = nullptr;			// 픽셀 쉐이더 컴파일 후 나온 바이트 파일 저장
};

