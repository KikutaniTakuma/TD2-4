//#pragma once
//#include "Drawers/PeraRender/PeraRender.h"
//#include "Drawers/Texture2D/Texture2D.h"
//#include "./Camera/Camera.h"
//#include <memory>
//
//class Cloud final {
//private:
//	Cloud() = default;
//	Cloud(const Cloud&) = delete;
//	Cloud(Cloud&&) = delete;
//public:
//	~Cloud() = default;
//
//	Cloud& operator=(const Cloud&) = delete;
//	Cloud& operator=(Cloud&&) = delete;
//
//public:
//	static void Initialize();
//
//	static void Finalize();
//
//	static Cloud* const GetInstance();
//
//private:
//	static Cloud* instance_;
//
//private:
//	void Init();
//
//public:
//	void Update();
//
//	void Draw();
//
//	inline Texture* const GetTex() const {
//		return pera2_->GetTex();
//	}
//
//private:
//	class CloudPipeline* pipeline_ = nullptr;
//
//	std::unique_ptr<PeraRender> pera_;
//	std::unique_ptr<PeraRender> pera2_;
//	std::unique_ptr<Camera> staticCamera_;
//	std::unique_ptr<Texture2D> tex_;
//};