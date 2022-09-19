#pragma once
#include "../Core/Mathf/Mathf.h"
#include "../Graphics/GraphicsCore.h"

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

class DistRenderPipeline: public GraphicsCore
{
public:
	DistRenderPipeline(HINSTANCE hInstance);
	DistRenderPipeline(const DistRenderPipeline& rhs) = delete;
	DistRenderPipeline& operator=(const DistRenderPipeline& rhs) = delete;
	~DistRenderPipeline();

public:
	void Init();

	void Render();

	void Resize();

private:

	//	渲染阴影
	void RenderShadowPass();

	//	提前深度
	void DepthPrePass();

	//	渲染GBuffer
	void RenderGBuffer();

	//	渲染不透明物体
	void RenderOpaquePass();

	//	延迟渲染
	void DeferredRenderPass();

	//	渲染天空球
	void RenderSkyBoxPass();

	//	渲染透明物体
	void RenderTransparentPass();

	//	渲染坐标轴
	void RenderAxisPass();

	//	渲染图标
	void RenderGizmosPass();

	//	渲染后处理
	void RenderPostProcessPass();

	//	渲染体积雾
	void DrawVolumeFog();

	//	渲染快速近似抗锯齿
	void DrawFxAA();


	//	RenderUtils

	//	材质Buffer
	ID3D12Resource* mMatBuffer = nullptr;

	//	设置描述符堆
	void SetDescriptorHeap();

	//	设置根签名
	void SetRootSignature();

	//	设置材质Buffer
	void SetMatBuffer(MatBufferType type);

	//	拷贝Pass
	void CopyColorPass();

	//	设置ViewheRect
	void SetViewAndRect();

	//	资源屏障 - 展示到目标
	void SetPresentToTarget(ID3D12Resource* Resource);

	//	资源屏障 - 设置目标到展示
	void SetTargetToPresnet(ID3D12Resource* Resource);

	//	资源屏障 - 设置读取到目标
	void SetReadToDest(ID3D12Resource* Resource);

	//	资源屏障 - 设置展示到源
	void SetPresentToSource(ID3D12Resource* Resource);

	//	资源屏障 - 设置源到展示
	void SetSourceToPresent(ID3D12Resource* Resource);

	//	资源屏障 - 设置目标到读取
	void SetDestToRead(ID3D12Resource* Resource);

	//	拷贝复制
	void CopyBlit(ID3D12Resource* dest, ID3D12Resource* source);

	//	设置为渲染目标
	void SetRenderTarget();

	//	设置GBuffer的渲染目标
	void SetGBufferTarget();

	//	清除GBuffer的渲染目标
	void ClearGBufferTarget();

	//	设置CBuffer
	void SetCBuffer();

	//	设置环境贴图到跟描述符表
	void SetCubeMapRootDescriptorTable();

	//	绘制渲染项目 Layer
	void DrawRenderItemFormLayer(std::string PSO, int Layer);
};