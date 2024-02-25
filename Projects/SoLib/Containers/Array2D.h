#pragma once
#include <vector>
#include <stdexcept>
#include <memory>

namespace SoLib {
	namespace Containers {

		template<typename T>
		class Array2D {
		public:
			// コンストラクタ
			Array2D(size_t rows = 0u, size_t cols = 0u) { Resize(rows, cols); }
			Array2D &operator=(const Array2D &other) {
				Resize(other.GetRows(), other.GetCols());
				std::copy(other.begin(), other.end(), this->begin());
			}
			Array2D &operator=(Array2D &&other) {
				data_ = std::move(other.data_);
				rows_ = other.rows_;
				cols_ = other.cols_;
			}

			// 行数を取得
			size_t GetRows() const {
				return rows_;
			}

			// 列数を取得
			size_t GetCols() const {
				return cols_;
			}

			// 指定した位置の要素を取得
			T &at(size_t row, size_t col) {
				if (row >= rows_ || col >= cols_) {
					throw std::out_of_range("添え字が範囲外に出ています");
				}
				return (*this)[row][col];
			}

			const T &at(size_t row, size_t col) const {
				if (row >= rows_ || col >= cols_) {
					throw std::out_of_range("添え字が範囲外に出ています");
				}
				return (*this)[row][col];
			}

			// 行数を変更
			void ResizeRows(size_t newRows) {
				if (newRows == rows_) { return; }

				std::unique_ptr<T[]> tmp = std::make_unique<T[]>(cols_ * newRows);
				std::move(data_.get(), data_.get() + std::min(rows_, newRows) * cols_, tmp.get());

				data_ = std::move(tmp);
				rows_ = newRows;
			}

			// 列数を変更
			void ResizeCols(size_t newCols) {
				if (newCols == cols_) { return; }
				std::unique_ptr<T[]> tmp = std::make_unique<T[]>(newCols * rows_);
				T *oldBegin = data_.get();
				T *newBegin = tmp.get();

				for (size_t i = 0u; i < rows_; i++) {
					std::move(oldBegin, oldBegin + std::min(cols_, newCols), newBegin);
					oldBegin += cols_;
					newBegin += newCols;
				}

				data_ = std::move(tmp);

				cols_ = newCols;
			}

			// 行と列のサイズを同時に変更
			void Resize(size_t newRows, size_t newCols) {
				if (not data_) {
					data_ = std::make_unique<T[]>(newRows * newCols);
					rows_ = newRows;
					cols_ = newCols;
					return;
				}
				ResizeRows(newRows);
				ResizeCols(newCols);
			}

			T *operator[](size_t i) {
#ifdef _DEBUG
				if (i >= rows_) {
					throw std::out_of_range("添え字が範囲外に出ています");
				}
#endif // _DEBUG
				return &data_[i * cols_];
			}

			const T *operator[](size_t i) const {
#ifdef _DEBUG
				if (i >= rows_) {
					throw std::out_of_range("添え字が範囲外に出ています");
				}
#endif // _DEBUG
				return &data_[i * cols_];
			}

			auto &get() { return data_; }
			const auto &get()const { return data_; }


			class iterator {
			public:
				iterator(Array2D<T> *parent, size_t row, size_t col) : pParent_(parent), rowIt_(row), colIt_(col), kMaxRow_(parent->GetRows()), kMaxCol_(parent->GetCols()) {}

				bool operator==(const iterator &other) const {
					return rowIt_ == other.rowIt_ && colIt_ == other.colIt_;
				}

				bool operator!=(const iterator &other) const {
					return !(*this == other);
				}

				iterator &operator++() {
					++colIt_;
					if (colIt_ == kMaxCol_) {
						++rowIt_;
						colIt_ = 0;
					}
					return *this;
				}

				T &operator*() {
					return pParent_->at(rowIt_, colIt_);
				}

			private:
				size_t rowIt_;
				size_t colIt_;

				size_t kMaxRow_;
				size_t kMaxCol_;

				Array2D<T> *pParent_;

			};

			class const_iterator {
			public:
				const_iterator(const Array2D<T> *parent, size_t row, size_t col) : pParent_(parent), rowIt_(row), colIt_(col), kMaxRow_(parent->GetRows()), kMaxCol_(parent->GetCols()) {}

				bool operator==(const const_iterator &other) const {
					return rowIt_ == other.rowIt_ && colIt_ == other.colIt_;
				}

				bool operator!=(const const_iterator &other) const {
					return !(*this == other);
				}

				const_iterator &operator++() {
					++colIt_;
					if (colIt_ == kMaxCol_) {
						++rowIt_;
						colIt_ = 0;
					}
					return *this;
				}

				const T &operator*() const {
					return pParent_->at(rowIt_, colIt_);
				}

			private:
				size_t rowIt_;
				size_t colIt_;

				size_t kMaxRow_;
				size_t kMaxCol_;

				const Array2D<T> *pParent_;

			};

			iterator begin() {
				return iterator(this, 0, 0);
			}

			const_iterator begin() const {
				return const_iterator(this, 0, 0);
			}
			const_iterator cbegin() const {
				return const_iterator(this, 0, 0);
			}

			iterator end() {
				return iterator(this, rows_, 0);
			}
			const_iterator end() const {
				return const_iterator(this, rows_, 0);
			}
			const_iterator cend() const {
				return const_iterator(this, rows_, 0);
			}
			struct View {

				T *ptr;
				T *endPtr;

				T *begin() { return ptr; }
				T *end() { return endPtr; }


			};
			struct ConstView {

				const T *ptr;
				const T *endPtr;
				const T *begin() { return ptr; }
				const T *end() { return endPtr; }

			};

			View view() { return View{ data_.get(),data_.get() + rows_ * cols_ }; }
			ConstView view()const { return ConstView{ data_.get(),data_.get() + rows_ * cols_ }; }

		private:

			size_t rows_;	// 行
			size_t cols_;	// 列

			std::unique_ptr<T[]> data_;

		};

	}
}