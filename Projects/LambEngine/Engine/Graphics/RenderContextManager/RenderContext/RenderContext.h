#pragma once
#include "../../GraphicsStructs.h"
#include "Engine/Core/DescriptorHeap/CbvSrvUavHeap.h"
#include "Engine/Core/DirectXCommand/DirectXCommand.h"

#include <typeinfo>
#include <concepts>
#include <array>
#include <memory>


class BaseRenderContext {
public:
    static constexpr uint32_t kMaxDrawInstance = 256u;


public:
    BaseRenderContext():
        mesh_(nullptr),
        pipeline_(nullptr),
        drawCount_(0),
        typeID_()
    {}
    virtual ~BaseRenderContext() = default;

    virtual void Draw() = 0;

    const std::string& GetID() const {
        return typeID_;
    }

    virtual void SetMesh(const Mesh* const mesh) = 0;
    virtual void SetPipeline(Pipeline* const pipeline) = 0;
    virtual void SetWVPMatrix(const WVPMatrix& matrix) = 0;
    virtual void SetColor(const Vector4& color) = 0;
    virtual void SetLight(const Light& light) = 0;

public:
    void AddDrawCount() {
        drawCount_++;
    }

    void ResetDrawCount() {
        drawCount_ = 0u;
    }

    bool IsDraw() const {
        return drawCount_ != 0u;
    }

    const std::string& GetRootName() const {
        return mesh_->node.name;
    }

    const Mesh* const GetMesh() const {
        return mesh_;
    }

protected:
    const Mesh* mesh_;

    Pipeline* pipeline_;
    uint32_t drawCount_;

    std::string typeID_;
};

using RenderData = BaseRenderContext;


template<class T = uint32_t, uint32_t bufferSize = RenderData::kMaxDrawInstance>
class RenderContext : public BaseRenderContext {
public:
    RenderContext():
        shaderData_()
    {
        pipeline_ = nullptr;
        drawCount_ = 0u;

        // ディスクリプタヒープ
        CbvSrvUavHeap* const descriptorHeap = CbvSrvUavHeap::GetInstance();

        descriptorHeap->BookingHeapPos(4);
        descriptorHeap->CreateView(shaderData_.light);
        descriptorHeap->CreateView(shaderData_.wvpMatrix);
        descriptorHeap->CreateView(shaderData_.color);
        descriptorHeap->CreateView(shaderData_.shaderStruct);

        typeID_ = (typeid(RenderContext<T, bufferSize>).name());
    }
    ~RenderContext() {
        // ディスクリプタヒープ
        CbvSrvUavHeap* const descriptorHeap = CbvSrvUavHeap::GetInstance();

        descriptorHeap->ReleaseView(shaderData_.light.GetHandleUINT());
        descriptorHeap->ReleaseView(shaderData_.wvpMatrix.GetHandleUINT());
        descriptorHeap->ReleaseView(shaderData_.color.GetHandleUINT());
        descriptorHeap->ReleaseView(shaderData_.shaderStruct.GetHandleUINT());
    }

    RenderContext(const RenderContext&) = delete;
    RenderContext(RenderContext&&) = delete;

    RenderContext& operator=(const RenderContext&) = delete;
    RenderContext& operator=(RenderContext&&) = delete;

public:
    [[noreturn]] void Draw() override {
        // ディスクリプタヒープ
        CbvSrvUavHeap* const descriptorHeap = CbvSrvUavHeap::GetInstance();
        // コマンドリスト
        ID3D12GraphicsCommandList* const commandlist = DirectXCommand::GetMainCommandlist()->GetCommandList();

        // パイプライン設定
        pipeline_->Use();

        // ライト構造体
        commandlist->SetGraphicsRootDescriptorTable(0, shaderData_.light.GetHandleGPU());
        // ワールドとカメラマトリックス, 色, 各シェーダーの構造体
        commandlist->SetGraphicsRootDescriptorTable(1, shaderData_.wvpMatrix.GetHandleGPU());
        // テクスチャ
        commandlist->SetGraphicsRootDescriptorTable(2, descriptorHeap->GetGpuHeapHandle(0));

        // 頂点バッファセット
        commandlist->IASetVertexBuffers(0, 1, &mesh_->vertexView);
        // インデックスバッファセット
        commandlist->IASetIndexBuffer(&mesh_->indexView);
        // ドローコール
        commandlist->DrawIndexedInstanced(mesh_->indexNumber, drawCount_, 0, 0, 0);
        //commandlist->DrawInstanced(mesh_->vertexNumber, drawCount_, 0, 0);
    }
    
public:
    inline void SetMesh(const Mesh* const mesh) override {
        if (!mesh) {
            throw Lamb::Error::Code<RenderContext>("mesh is nullptr", __func__);
        }
        mesh_ = mesh;
    }
    inline void SetPipeline(Pipeline* const pipeline) override {
        if (!pipeline) {
            throw Lamb::Error::Code<RenderContext>("pipeline is nullptr", __func__);
        }
        pipeline_ = pipeline;
    }
    inline void SetWVPMatrix(const WVPMatrix& matrix) override {
        if (kMaxDrawInstance <= drawCount_) {
            throw Lamb::Error::Code<RenderContext>("drawCount is over 256", __func__);
        }

        shaderData_.wvpMatrix[drawCount_].worldMat = mesh_->node.loacalMatrix * matrix.worldMat;
        shaderData_.wvpMatrix[drawCount_].cameraMat = matrix.cameraMat;
    }
    inline void SetColor(const Vector4& color) override {
        if (kMaxDrawInstance <= drawCount_) {
            throw Lamb::Error::Code<RenderContext>("drawCount is over 256", __func__);
        }

        shaderData_.color[drawCount_] = color;
    }
    inline void SetLight(const Light& light) override {
        *shaderData_.light = light;
    }
    inline void SetSahderStruct(const T& data) {
        if (kMaxDrawInstance <= drawCount_) {
            throw Lamb::Error::Code<RenderContext>("drawCount is over 256", __func__);
        }
        shaderData_.shaderStruct[drawCount_] = data;
    }


private:
    ShaderData<T, bufferSize> shaderData_;
};

template<class T>
concept IsBasedRenderContext =  std::is_base_of_v<BaseRenderContext, T>;

class RenderSet {
public:
    RenderSet() = default;
    ~RenderSet() = default;
    RenderSet(const RenderSet&) = delete;
    RenderSet(RenderSet&&) = delete;

    RenderSet& operator=(const RenderSet&) = delete;
    RenderSet& operator=(RenderSet&&) = delete;


public:
    inline void Set(RenderData* const renderData, BlendType blend) {
        if (not renderData) {
            throw Lamb::Error::Code<RenderSet>("renderData is nullptr", __func__);
        }

        renderDatas_[blend].reset(renderData);
    }


    inline void Draw() {
        for (auto& i : renderDatas_) {
            if (i->IsDraw()) {
                i->Draw();
            }
        }
    }

    inline void ResetDrawCount() {
        for (auto& i : renderDatas_) {
            i->ResetDrawCount();
        }
    }

public:
    inline RenderData* GetRenderContext(BlendType blend) {
        return renderDatas_[blend].get();
    }

    // ごり押し
    // やばい
    // マジでヤバイ
    // 使うときは気を付けて(一応型が違ったらエラーは出る)
    template<IsBasedRenderContext ClassName>
    inline ClassName* GetRenderContextDowncast(BlendType blend) {
        if (typeid(ClassName).name() != renderDatas_[blend]->GetID()) {
            throw Lamb::Error::Code<RenderSet>("does not match class type", __func__);
        }

        return dynamic_cast<ClassName*>(renderDatas_[blend].get());
    }

    const std::string& GetRootNodeName() const {
        return renderDatas_.front()->GetRootName();
    }

    const Mesh* const GetMesh() const {
        return renderDatas_.front()->GetMesh();
    }

public:
    std::array<std::unique_ptr<RenderData>, BlendType::kNum>::iterator begin() {
        return renderDatas_.begin();
    }
    std::array<std::unique_ptr<RenderData>, BlendType::kNum>::iterator end() {
        return renderDatas_.end();
    }
    std::array<std::unique_ptr<RenderData>, BlendType::kNum>::const_iterator cbegin() const {
        return renderDatas_.cbegin();
    }
    std::array<std::unique_ptr<RenderData>, BlendType::kNum>::const_iterator cend() const {
        return renderDatas_.cend();
    }
    std::array<std::unique_ptr<RenderData>, BlendType::kNum>::reverse_iterator rbegin() {
        return renderDatas_.rbegin();
    }
    std::array<std::unique_ptr<RenderData>, BlendType::kNum>::reverse_iterator rend() {
        return renderDatas_.rend();
    }
    std::array<std::unique_ptr<RenderData>, BlendType::kNum>::const_reverse_iterator crbegin() const {
        return renderDatas_.crbegin();
    }
    std::array<std::unique_ptr<RenderData>, BlendType::kNum>::const_reverse_iterator crend() const {
        return renderDatas_.crend();
    }

    std::unique_ptr<RenderData>* const data() {
        return renderDatas_.data();
    }
    const std::unique_ptr<RenderData>* const data() const {
        return renderDatas_.data();
    }

private:
    std::array<std::unique_ptr<RenderData>, BlendType::kNum> renderDatas_;
};