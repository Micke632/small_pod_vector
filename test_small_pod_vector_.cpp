
#include "small_pod_vector.hpp"

TEST(TestCaseName, smallpod)
{
	{
		ml::small_pod_vector<int, 4> vec;

		EXPECT_EQ(vec.empty(), true);
		EXPECT_EQ(vec.size(), 0);
		EXPECT_EQ(vec.capacity(), 4);
		EXPECT_EQ(vec.end(), vec.begin());

		vec.push_back(1);
		{

			EXPECT_EQ(vec.front(), 1);
			EXPECT_EQ(vec.back(), 1);

			int ints[] = { 1 };

			EXPECT_EQ(vec.size(), 1);
			EXPECT_EQ(memcmp(vec.data(), ints, sizeof(ints)), 0);
		}

		vec.push_back(2);
		vec.push_back(3);
		vec.push_back(4);

		{
			EXPECT_EQ(vec.front(), 1);
			EXPECT_EQ(vec.back(), 4);

			int ints[] = { 1,2,3,4 };

			EXPECT_EQ(vec.size(), 4);
			EXPECT_EQ(memcmp(vec.data(), ints, sizeof(ints)), 0);
		}


		//dynamic memory
		vec.push_back(5);

		{

			EXPECT_EQ(vec.front(), 1);
			EXPECT_EQ(vec.back(), 5);

			int ints[] = { 1,2,3,4,5 };

			EXPECT_EQ(vec.size(), 5);
			EXPECT_EQ(memcmp(vec.data(), ints, sizeof(ints)), 0);
		}

		vec.clear();

		{
			EXPECT_EQ(vec.end(), vec.begin());
			EXPECT_EQ(vec.size(), 0);

		}
	}

	{

		ml::small_pod_vector<int, 4> vec(4);

		EXPECT_EQ(vec.size(), 4);

		vec[0] = 0;
		vec[1] = 1;
		vec[2] = 2;
		vec[3] = 3;

		{
			int ints[] = { 0,1,2,3 };
			EXPECT_EQ(memcmp(vec.data(), ints, sizeof(ints)), 0);
			EXPECT_EQ(vec.front(), 0);
			EXPECT_EQ(vec.back(), 3);
		}

	}

	{

		ml::small_pod_vector<int, 4> vec;

		vec.resize(4);

		vec[0] = 0;
		vec[1] = 1;
		vec[2] = 2;
		vec[3] = 3;

		{
			int ints[] = { 0,1,2,3 };

			EXPECT_EQ(vec.size(), 4);
			EXPECT_EQ(memcmp(vec.data(), ints, sizeof(ints)), 0);
			EXPECT_EQ(vec.front(), 0);
			EXPECT_EQ(vec.back(), 3);

		}

		//undefined behavior
		//vec[4] =4;

		vec.push_back(4);


		{
			int ints[] = { 0,1,2,3,4 };

			EXPECT_EQ(vec.size(), 5);
			EXPECT_EQ(memcmp(vec.data(), ints, sizeof(ints)), 0);
			EXPECT_EQ(vec.front(), 0);
			EXPECT_EQ(vec.back(), 4);

		}


	}
}


TEST(TestCaseName, smallpod2)
{
	ml::small_pod_vector<int, 4> vec;
	//reserve more then static size..
	vec.reserve(8);

	vec.push_back(1);
	{

		int ints[] = { 1 };

		EXPECT_EQ(vec.size(), 1);
		EXPECT_EQ(memcmp(vec.data(), ints, sizeof(ints)), 0);
	}

	vec.push_back(2);
	vec.push_back(3);
	vec.push_back(4);

	{
		int ints[] = { 1,2,3,4 };

		EXPECT_EQ(vec.size(), 4);
		EXPECT_EQ(memcmp(vec.data(), ints, sizeof(ints)), 0);
	}


	vec.push_back(5);

	{

		int ints[] = { 1,2,3,4,5 };

		EXPECT_EQ(vec.size(), 5);
		EXPECT_EQ(memcmp(vec.data(), ints, sizeof(ints)), 0);
	}

	vec.push_back(6);
	vec.push_back(7);
	vec.push_back(8);
	{
		int ints[] = { 1,2,3,4,5,6,7,8 };

		EXPECT_EQ(vec.size(), 8);
		EXPECT_EQ(memcmp(vec.data(), ints, sizeof(ints)), 0);
	}

	//allocs more space
	auto it = vec.insert(vec.end(), { 9,9,9,9,9 });

	{
		EXPECT_EQ(it, vec.begin() + 8);
		int ints[] = { 1,2,3,4,5,6,7,8,9,9,9,9,9 };

		EXPECT_EQ(vec.size(), 13);
		EXPECT_EQ(memcmp(vec.data(), ints, sizeof(ints)), 0);
	}

	vec.reserve(32);
	{
		int ints[] = { 1,2,3,4,5,6,7,8,9,9,9,9,9 };

		EXPECT_EQ(vec.size(), 13);
		EXPECT_EQ(memcmp(vec.data(), ints, sizeof(ints)), 0);
		EXPECT_EQ(vec.front(), 1);
		EXPECT_EQ(vec.back(), 9);


	}

	vec.shrink_to_fit();

	{
		int ints[] = { 1,2,3,4,5,6,7,8,9,9,9,9,9 };

		EXPECT_EQ(vec.size(), 13);
		EXPECT_EQ(memcmp(vec.data(), ints, sizeof(ints)), 0);
		EXPECT_EQ(vec.front(), 1);
		EXPECT_EQ(vec.back(), 9);
	}

	vec.push_back(10);
	{
		int ints[] = { 1,2,3,4,5,6,7,8,9,9,9,9,9,10 };

		EXPECT_EQ(vec.size(), 14);
		EXPECT_EQ(memcmp(vec.data(), ints, sizeof(ints)), 0);
		EXPECT_EQ(vec.front(), 1);
		EXPECT_EQ(vec.back(), 10);
	}

	vec.erase(vec.begin() + 1, vec.end());

	{
		int ints[] = { 1 };

		EXPECT_EQ(vec.size(), 1);
		EXPECT_EQ(memcmp(vec.data(), ints, sizeof(ints)), 0);
		EXPECT_EQ(vec.front(), 1);
		EXPECT_EQ(vec.back(), 1);
	}

	vec.shrink_to_fit();

	{
		int ints[] = { 1 };

		EXPECT_EQ(vec.size(), 1);

		EXPECT_EQ(memcmp(vec.data(), ints, sizeof(ints)), 0);
		EXPECT_EQ(vec.front(), 1);
		EXPECT_EQ(vec.back(), 1);
	}

	vec.push_back(10);

	{
		int ints[] = { 1,10 };

		EXPECT_EQ(vec.size(), 2);
		EXPECT_EQ(memcmp(vec.data(), ints, sizeof(ints)), 0);
		EXPECT_EQ(vec.front(), 1);
		EXPECT_EQ(vec.back(), 10);
	}

	vec.erase(vec.begin() + 1, vec.end());


	vec.shrink_to_fit();

	{
		int ints[] = { 1 };

		EXPECT_EQ(vec.size(), 1);
		EXPECT_EQ(memcmp(vec.data(), ints, sizeof(ints)), 0);
		EXPECT_EQ(vec.front(), 1);
		EXPECT_EQ(vec.back(), 1);
	}

}

TEST(TestCaseName, smallpod3)
{
	ml::small_pod_vector<int, 4> vec;

	vec.push_back(1);
	vec.push_back(2);
	vec.push_back(3);
	vec.push_back(4);

	{

		int ints[] = { 1,2,3,4 };

		EXPECT_EQ(vec.size(), 4);
		EXPECT_EQ(memcmp(vec.data(), ints, sizeof(ints)), 0);
	}

	vec.pop_back();

	{

		int ints[] = { 1,2,3 };

		EXPECT_EQ(vec.size(), 3);
		EXPECT_EQ(memcmp(vec.data(), ints, sizeof(ints)), 0);
	}

	auto it = vec.erase(vec.begin());

	{
		EXPECT_EQ(it, vec.begin());
		int ints[] = { 2,3 };

		EXPECT_EQ(vec.size(), 2);
		EXPECT_EQ(memcmp(vec.data(), ints, sizeof(ints)), 0);
	}

	vec.push_back(5);
	vec.push_back(6);
	vec.push_back(7);
	{

		int ints[] = { 2,3,5,6,7 };

		EXPECT_EQ(vec.size(), 5);
		EXPECT_EQ(memcmp(vec.data(), ints, sizeof(ints)), 0);
	}


	vec.erase(vec.begin());

	{

		int ints[] = { 3,5,6,7 };

		EXPECT_EQ(vec.size(), 4);
		EXPECT_EQ(memcmp(vec.data(), ints, sizeof(ints)), 0);
	}

	vec.erase(vec.begin() + 1);


	{

		int ints[] = { 3,6,7 };

		EXPECT_EQ(vec.size(), 3);
		EXPECT_EQ(memcmp(vec.data(), ints, sizeof(ints)), 0);
	}

	vec.insert(vec.begin(), 1);

	{
		int ints[] = { 1,3,6,7 };

		EXPECT_EQ(vec.size(), 4);
		EXPECT_EQ(memcmp(vec.data(), ints, sizeof(ints)), 0);
	}


	vec.insert(vec.begin() + 1, 2);

	{
		int ints[] = { 1,2,3,6,7 };

		EXPECT_EQ(vec.size(), 5);
		EXPECT_EQ(memcmp(vec.data(), ints, sizeof(ints)), 0);
	}

	vec.pop_back();
	vec.pop_back();
	vec.pop_back();
	vec.pop_back();
	vec.pop_back();

	{
		EXPECT_EQ(vec.size(), 0);
	}
	{
		ml::small_pod_vector<int, 4> vec2;

		vec2.push_back(1);
		vec2.push_back(2);
		vec2.push_back(3);

		auto it = vec2.insert(vec2.begin() + 3, 4);
		EXPECT_EQ(it, vec2.begin() + 3);


		//dynamic data
		it = vec2.insert(vec2.begin() + 1, 2);

		{
			EXPECT_EQ(it, vec2.begin() + 1);

			int ints[] = { 1,2,2,3,4 };

			EXPECT_EQ(vec2.size(), 5);
			EXPECT_EQ(memcmp(vec2.data(), ints, sizeof(ints)), 0);
		}

		it = vec2.erase(vec2.begin(), vec2.begin() + 3);

		EXPECT_EQ(vec2.size(), 2);
		EXPECT_EQ(it, vec2.begin());

		it = vec2.erase(vec2.begin() + 1);

		EXPECT_EQ(vec2.size(), 1);
		EXPECT_EQ(it, vec2.begin() + 1);
		EXPECT_EQ(it++, vec2.end());

	}
	{

		ml::small_pod_vector<int, 4, 2> vec2;

		vec2.push_back(1);
		vec2.push_back(2);
		vec2.push_back(3);
		vec2.push_back(4);
		{

			int ints[] = { 1,2,3,4 };

			EXPECT_EQ(vec2.size(), 4);
			EXPECT_EQ(memcmp(vec2.data(), ints, sizeof(ints)), 0);
		}

		vec2.push_back(5);
		{

			int ints[] = { 1,2,3,4,5 };

			EXPECT_EQ(vec2.size(), 5);
			EXPECT_EQ(memcmp(vec2.data(), ints, sizeof(ints)), 0);
		}

		vec2.erase(vec2.begin() + 1);
		{
			int ints[] = { 1,3,4,5 };

			EXPECT_EQ(vec2.size(), 4);
			EXPECT_EQ(memcmp(vec2.data(), ints, sizeof(ints)), 0);
		}

		vec2.erase(vec2.begin() + 1);

		{
			int ints[] = { 1,4,5 };

			EXPECT_EQ(vec2.size(), 3);
			EXPECT_EQ(memcmp(vec2.data(), ints, sizeof(ints)), 0);
		}


		auto it = vec2.erase(vec2.begin() + 1);

		{
			EXPECT_EQ(it, vec2.begin() + 1);
			int ints[] = { 1,5 };

			EXPECT_EQ(vec2.size(), 2);
			EXPECT_EQ(memcmp(vec2.data(), ints, sizeof(ints)), 0);
		}

		//move back to static_size
		it = vec2.erase(vec2.begin() + 1);

		{
			EXPECT_EQ(it, vec2.begin() + 1);
			EXPECT_EQ(it++, vec2.end());
			int ints[] = { 1 };

			EXPECT_EQ(vec2.size(), 1);
			EXPECT_EQ(vec2.front(), 1);
			EXPECT_EQ(vec2.back(), 1);
			EXPECT_EQ(memcmp(vec2.data(), ints, sizeof(ints)), 0);
		}

		vec2.push_back(4);
		{

			int ints[] = { 1,4 };

			EXPECT_EQ(vec2.size(), 2);
			EXPECT_EQ(memcmp(vec2.data(), ints, sizeof(ints)), 0);
		}

		vec2.insert(vec2.begin(), { 1,2,3,4 });

		{

			int ints[] = { 1,2,3,4,1,4 };

			EXPECT_EQ(vec2.size(), 6);
			EXPECT_EQ(memcmp(vec2.data(), ints, sizeof(ints)), 0);
		}
		const int val = 1;

		vec2.insert(vec2.begin(), val);

		{
			int ints[] = { 1,1,2,3,4,1,4 };

			EXPECT_EQ(vec2.size(), 7);
			EXPECT_EQ(memcmp(vec2.data(), ints, sizeof(ints)), 0);
		}

		vec.clear();
		vec.insert(vec.begin(), { 6,6,6 });

		vec2.insert(vec2.begin(), vec.begin(), vec.end());

		{

			int ints[] = { 6,6,6,1,1,2,3,4,1,4 };

			EXPECT_EQ(vec2.size(), 10);
			EXPECT_EQ(memcmp(vec2.data(), ints, sizeof(ints)), 0);
		}

		vec.insert(vec.begin(), { 1,2,3 });
		vec2.assign(vec.begin(), vec.end());

		{
			int ints[] = { 1,2,3,6,6,6 };
			EXPECT_EQ(vec2.size(), 6);
			EXPECT_EQ(memcmp(vec2.data(), ints, sizeof(ints)), 0);
		}


	}
}




TEST(TestCaseName, smallpod5)
{
	ml::small_pod_vector<int, 4> vec(2, 2);


	{
		int ints[] = { 2,2 };

		EXPECT_EQ(vec.size(), 2);
		EXPECT_EQ(memcmp(vec.data(), ints, sizeof(ints)), 0);
		EXPECT_EQ(vec.front(), 2);
		EXPECT_EQ(vec.back(), 2);

	}

	struct S
	{
		int t;
		int h;
		int g;
	};

	ml::small_pod_vector<S, 4> vec2;

	S s;
	s.t = 1;
	s.h = 10;
	s.g = 100;
	vec2.push_back(s);

	EXPECT_EQ(vec2.size(), 1);

	S ss = vec2[0];

	EXPECT_EQ(ss.t, 1);
	EXPECT_EQ(ss.h, 10);
	EXPECT_EQ(ss.g, 100);

}


TEST(TestCaseName, smallpod6)
{


	ml::small_pod_vector<int, 2> vec;

	vec.push_back(1);
	vec.push_back(2);
	vec.push_back(3);
	vec.push_back(4);

	ml::small_pod_vector<int, 2> vec2 = std::move(vec);

	{

		int ints[] = { 1,2,3,4 };

		EXPECT_EQ(vec2.size(), 4);
		EXPECT_EQ(memcmp(vec2.data(), ints, sizeof(ints)), 0);
		EXPECT_EQ(vec2.front(), 1);
		EXPECT_EQ(vec2.back(), 4);
	}

	ml::small_pod_vector<int, 2> vecc(1, 1);

	//move from static data
	ml::small_pod_vector<int, 2> vec2a = std::move(vecc);

	{

		int ints[] = { 1 };

		EXPECT_EQ(vec2a.size(), 1);
		EXPECT_EQ(memcmp(vec2a.data(), ints, sizeof(ints)), 0);
		EXPECT_EQ(vec2a.front(), 1);
		EXPECT_EQ(vec2a.back(), 1);
	}

	vec = vec2;

	{

		int ints[] = { 1,2,3,4 };

		EXPECT_EQ(vec.size(), 4);
		EXPECT_EQ(memcmp(vec.data(), ints, sizeof(ints)), 0);
		EXPECT_EQ(vec.front(), 1);
		EXPECT_EQ(vec.back(), 4);
	}


	ml::small_pod_vector<int, 2> vec3(6, 4);

	vec = vec3;

	{

		int ints[] = { 4,4,4,4,4,4 };

		EXPECT_EQ(vec.size(), 6);
		EXPECT_EQ(memcmp(vec.data(), ints, sizeof(ints)), 0);
		EXPECT_EQ(vec.front(), 4);
		EXPECT_EQ(vec.back(), 4);
	}


	ml::small_pod_vector<int, 2> vec4(vec);

	{

		int ints[] = { 4,4,4,4,4,4 };

		EXPECT_EQ(vec4.size(), 6);
		EXPECT_EQ(memcmp(vec4.data(), ints, sizeof(ints)), 0);
		EXPECT_EQ(vec4.front(), 4);
		EXPECT_EQ(vec4.back(), 4);
	}



	vec4.push_back(5);

	{

		int ints[] = { 4,4,4,4,4,4,5 };

		EXPECT_EQ(vec4.size(), 7);
		EXPECT_EQ(memcmp(vec4.data(), ints, sizeof(ints)), 0);
		EXPECT_EQ(vec4.front(), 4);
		EXPECT_EQ(vec4.back(), 5);
	}



	ml::small_pod_vector<int, 2> vec5(std::move(vec4));

	{

		int ints[] = { 4,4,4,4,4,4,5 };

		EXPECT_EQ(vec5.size(), 7);
		EXPECT_EQ(memcmp(vec5.data(), ints, sizeof(ints)), 0);
		EXPECT_EQ(vec5.front(), 4);
		EXPECT_EQ(vec5.back(), 5);
	}

	ml::small_pod_vector<int, 2> v(1, 1);

	ml::small_pod_vector<int, 2> vec6(std::move(v));

	{

		int ints[] = { 1 };

		EXPECT_EQ(vec6.size(), 1);
		EXPECT_EQ(memcmp(vec6.data(), ints, sizeof(ints)), 0);
		EXPECT_EQ(vec6.front(), 1);
		EXPECT_EQ(vec6.back(), 1);
	}

}


TEST(TestCaseName, smallpod8)
{


	ml::small_pod_vector<int, 6, 3> vec;

	vec.push_back(1);
	vec.push_back(2);
	vec.push_back(3);
	vec.push_back(4);
	vec.push_back(5);
	vec.push_back(6);
	vec.push_back(7);


	//old school loop

	for (auto it = vec.begin(); it != vec.end();)
	{
		if (((*it) == 2) || ((*it) == 4))
		{
			it = vec.erase(it);
		}
		else
			++it;
	}

	int ints[] = { 1,3,5,6,7 };

	EXPECT_EQ(vec.size(), 5);

	EXPECT_EQ(memcmp(vec.data(), ints, sizeof(ints)), 0);

	//will caouse flip to static memory
	for (auto it = vec.begin(); it != vec.end();)
	{
		if (((*it) == 1) || ((*it) == 5) || ((*it) == 6))
		{
			it = vec.erase(it);
		}
		else
			++it;
	}

	EXPECT_EQ(vec.front(), 3);
	EXPECT_EQ(vec.back(), 7);

	int ints2[] = { 3,7 };

	EXPECT_EQ(vec.size(), 2);
	EXPECT_EQ(memcmp(vec.data(), ints2, sizeof(ints2)), 0);
}

int32_t mallocs = 0, frees = 0;



struct counting_allocator
{
	ml::impl::pod_allocator a;

	using size_type = size_t;
	void* malloc(size_type size)
	{
		++mallocs;
		return a.malloc(size);
	}


	void free(void* mem)
	{
		if (mem) ++frees;
		a.free(mem);
	}
};

template <typename T>
using cpodvec = ml::small_pod_vector<T, 16, 8, counting_allocator>;

TEST(TestCaseName, smallpod7)
{
	mallocs = 0, frees = 0;
	{
		cpodvec<int>  vec;

		vec.insert(vec.begin(), { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20 });
		EXPECT_EQ(vec.size(), 20);

		EXPECT_EQ(mallocs, 1);

		auto it = std::remove_if(vec.begin(), vec.end(), [](auto p) {return p % 2 == 0; });
		vec.erase(it, vec.end());

		EXPECT_EQ(vec.size(), 10);

		int ints[] = { 1,3,5,7,9,11,13,15,17,19 };

		EXPECT_EQ(memcmp(vec.data(), ints, sizeof(ints)), 0);



		vec.erase(vec.begin());
		it = vec.erase(vec.begin());
		EXPECT_EQ(it, vec.begin());
		//back to static
		it = vec.erase(vec.begin());
		EXPECT_EQ(it, vec.begin());
		EXPECT_EQ(7, vec.size());
		EXPECT_EQ(vec.capacity(), 16);
		//to dynamic memory , but should still fit in old memory
		vec.insert(vec.end(), { 21,22,23,24,25,26,27,28,29,30 });

		EXPECT_EQ(mallocs, 1);

		//more memory needed
		vec.insert(vec.end(), { 31,32,33,34,35,36,37,38,39,40,41,42 });

		EXPECT_EQ(mallocs, 2);
		EXPECT_EQ(frees, 1);

	}

	EXPECT_EQ(mallocs, frees);

}