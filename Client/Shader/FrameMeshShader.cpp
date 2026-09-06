#include "pch.h"
#include "FrameMeshShader.h"
#include "ShaderManager.h"

bool FrameMeshShader::Init()
{
	if (!LoadVertexShader("FrameMeshVS", "ColorMesh.fx"))
	{
		return false;
	}

	if (!LoadPixelShader("FrameMeshPS", "ColorMesh.fx"))
	{
		return false;
	}

	AddInputLayoutDesc("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0);
	
	if (!CreateInputLayout())
	{
		return false;
	}

	return true;
}
