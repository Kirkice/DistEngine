#pragma once
#include "../Core/Mathf/Mathf.h"
#include "../Graphics/GraphicsCore.h"

namespace DistRender
{
	using Microsoft::WRL::ComPtr;
	using namespace DirectX;
	using namespace DirectX::PackedVector;

	//Render Layer
	enum class MatBufferType : int
	{
		PBR = 0,
		SkyBox = 1,
		PostProcess = 2,
	};

	class DistRenderPipeline : public GraphicsCore
	{
	public:
	DistRenderPipeline(HINSTANCE hInstance);
	DistRenderPipeline(const DistRenderPipeline& rhs) = delete;
	DistRenderPipeline& operator=(const DistRenderPipeline& rhs) = delete;
	~DistRenderPipeline();

public:

	void Render();

private:

	//	��Ⱦ��Ӱ
	void RenderShadowPass();

	//	��ǰ���
	void DepthPrePass();

	//	��ȾGBuffer
	void RenderGBuffer();

	//	��Ⱦ��͸������
	void RenderOpaquePass();

	//	�ӳ���Ⱦ
	void DeferredRenderPass();

	//	��Ⱦ�����
	void RenderSkyBoxPass();

	//	��Ⱦ͸������
	void RenderTransparentPass();

	//	��Ⱦ������
	void RenderAxisPass();

	//	��Ⱦͼ��
	void RenderGizmosPass();

	//	��Ⱦ����
	void RenderPostProcessPass();

	//	��Ⱦ�����
	void DrawVolumeFog();

	//	��Ⱦ���ٽ��ƿ����
	void DrawFxAA();


	//	RenderUtils

	//	����Buffer
	ID3D12Resource* mMatBuffer = nullptr;

	//	������������
	void SetDescriptorHeap();

	//	���ø�ǩ��
	void SetRootSignature();

	//	���ò���Buffer
	void SetMatBuffer(MatBufferType type);

	//	����Pass
	void CopyColorPass();

	//	����ViewheRect
	void SetViewAndRect();

	//	��Դ���� - չʾ��Ŀ��
	void SetPresentToTarget(ID3D12Resource* Resource);

	//	��Դ���� - ����Ŀ�굽չʾ
	void SetTargetToPresnet(ID3D12Resource* Resource);

	//	��Դ���� - ���ö�ȡ��Ŀ��
	void SetReadToDest(ID3D12Resource* Resource);

	//	��Դ���� - ����չʾ��Դ
	void SetPresentToSource(ID3D12Resource* Resource);

	//	��Դ���� - ����Դ��չʾ
	void SetSourceToPresent(ID3D12Resource* Resource);

	//	��Դ���� - ����Ŀ�굽��ȡ
	void SetDestToRead(ID3D12Resource* Resource);

	//	��������
	void CopyBlit(ID3D12Resource* dest, ID3D12Resource* source);

	//	����Ϊ��ȾĿ��
	void SetRenderTarget();

	//	����GBuffer����ȾĿ��
	void SetGBufferTarget();

	//	���GBuffer����ȾĿ��
	void ClearGBufferTarget();

	//	����CBuffer
	void SetCBuffer();

	//	���û�����ͼ������������
	void SetCubeMapRootDescriptorTable();

	//	������Ⱦ��Ŀ Layer
	void DrawRenderItemFormLayer(std::string PSO, int Layer);
	};
}