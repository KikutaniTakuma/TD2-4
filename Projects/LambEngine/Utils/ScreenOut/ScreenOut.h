#pragma once
#include <memory>
#include <string>
#include <cstdint>
#include <functional>

namespace Lamb{
	class ScreenOutAllocator;
}

class ScreenOut {
	friend class Lamb::ScreenOutAllocator;
private:
	ScreenOut();
	ScreenOut(const ScreenOut&) = delete;
	ScreenOut(ScreenOut&&) = delete;
	~ScreenOut() = default;

	ScreenOut& operator=(const ScreenOut&) = delete;
	ScreenOut& operator=(ScreenOut&&) = delete;

private:
	static void Initialize();

	static void Finalize();

	static ScreenOut* GetInstance();

private:
	static ScreenOut* instance_;

private:
	std::unique_ptr<class StringOut> sout_;
};

class Vector2;
class Vector3;
class Vector4;
class Quaternion;

namespace Lamb {
	class ScreenOutAllocator {
		friend class Engine;
	public:
		ScreenOutAllocator();
		~ScreenOutAllocator();
	private:
		ScreenOutAllocator(const ScreenOutAllocator&) = delete;
		ScreenOutAllocator(ScreenOutAllocator&&) = delete;

		ScreenOutAllocator& operator=(const ScreenOutAllocator&) = delete;
		ScreenOutAllocator& operator=(ScreenOutAllocator&&) = delete;

	public:
		const ScreenOutAllocator& operator<<(const std::string& right) const;
		const ScreenOutAllocator& operator<<(const std::wstring& right)const;

		const ScreenOutAllocator& operator<<(int8_t right)const;
		const ScreenOutAllocator& operator<<(int16_t right)const;
		const ScreenOutAllocator& operator<<(int32_t right)const;
		const ScreenOutAllocator& operator<<(int64_t right)const;

		const ScreenOutAllocator& operator<<(uint8_t right)const;
		const ScreenOutAllocator& operator<<(uint16_t right)const;
		const ScreenOutAllocator& operator<<(uint32_t right)const;
		const ScreenOutAllocator& operator<<(uint64_t right)const;

		const ScreenOutAllocator& operator<<(float right)const;
		const ScreenOutAllocator& operator<<(double right)const;

		const ScreenOutAllocator& operator<<(const Vector2& right)const;
		const ScreenOutAllocator& operator<<(const Vector3& right)const;
		const ScreenOutAllocator& operator<<(const Vector4& right)const;
		const ScreenOutAllocator& operator<<(const Quaternion& right)const;

		const ScreenOutAllocator& operator<<(std::function<const ScreenOutAllocator&(const ScreenOutAllocator&)> right)const;


		const ScreenOutAllocator& operator>>(std::string& right) const;
		const ScreenOutAllocator& operator>>(std::wstring& right)const;

	private:
		void Clear() const;
		void Draw() const;

	private:
		class ScreenOut* screenOutPtr_;
	};

	extern const ScreenOutAllocator screenout;

	const ScreenOutAllocator& endline(const ScreenOutAllocator& allocator);
	const ScreenOutAllocator& clear(const ScreenOutAllocator& allocator);
}