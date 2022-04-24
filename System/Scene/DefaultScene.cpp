#include "DefaultScene.h"
#include "../../Component/DirectionLight.h"
#include "../EngineSystem/SystemUtils.h"
#include "../PhysicsSystem/PhysicsUtils.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

namespace Dist
{
	const int GizmoCount = 6;

	DefaultScene::DefaultScene()
	{
		mMainLight = DirectionLight();
		mCamera = Camera();
		mSkyBoxTexPath = L"";
	}

	DefaultScene::~DefaultScene()
	{

	}

	//	��ʼ������
	void DefaultScene::InitScene(Microsoft::WRL::ComPtr<ID3D12Device> device, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList)
	{
		//	���������ƹ�
		BuildLights();

		//	�������
		BuildCamera();

		//	������ǩ��
		BuildRootSignature(device);

		//	����SSAO��ǩ��
		BuildSsaoRootSignature(device);

		//	������ɫ��
		BuildShadersAndInputLayout();

		//	����MeshRender
		LoadMeshRender();

		//	������Ⱦ��Ŀ �Լ��޳�
		LoadRenderItem(device, mCommandList);
		
	}

	//	���³���
	void DefaultScene::UpdateScene()
	{

	}

	//	�����ƹ�
	void DefaultScene::BuildLights()
	{
		//	��Ϊ����Դ
		mMainLight.isMainLight = true;
		//	�ƹ���ɫ
		mMainLight.color = Color(1, 0.9568627f, 0.8392157f, 1);
		//	���õƹ�λ��
		mMainLight.position = Vector3(0, 3, 0);
		//	���õƹ�ŷ����
		mMainLight.eulerangle = Vector3(50, -30, 0);
		//	����Դ����
		mMainLight.name = "Direction Light";
	}

	//	�������
	void DefaultScene::BuildCamera()
	{
		//	�������
		mCamera.SetLens(60, 16 / 9, 0.3f, 0.6f);
		//	�������
		mCamera.position = Vector3(-4, 3, -4);
		//	���ŷ����
		mCamera.eulerangle = Vector3(20, 45, 0);
		//	�������
		mCamera.name = "Main Camera";
	}

	//	������ǩ��
	void DefaultScene::BuildRootSignature(Microsoft::WRL::ComPtr<ID3D12Device> device)
	{
		CD3DX12_DESCRIPTOR_RANGE texTable0;
		texTable0.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 5, 0, 0);

		CD3DX12_DESCRIPTOR_RANGE texTable1;
		texTable1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 60, 5, 0);

		// Root parameter can be a table, root descriptor or root constants.
		CD3DX12_ROOT_PARAMETER slotRootParameter[6];

		// Perfomance TIP: Order from most frequent to least frequent.
		slotRootParameter[0].InitAsConstantBufferView(0);
		slotRootParameter[1].InitAsConstantBufferView(1);
		slotRootParameter[2].InitAsConstantBufferView(2);
		slotRootParameter[3].InitAsShaderResourceView(0, 1);
		slotRootParameter[4].InitAsDescriptorTable(1, &texTable0, D3D12_SHADER_VISIBILITY_PIXEL);
		slotRootParameter[5].InitAsDescriptorTable(1, &texTable1, D3D12_SHADER_VISIBILITY_PIXEL);

		auto staticSamplers = SystemUtils::GetStaticSamplers();
		 
		// A root signature is an array of root parameters.
		CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(6, slotRootParameter,
			(UINT)staticSamplers.size(), staticSamplers.data(),
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		// create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
		ComPtr<ID3DBlob> serializedRootSig = nullptr;
		ComPtr<ID3DBlob> errorBlob = nullptr;
		HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
			serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

		if (errorBlob != nullptr)
		{
			::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
		}
		ThrowIfFailed(hr);

		ThrowIfFailed(device->CreateRootSignature(
			0,
			serializedRootSig->GetBufferPointer(),
			serializedRootSig->GetBufferSize(),
			IID_PPV_ARGS(mRootSignature.GetAddressOf())));
	}

	//	����SSAO��ǩ��
	void DefaultScene::BuildSsaoRootSignature(Microsoft::WRL::ComPtr<ID3D12Device> device)
	{
		CD3DX12_DESCRIPTOR_RANGE texTable0;
		texTable0.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 2, 0, 0);

		CD3DX12_DESCRIPTOR_RANGE texTable1;
		texTable1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2, 0);

		// Root parameter can be a table, root descriptor or root constants.
		CD3DX12_ROOT_PARAMETER slotRootParameter[4];

		// Perfomance TIP: Order from most frequent to least frequent.
		slotRootParameter[0].InitAsConstantBufferView(0);
		slotRootParameter[1].InitAsConstants(1, 1);
		slotRootParameter[2].InitAsDescriptorTable(1, &texTable0, D3D12_SHADER_VISIBILITY_PIXEL);
		slotRootParameter[3].InitAsDescriptorTable(1, &texTable1, D3D12_SHADER_VISIBILITY_PIXEL);

		const CD3DX12_STATIC_SAMPLER_DESC pointClamp(
			0, // shaderRegister
			D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

		const CD3DX12_STATIC_SAMPLER_DESC linearClamp(
			1, // shaderRegister
			D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

		const CD3DX12_STATIC_SAMPLER_DESC depthMapSam(
			2, // shaderRegister
			D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
			D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressU
			D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressV
			D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressW
			0.0f,
			0,
			D3D12_COMPARISON_FUNC_LESS_EQUAL,
			D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE);

		const CD3DX12_STATIC_SAMPLER_DESC linearWrap(
			3, // shaderRegister
			D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
			D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

		std::array<CD3DX12_STATIC_SAMPLER_DESC, 4> staticSamplers =
		{
			pointClamp, linearClamp, depthMapSam, linearWrap
		};

		// A root signature is an array of root parameters.
		CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(4, slotRootParameter,
			(UINT)staticSamplers.size(), staticSamplers.data(),
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		// create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
		ComPtr<ID3DBlob> serializedRootSig = nullptr;
		ComPtr<ID3DBlob> errorBlob = nullptr;
		HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
			serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

		if (errorBlob != nullptr)
		{
			::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
		}
		ThrowIfFailed(hr);

		ThrowIfFailed(device->CreateRootSignature(
			0,
			serializedRootSig->GetBufferPointer(),
			serializedRootSig->GetBufferSize(),
			IID_PPV_ARGS(mSsaoRootSignature.GetAddressOf())));
	}


	//	������ɫ��
	void DefaultScene::BuildShadersAndInputLayout()
	{
		//	PBR Lit Shader
		mShaders["litVS"] = SystemUtils::CompileShader(L"Shaders\\LitPass.hlsl", nullptr, "VS", "vs_5_1");
		mShaders["litPS"] = SystemUtils::CompileShader(L"Shaders\\LitPass.hlsl", nullptr, "PS", "ps_5_1");

		//	ͼ�� Shader
		mShaders["gizmoVS"] = SystemUtils::CompileShader(L"Shaders\\GizmoPass.hlsl", nullptr, "VS", "vs_5_1");
		mShaders["gizmoPS"] = SystemUtils::CompileShader(L"Shaders\\GizmoPass.hlsl", nullptr, "PS", "ps_5_1");

		//	��Χ�� Shader
		mShaders["boundingVS"] = SystemUtils::CompileShader(L"Shaders\\Bounding.hlsl", nullptr, "VS", "vs_5_1");
		mShaders["boundingPS"] = SystemUtils::CompileShader(L"Shaders\\Bounding.hlsl", nullptr, "PS", "ps_5_1");

		//	��Ӱ Shader
		mShaders["shadowVS"] = SystemUtils::CompileShader(L"Shaders\\Shadows.hlsl", nullptr, "VS", "vs_5_1");
		mShaders["shadowOpaquePS"] = SystemUtils::CompileShader(L"Shaders\\Shadows.hlsl", nullptr, "PS", "ps_5_1");

		//	Debug Shader
		mShaders["debugVS"] = SystemUtils::CompileShader(L"Shaders\\ShadowDebug.hlsl", nullptr, "VS", "vs_5_1");
		mShaders["debugPS"] = SystemUtils::CompileShader(L"Shaders\\ShadowDebug.hlsl", nullptr, "PS", "ps_5_1");

		//	Normal Shader
		mShaders["drawNormalsVS"] = SystemUtils::CompileShader(L"Shaders\\DrawNormals.hlsl", nullptr, "VS", "vs_5_1");
		mShaders["drawNormalsPS"] = SystemUtils::CompileShader(L"Shaders\\DrawNormals.hlsl", nullptr, "PS", "ps_5_1");

		//	SSAO Shader
		mShaders["ssaoVS"] = SystemUtils::CompileShader(L"Shaders\\Ssao.hlsl", nullptr, "VS", "vs_5_1");
		mShaders["ssaoPS"] = SystemUtils::CompileShader(L"Shaders\\Ssao.hlsl", nullptr, "PS", "ps_5_1");

		mShaders["ssaoBlurVS"] = SystemUtils::CompileShader(L"Shaders\\SsaoBlur.hlsl", nullptr, "VS", "vs_5_1");
		mShaders["ssaoBlurPS"] = SystemUtils::CompileShader(L"Shaders\\SsaoBlur.hlsl", nullptr, "PS", "ps_5_1");

		//	SkyShader
		mShaders["skyVS"] = SystemUtils::CompileShader(L"Shaders\\Sky.hlsl", nullptr, "VS", "vs_5_1");
		mShaders["skyPS"] = SystemUtils::CompileShader(L"Shaders\\Sky.hlsl", nullptr, "PS", "ps_5_1");

		mInputLayout =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};
	}

	//	���µƹ�
	void DefaultScene::UpdateLights(const GameTimer& gt)
	{

	}

	//	��������CB
	void DefaultScene::UpdateObjectCBs(const GameTimer& gt)
	{

	}

	//	���²���
	void DefaultScene::UpdateMaterials(const GameTimer& gt)
	{

	}

	//	������Ӱλ��
	void DefaultScene::UpdateShadowTransform(const GameTimer& gt)
	{

	}

	//	����Pass CB
	void DefaultScene::UpdateMainPassCB(const GameTimer& gt)
	{

	}

	//	����Shadow CB
	void DefaultScene::UpdateShadowPassCB(const GameTimer& gt)
	{

	}

	//	����SSAO CB
	void DefaultScene::UpdateSsaoCB(const GameTimer& gt)
	{

	}

	//	����ͼƬ
	std::vector<ComPtr<ID3D12Resource>>& DefaultScene::LoadTextures(TexturesType type)
	{
		std::vector<ComPtr<ID3D12Resource>> tex2DList;

		if (type == TexturesType::RenderItem)
		{
			tex2DList =
			{
				mTextures["defaultDiffuseMap"]->Resource,
				mTextures["defaultNormalMap"]->Resource,
				mTextures["defaultMsoMap"]->Resource,
				mTextures["defaultEmissionMap"]->Resource,
				mTextures["defaultLUTMap"]->Resource,

				mTextures["WoodenDiffuse"]->Resource,
				mTextures["WoodenNormal"]->Resource,

				mTextures["MetallicDiffuse"]->Resource,
				mTextures["MetallicNormal"]->Resource,

				mTextures["BricsDiffuse"]->Resource,
				mTextures["BricsNormal"]->Resource,

				mTextures["OakDiffuse"]->Resource,
				mTextures["OakNormal"]->Resource,

				mTextures["WoodenDiffuse2"]->Resource,
				mTextures["WoodenNormal2"]->Resource,

				mTextures["WoodenDiffuse3"]->Resource,
				mTextures["WoodenNormal3"]->Resource,

				mTextures["WoodenDiffuse4"]->Resource,
				mTextures["WoodenNormal4"]->Resource,
			};
		}
		else
		{
			tex2DList =
			{
				mTextures["AreaLightGizmo"]->Resource,
				mTextures["DirectionalLightGizmo"]->Resource,
				mTextures["ParticleSystemGizmo"]->Resource,
				mTextures["PointLightGizmo"]->Resource,
				mTextures["SpotLightGizmo"]->Resource,
				mTextures["WirePlane"]->Resource,
			};
		}

		return tex2DList;
	}

	//	����MeshRender
	void DefaultScene::LoadMeshRender()
	{
		//	ƽ���MeshRender

		auto plane = std::make_unique<PBRMeshRender>();
		plane->name = "plane_render";

		//��������
		plane->material.Name = "plane_mat";
		plane->material.MatCBIndex = 1;
		plane->material.DiffuseMapIndex = GizmoCount + 0;
		plane->material.NormalMapIndex = GizmoCount + 1;
		plane->material.MsoMapIndex = GizmoCount + 2;
		plane->material.EmissionMapIndex = GizmoCount + 3;
		plane->material.LUTMapIndex = GizmoCount + 4;
		plane->material.DiffuseColor = Color(1.0f, 1.0f, 1.0f, 1.0f);
		plane->material.Smoothness = 0.0f;
		plane->material.Metallic = 0.0f;
		plane->material.Occlusion = 1;
		plane->material.EmissionColor = Color(0.0f, 0.0f, 0.0f, 1.0f);
		plane->material.EmissionStrength = 0.0f;

		//	����ƽ������
		plane->mesh.CreateGrid(30.0f, 30.0f, 60, 40);

		//	��������
		plane->position = Vector3(0, 0, 0);
		plane->eulerangle = Vector3(0, 0, 0);
		plane->scale = Vector3(1, 1, 1);
		
		//	������ײ��
		plane->bound.aabb = BoundingAABB(plane->mesh.data);

		//	����AABB����������
		plane->bound.aabb.m_min = plane->GetWorldMatrix() * plane->bound.aabb.m_min;
		plane->bound.aabb.m_max = plane->GetWorldMatrix() * plane->bound.aabb.m_max;
		mMeshRender.push_back(std::move(plane));

		//	���MeshRender

		auto sphere = std::make_unique<PBRMeshRender>();
		sphere->name = "sphere_render";

		//��������
		sphere->material.Name = "Wooden2";
		sphere->material.MatCBIndex = 2;
		sphere->material.DiffuseMapIndex = GizmoCount + 13;
		sphere->material.NormalMapIndex = GizmoCount + 14;
		sphere->material.MsoMapIndex = GizmoCount + 2;
		sphere->material.EmissionMapIndex = GizmoCount + 3;
		sphere->material.LUTMapIndex = GizmoCount + 4;
		sphere->material.DiffuseColor = Color(1.0f, 1.0f, 1.0f, 1.0f);
		sphere->material.Smoothness = 0.15f;
		sphere->material.Metallic = 0.0f;
		sphere->material.Occlusion = 1.0f;
		sphere->material.EmissionColor = Color(0.0f, 0.0f, 0.0f, 1.0f);
		sphere->material.EmissionStrength = 0.0f;

		//	����ƽ������
		sphere->mesh.CreateSphere(0.5f, 20, 20);

		//	��������
		sphere->position = Vector3(0, -1, 3);
		sphere->eulerangle = Vector3(0, 0, 0);
		sphere->scale = Vector3(2, 2, 2);

		//	������ײ��
		sphere->bound.aabb = BoundingAABB(sphere->mesh.data);

		//	����AABB����������
		sphere->bound.aabb.m_min = sphere->GetWorldMatrix() * sphere->bound.aabb.m_min;
		sphere->bound.aabb.m_max = sphere->GetWorldMatrix() * sphere->bound.aabb.m_max;
		mMeshRender.push_back(std::move(sphere));
	}

	//	������Ⱦ��
	void DefaultScene::LoadRenderItem(Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList)
	{
		for (size_t i = 0; i < mMeshRender.size(); i++)
		{
			//	��׶�޳�
			if (!PhysicsUtils::CheckFrustumAABBIntersect(mCamera, mMeshRender[i]->bound))
				return;

			auto Ritem = std::make_unique<PBRRenderItem>();

			XMStoreFloat4x4(&Ritem->World, mMeshRender[i]->GetWorldMatrix().ToSIMD());
			Ritem->TexTransform = Dist::Identity4x4();
			Ritem->ObjCBIndex = 0;
			Ritem->Mat = &mMeshRender[i]->material;
			Ritem->Geo = SystemUtils::BuidlMeshGeometryFromMeshData(mMeshRender[i]->name,mMeshRender[i]->mesh.data,md3dDevice,mCommandList);
			Ritem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			Ritem->IndexCount = Ritem->Geo->DrawArgs["mesh"].IndexCount;
			Ritem->StartIndexLocation = Ritem->Geo->DrawArgs["mesh"].StartIndexLocation;
			Ritem->BaseVertexLocation = Ritem->Geo->DrawArgs["mesh"].BaseVertexLocation;
			
			mRitemLayer[(int)RenderLayer::Opaque].push_back(Ritem.get());
			mAllRitems.push_back(std::move(Ritem));
		}
	}
}