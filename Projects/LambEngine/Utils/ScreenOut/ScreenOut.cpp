#include "ScreenOut.h"
#include "Drawers/StringOut/StringOut.h"

ScreenOut* ScreenOut::instance_ = nullptr;

void ScreenOut::Initialize() {
	if (!instance_) {
		instance_ = new ScreenOut{};
	}
}

void ScreenOut::Finalize() {
	delete instance_;
	instance_ = nullptr;
}

ScreenOut* ScreenOut::GetInstance() {
	return instance_;
}

ScreenOut::ScreenOut() :
	sout_{}
{
	sout_ = std::make_unique<StringOut>();
	sout_->SetFormat("./Resources/Font/default.spritefont");
	sout_->scale *= 0.3f;
}

namespace Lamb {
	const ScreenOutAllocator screenout;
	const ScreenOutAllocator& endline(const ScreenOutAllocator& allocator) {
		allocator << "\n";
		return allocator;
	}

	const ScreenOutAllocator& clear(const ScreenOutAllocator& allocator) {
		std::string buf;
		allocator >> buf;
		return allocator;
	}

	ScreenOutAllocator::ScreenOutAllocator():
		screenOutPtr_{nullptr}
	{
		ScreenOut::Initialize();
		screenOutPtr_ = ScreenOut::GetInstance();
	}

	ScreenOutAllocator::~ScreenOutAllocator() {
		ScreenOut::Finalize();
	}

	const ScreenOutAllocator& ScreenOutAllocator::operator<<(const std::string& right) const {
		*(screenOutPtr_->sout_) << right;

		return *this;
	}
	const ScreenOutAllocator& ScreenOutAllocator::operator<<(const std::wstring& right) const {
		*(screenOutPtr_->sout_) << right;

		return *this;
	}

	const ScreenOutAllocator& ScreenOutAllocator::operator<<(int8_t right) const {
		*(screenOutPtr_->sout_) << right;

		return *this;
	}
	const ScreenOutAllocator& ScreenOutAllocator::operator<<(int16_t right) const {
		*(screenOutPtr_->sout_) << right;

		return *this;
	}
	const ScreenOutAllocator& ScreenOutAllocator::operator<<(int32_t right) const {
		*(screenOutPtr_->sout_) << right;

		return *this;
	}
	const ScreenOutAllocator& ScreenOutAllocator::operator<<(int64_t right) const {
		*(screenOutPtr_->sout_) << right;

		return *this;
	}

	const ScreenOutAllocator& ScreenOutAllocator::operator<<(uint8_t right) const {
		*(screenOutPtr_->sout_) << right;

		return *this;
	}
	const ScreenOutAllocator& ScreenOutAllocator::operator<<(uint16_t right) const {
		*(screenOutPtr_->sout_) << right;

		return *this;
	}
	const ScreenOutAllocator& ScreenOutAllocator::operator<<(uint32_t right) const {
		*(screenOutPtr_->sout_) << right;

		return *this;
	}
	const ScreenOutAllocator& ScreenOutAllocator::operator<<(uint64_t right) const {
		*(screenOutPtr_->sout_) << right;

		return *this;
	}

	const ScreenOutAllocator& ScreenOutAllocator::operator<<(float right) const {
		*(screenOutPtr_->sout_) << right;

		return *this;
	}
	const ScreenOutAllocator& ScreenOutAllocator::operator<<(double right) const {
		*(screenOutPtr_->sout_) << right;

		return *this;
	}

	const ScreenOutAllocator& ScreenOutAllocator::operator<<(const Vector2& right) const {
		*(screenOutPtr_->sout_) << right;

		return *this;
	}
	const ScreenOutAllocator& ScreenOutAllocator::operator<<(const Vector3& right) const {
		*(screenOutPtr_->sout_) << right;

		return *this;
	}
	const ScreenOutAllocator& ScreenOutAllocator::operator<<(const Vector4& right) const {
		*(screenOutPtr_->sout_) << right;

		return *this;
	}
	const ScreenOutAllocator& ScreenOutAllocator::operator<<(const Quaternion& right) const {
		*(screenOutPtr_->sout_) << right;

		return *this;
	}

	const ScreenOutAllocator& ScreenOutAllocator::operator>>(std::string& right) const {
		*(screenOutPtr_->sout_) >> right;

		return *this;
	}
	const ScreenOutAllocator& ScreenOutAllocator::operator>>(std::wstring& right) const {
		*(screenOutPtr_->sout_) >> right;

		return *this;
	}

	void ScreenOutAllocator::Clear() const {
		screenOutPtr_->sout_->Clear();
	}

	void ScreenOutAllocator::Draw() const {
		screenOutPtr_->sout_->Draw();
	}
	const ScreenOutAllocator& ScreenOutAllocator::operator<<(std::function<const ScreenOutAllocator& (const ScreenOutAllocator&)> right)const {
		return right(*this);
	}
}