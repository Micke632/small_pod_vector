
// ml-small_pod_vector v1.00


//                  VERSION HISTORY
//
//  1.00 Initial version
//  1.01 implemented resize()

#pragma once

#include <type_traits>
#include <cstddef>
#include <memory>
#include <assert.h>

namespace ml
{

	namespace impl
	{
		class pod_allocator
		{
		public:
			using size_type = size_t;
			static void* malloc(size_type size) { return std::malloc(size); }
			static void free(void* mem) { std::free(mem); }

		};
	}

	template<typename T, size_t StaticCapacity = 16, size_t RevertToStaticSize = 0, class Alloc = impl::pod_allocator>
	class small_pod_vector
	{
		static_assert(RevertToStaticSize <= StaticCapacity + 1, "ml::small_pod_vector: the revert-to-static size shouldn't exceed the static capacity by more than one");

		static_assert(std::is_trivial<T>::value, "ml::small_pod_vector with non-trivial type");


	public:
		using allocator_type = Alloc;
		using value_type = T;
		using size_type = typename Alloc::size_type;
		using reference = T & ;
		using const_reference = const T&;
		using pointer = T * ;

		using const_pointer = const T*;
		using iterator = pointer;
		using const_iterator = const_pointer;
		using reverse_iterator = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;

		static constexpr size_t static_capacity = StaticCapacity;
		static constexpr intptr_t revert_to_static_size = RevertToStaticSize;

		small_pod_vector()
			: small_pod_vector(Alloc())
		{}

		small_pod_vector(const Alloc& alloc)
			: m_alloc(alloc)
			, m_capacity(StaticCapacity)
			, m_dynamic_capacity(0)
			, m_dynamic_data(nullptr)
		{
			m_begin = m_end = static_begin_ptr();
		}

		explicit small_pod_vector(size_t count, const Alloc& alloc = Alloc())
			: small_pod_vector(alloc)
		{
			resize(count);
		}

		explicit small_pod_vector(size_t count, const T& value, const Alloc& alloc = Alloc())
			: small_pod_vector(alloc)
		{
			assign_impl(count, value);
		}

		template <class InputIterator, typename = decltype(*std::declval<InputIterator>())>
		small_pod_vector(InputIterator first, InputIterator last, const Alloc& alloc = Alloc())
			: small_pod_vector(alloc)
		{
			assign_impl(first, last);
		}

		small_pod_vector(std::initializer_list<T> l, const Alloc& alloc = Alloc())
			: small_pod_vector(alloc)
		{
			assign_impl(l);
		}

		small_pod_vector(const small_pod_vector& v)
			: small_pod_vector(v, v.get_allocator())
		{}

		small_pod_vector(const small_pod_vector& v, const Alloc& alloc)
			: small_pod_vector(alloc)
		{
			if (v.size() > StaticCapacity)
			{
				m_dynamic_capacity = v.size();
				m_begin = m_dynamic_data = pointer(m_alloc.malloc(sizeof(value_type)*m_dynamic_capacity));
				m_capacity = v.size();
			}
			else
			{
				m_begin = static_begin_ptr();
				m_capacity = StaticCapacity;
			}

			memcpy(m_begin, v.m_begin, v.byte_size());

			m_end = m_begin + v.size();

		}

		small_pod_vector(small_pod_vector&& v)
			: m_alloc(std::move(v.m_alloc))
			, m_capacity(v.m_capacity)
			, m_dynamic_capacity(v.m_dynamic_capacity)
			, m_dynamic_data(v.m_dynamic_data)
		{
			if (v.m_begin == v.static_begin_ptr())
			{
				m_begin = static_begin_ptr();
				m_end = m_begin + v.size();

				memcpy(m_begin, v.m_begin, v.byte_size());

				v.clear();
			}
			else
			{
				m_begin = v.m_begin;
				m_end = v.m_end;
			}

			v.m_dynamic_capacity = 0;
			v.m_dynamic_data = nullptr;
			v.m_begin = v.m_end = v.static_begin_ptr();
			v.m_capacity = StaticCapacity;
		}

		~small_pod_vector()
		{
			clear();

			if (m_dynamic_data)
			{
				m_alloc.free(m_dynamic_data);
			}
		}

		small_pod_vector& operator=(const small_pod_vector& v)
		{
			if (this == &v)
			{
				return *this;
			}

			clear();

			if (m_begin != static_begin_ptr())
			{
				m_begin = static_begin_ptr();
			}


			auto buff = choose_data(v.size());

			memcpy(buff, v.m_begin, v.byte_size());

			m_begin = buff;
			m_end = m_begin + v.size();

			update_capacity();

			return *this;
		}

		small_pod_vector& operator=(small_pod_vector&& v)
		{
			clear();

			get_alloc() = std::move(v.get_alloc());
			m_capacity = v.m_capacity;
			m_dynamic_capacity = v.m_dynamic_capacity;
			m_dynamic_data = v.m_dynamic_data;

			if (v.m_begin == v.static_begin_ptr())
			{
				m_begin = static_begin_ptr();
				m_end = m_begin + v.size();

				memcpy(m_begin, v.m_begin, v.byte_size());

				v.clear();
			}
			else
			{
				m_begin = v.m_begin;
				m_end = v.m_end;
			}

			v.m_dynamic_capacity = 0;
			v.m_dynamic_data = nullptr;
			v.m_begin = v.m_end = v.static_begin_ptr();
			v.m_capacity = StaticCapacity;

			return *this;
		}

		void assign(size_type count, const T& value)
		{
			clear();
			assign_impl(count, value);
		}

		template <class InputIterator, typename = decltype(*std::declval<InputIterator>())>
		void assign(InputIterator first, InputIterator last)
		{
			clear();
			assign_impl(first, last);
		}

		void assign(std::initializer_list<T> ilist)
		{
			clear();
			assign_impl(ilist);
		}

		allocator_type get_allocator() const
		{
			return m_alloc;
		}

		const_reference at(size_type i) const
		{
			assert(i < size());
			return *(m_begin + i);
		}

		reference at(size_type i)
		{
			assert(i < size());
			return *(m_begin + i);
		}

		const_reference operator[](size_type i) const
		{
			return at(i);
		}

		reference operator[](size_type i)
		{
			return at(i);
		}

		const_reference front() const
		{
			return at(0);
		}

		reference front()
		{
			return at(0);
		}

		const_reference back() const
		{
			return *(m_end - 1);
		}

		reference back()
		{
			return *(m_end - 1);
		}

		const_pointer data() const noexcept
		{
			return m_begin;
		}

		pointer data() noexcept
		{
			return m_begin;
		}

		// iterators
		iterator begin() noexcept
		{
			return m_begin;
		}

		const_iterator begin() const noexcept
		{
			return m_begin;
		}

		const_iterator cbegin() const noexcept
		{
			return m_begin;
		}

		iterator end() noexcept
		{
			return m_end;
		}

		const_iterator end() const noexcept
		{
			return m_end;
		}

		const_iterator cend() const noexcept
		{
			return m_end;
		}

		reverse_iterator rbegin() noexcept
		{
			return reverse_iterator(end());
		}

		const_reverse_iterator rbegin() const noexcept
		{
			return const_reverse_iterator(end());
		}

		const_reverse_iterator crbegin() const noexcept
		{
			return const_reverse_iterator(end());
		}

		reverse_iterator rend() noexcept
		{
			return reverse_iterator(begin());
		}

		const_reverse_iterator rend() const noexcept
		{
			return const_reverse_iterator(begin());
		}

		const_reverse_iterator crend() const noexcept
		{
			return const_reverse_iterator(begin());
		}

		bool empty() const noexcept
		{
			return m_begin == m_end;
		}

		size_t size() const noexcept
		{
			return m_end - m_begin;
		}

		size_t byte_size() const noexcept
		{
			return sizeof(value_type) * size();
		}

		void reserve(size_type new_cap)
		{
			if (new_cap <= m_capacity) return;

			auto new_buf = choose_data(new_cap);

			assert(new_buf != m_begin); // should've been handled by new_cap <= m_capacity
			assert(new_buf != static_begin_ptr()); // we should never reserve into static memory

			const auto s = size();
			if (s < RevertToStaticSize)
			{
				// we've allocated enough memory for the dynamic buffer but don't move there until we have to
				return;
			}

			memcpy(new_buf, m_begin, m_capacity * sizeof(value_type));

			if (m_begin != static_begin_ptr())
			{
				assert(m_begin != m_dynamic_data);
				m_alloc.free(m_begin);
			}

			m_begin = new_buf;
			m_end = new_buf + s;
			m_capacity = m_dynamic_capacity;
		}

		constexpr size_t capacity() const noexcept
		{
			return m_capacity;
		}

		void shrink_to_fit()
		{
			const auto s = size();

			if (s == m_capacity) return;
			if (m_begin == static_begin_ptr()) return;


			if (s < StaticCapacity)
			{
				// revert to static capacity
				m_begin = static_begin_ptr();

				m_capacity = StaticCapacity;

				memcpy(m_begin, m_dynamic_data, m_capacity * sizeof(value_type));

				m_end = m_begin + s;

				//deallocate memory. 				
				m_alloc.free(m_dynamic_data);
				m_dynamic_data = nullptr;
				m_dynamic_capacity = 0;
			}
			else
			{
				// alloc new smaller buffer

				auto new_buf = pointer(m_alloc.malloc(sizeof(value_type)*s));

				m_capacity = s;

				memcpy(new_buf, m_begin, byte_size());

				m_alloc.free(m_dynamic_data);

				m_dynamic_data = new_buf;
				m_begin = m_dynamic_data;
				m_dynamic_capacity = m_capacity;
				m_end = m_begin + s;
			}


		}


		void clear() noexcept
		{
			if (RevertToStaticSize > 0)
			{
				m_begin = m_end = static_begin_ptr();
				m_capacity = StaticCapacity;
			}
			else
			{
				m_end = m_begin;
			}

		}

		iterator insert(const_iterator position, const value_type& val)
		{
			auto pos = grow_at(position, 1);
			*pos = val;
			return pos;
		}

		iterator insert(const_iterator position, value_type&& val)
		{
			auto pos = grow_at(position, 1);
			*pos = val;
			return pos;
		}



		template <typename InputIterator, typename = decltype(*std::declval<InputIterator>())>
		iterator insert(const_iterator position, InputIterator first, InputIterator last)
		{
			auto pos = grow_at(position, last - first);

			copy_not_aliased(pos, first, last);

			return pos;
		}

		iterator insert(const_iterator position, std::initializer_list<T> ilist)
		{
			auto pos = grow_at(position, ilist.size());

			copy_not_aliased(pos, ilist.begin(), ilist.end());

			return pos;
		}

		iterator erase(const_iterator position)
		{
			return shrink_at(position, 1);
		}

		iterator erase(const_iterator first, const_iterator last)
		{
			assert(first <= last);
			return shrink_at(first, last - first);
		}

		void push_back(const_reference val)
		{
			auto pos = grow_at(m_end, 1);
			*pos = val;
		}



		void pop_back()
		{
			assert(m_end > m_begin);
			m_end = m_end - 1;
		}


		void resize(size_type n)
		{
			auto new_buf = choose_data(n);

			if (new_buf == m_begin)
			{
				reserve(n);

				m_end = m_begin + n;
			}
			else
			{
				// we need to transfer the elements into the new buffer

				memcpy(new_buf, m_begin, m_capacity * sizeof(value_type));

				if (m_begin != static_begin_ptr())
				{
					if (new_buf != static_begin_ptr())
					{
						assert(m_begin != m_dynamic_data);
						m_alloc.free(m_begin);
					}

				}
				update_capacity();

				m_begin = new_buf;
				m_end = new_buf + n;
			}

		}

	private:


		static void copy_not_aliased(T* p, const T* begin, const T* end)
		{
			auto s = size_t(end - begin) * sizeof(T);
			std::memcpy(p, begin, s);
		}


		T* static_begin_ptr()
		{
			return reinterpret_cast<pointer>(m_static_data + 0);
		}

		// increase the size by splicing the elements in such a way that
		// a hole of uninitialized elements is left at position, with size num
		// returns the (potentially new) address of the hole
		T* grow_at(const T* cp, size_t num)
		{
			auto position = const_cast<T*>(cp);

			assert(!(position < m_begin || position > m_end));

			const auto offset = cp - m_begin;
			const auto s = size();
			auto new_buf = choose_data(s + num);

			if (new_buf == m_begin)
			{
				std::memmove(m_begin + offset + num, m_begin + offset, (s - offset) * sizeof(value_type));
				m_end = m_begin + s + num;
				return m_begin + offset;
			}
			else
			{
				// we need to transfer the elements into the new buffer

				position = new_buf + (position - m_begin);

				memcpy(new_buf, m_begin, m_capacity * sizeof(value_type));

				std::memmove(new_buf + offset + num, new_buf + offset, (s - offset) * sizeof(value_type));

				if (m_begin != static_begin_ptr())
				{
					//deallocate old memory
					assert(m_begin != m_dynamic_data);
					m_alloc.free(m_begin);
				}

				m_capacity = m_dynamic_capacity;

				m_begin = new_buf;
				m_end = new_buf + s + num;

				return position;
			}
		}

		T* shrink_at(const T* cp, size_t num)
		{
			auto position = const_cast<T*>(cp);

			assert(!(position < m_begin || position > m_end || position + num > m_end));

			const auto s = size();

			auto new_buf = choose_data(s - num);

			if (new_buf == m_begin)
			{
				std::memmove(position, position + num, size_t(m_end - position - num) * sizeof(T));

				m_end -= num;
			}
			else
			{
				// we need to transfer the elements into the new buffer

				assert(new_buf == static_begin_ptr()); // since we're shrinking that's the only way to have a new buffer

				m_capacity = StaticCapacity;

				memcpy(new_buf, m_begin, byte_size());

				auto offset = position - m_begin;

				std::memmove(new_buf + offset, new_buf + offset + num, (s - offset) * sizeof(T));

				position = new_buf + offset;

				m_begin = new_buf;
				m_end = new_buf + (s - num);

			}

			return position;
		}

		void assign_impl(size_type count, const T& value)
		{
			assert(m_begin);
			assert(m_begin == m_end);

			m_begin = m_end = choose_data(count);

			for (size_type i = 0; i < count; ++i)
			{
				*m_end = value;

				++m_end;
			}

			update_capacity();
		}

		template <class InputIterator>
		void assign_impl(InputIterator first, InputIterator last)
		{
			assert(m_begin);
			assert(m_begin == m_end);

			m_begin = choose_data(last - first);

			copy_not_aliased(m_begin, first, last);

			m_end = m_begin + (last - first);

			update_capacity();
		}

		void assign_impl(std::initializer_list<T> ilist)
		{
			assert(m_begin);
			assert(m_begin == m_end);

			m_begin = choose_data(ilist.size());

			copy_not_aliased(m_begin, ilist.begin(), ilist.end());

			m_end = m_begin + ilist.size();

			update_capacity();
		}

		void update_capacity()
		{
			m_capacity = m_begin == static_begin_ptr() ? StaticCapacity : m_dynamic_capacity;

		}

		T* choose_data(size_t desired_capacity)
		{
			if (m_begin == m_dynamic_data)
			{
				// we're at the dyn buffer, so see if it needs resize or revert to static

				if (desired_capacity > m_dynamic_capacity)
				{

					while (m_dynamic_capacity < desired_capacity)
					{
						m_dynamic_capacity *= 2;

					}
					m_dynamic_data = pointer(m_alloc.malloc(sizeof(value_type)*m_dynamic_capacity));
					return m_dynamic_data;
				}
				else if (desired_capacity < RevertToStaticSize)
				{
					// we're reverting to the static buffer
					return static_begin_ptr();
				}
				else
				{
					// if the capacity and we don't revert to static, just do nothing
					return m_dynamic_data;
				}
			}
			else
			{
				assert(m_begin == static_begin_ptr()); // corrupt begin ptr?

				if (desired_capacity > StaticCapacity)
				{
					// we must move to dyn memory

					// see if we have enough
					if (desired_capacity > m_dynamic_capacity)
					{
						// we need to allocate more

						if (m_dynamic_data)
						{
							m_alloc.free(m_dynamic_data);
						}

						m_dynamic_capacity = desired_capacity;
						//add a little more
						m_dynamic_capacity += 4;

						m_dynamic_data = pointer(m_alloc.malloc(sizeof(value_type)*m_dynamic_capacity));
					}

					return m_dynamic_data;
				}
				else
				{
					// we have enough capacity as it is
					return static_begin_ptr();
				}
			}
		}

		allocator_type& get_alloc() { return static_cast<allocator_type&>(*this); }
		const allocator_type& get_alloc() const { return static_cast<const allocator_type&>(*this); }

		pointer m_begin;
		pointer m_end;

		size_t m_capacity;
		typename std::aligned_storage<sizeof(T), std::alignment_of<T>::value>::type m_static_data[StaticCapacity];

		size_t m_dynamic_capacity;
		pointer m_dynamic_data;
		Alloc m_alloc;

	};



}
