#include "pch.h"
#include "TransformCBuffer.h"

TransformCBuffer::TransformCBuffer()
{
}

TransformCBuffer::~TransformCBuffer()
{
}

void TransformCBuffer::SetWorldMatrix(const FMatrix& mat)
{
	_data._world = mat;
}

void TransformCBuffer::SetViewMatrix(const FMatrix& mat)
{
	_data._view = mat;
}

void TransformCBuffer::SetProjMatrix(const FMatrix& mat)
{
	_data._proj = mat;
}


void TransformCBuffer::Update()
{
	_data._wv = _data._world * _data._view;
	_data._wvp = _data._wv * _data._proj;

	_data._world.Transpose();
	_data._view.Transpose();
	_data._proj.Transpose();
	_data._wv.Transpose();
	_data._wvp.Transpose();

	SetData(&_data);
}