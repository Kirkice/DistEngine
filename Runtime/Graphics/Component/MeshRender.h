/***********************************************************************************************
 ***				      G R A P H I C S C O R E  ---  D I S T E N G I N E                  ***
 ***********************************************************************************************
 *                                                                                             *
 *                                   Project Name : GraphicsCore							   *
 *                                                                                             *
 *                                   File Name : MeshRender.h								   *
 *                                                                                             *
 *                                    Programmer : Kirk                                        *
 *                                                                                             *
 *                                     Date : 2022/8/12                                        *
 *                                                                                             *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#pragma once
#include "DX12Utils.h"
#include "Component.h";
#include "MeshFliter.h"
#include "Mathf.h"
#include "../Materials/Material.h"


class MeshRender : public Component
{
public:
    MeshRender(std::string _name);

    MeshRender(
        MeshFliter* mesh,
        Material* mat,
        std::string _name);

    ~MeshRender();

public:

    //  ����
    MeshFliter* mesh;

    //  Lit
    Material* mat;

    //  ����
    void Draw(const ComPtr<ID3D12GraphicsCommandList>& pCmdList, bool bundleUsingOverride);
};
