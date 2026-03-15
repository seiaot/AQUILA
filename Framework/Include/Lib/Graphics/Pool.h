#pragma once

#include <cstdint>
#include <mutex>
#include <cassert>
#include <functional>

#include <stdlib.h>
#include <malloc.h>

namespace Lib {
namespace Graphics {

template<typename T>
class Pool
{
	struct Item {
		T m_Value;
		uint32_t m_Index;
		Item* m_pPrev;
		Item* m_pNext;

		Item()
			: m_Value()
			, m_Index(0)
			, m_pPrev(nullptr)
			, m_pNext(nullptr)
		{
			;
		}
	};

public:
	//コンストラクタ
	Pool()
		: m_pBuffer(nullptr)
		, m_Active()
		, m_Free()
		, m_Capacity(0)
		, m_Count(0)
	{
		;
	}

	//デストラクタ
	~Pool()
	{
		Term();
	}

	//初期化
	bool Init(uint32_t count)
	{
		std::lock_guard<std::mutex> guard(m_Mutex);

		m_pBuffer = static_cast<uint8_t*>(malloc(sizeof(Item) * count));
		if (m_pBuffer == nullptr)
			return  false;

		m_Capacity = count;

		m_Active.m_pPrev = m_Active.m_pNext = &m_Active;
		m_Active.m_Index = uint32_t(-1);

		m_Free.m_Index = uint32_t(-2);
		{
			Item* pPrev = &m_Free;
			for (auto i = 0u; i < m_Capacity; ++i)
			{
				auto item = AssignItem(i);

				item->m_Index = i;
				item->m_pPrev = pPrev;

				pPrev->m_pNext = item;

				pPrev = item;
			}
			pPrev->m_pNext = &m_Free;
			m_Free.m_pPrev = pPrev;
		}

		m_Count = 0;

		return true;
	}

	//終了処理
	void Term()
	{
		std::lock_guard<std::mutex> guard(m_Mutex);

		if (m_pBuffer)
		{
			free(m_pBuffer);
			m_pBuffer = nullptr;
		}

		m_Active.m_pPrev = nullptr;
		m_Active.m_pNext = nullptr;
		m_Free.m_pPrev = nullptr;
		m_Free.m_pNext = nullptr;

		m_Capacity = 0;
		m_Count = 0;
	}

	//アイテムを確保する
	T* Alloc(std::function<void(uint32_t, T*)>func = nullptr)
	{
		std::lock_guard<std::mutex> guard(m_Mutex);

		if (m_Free.m_pNext == &m_Free || m_Count >= m_Capacity)
			return nullptr;

		auto item = m_Free.m_pNext;
		m_Free.m_pNext = item->m_pNext;

		item->m_pPrev = m_Active.m_pPrev;
		item->m_pNext = &m_Active;
		item->m_pPrev->m_pNext = item->m_pNext->m_pPrev = item;

		++m_Count;

		//メモリ割り当て
		auto val = new((void*)item) T();

		if (func != nullptr) {
			func(item->m_Index, val);
		}

		return val;
	}

	void Free(T* pValue)
	{
		if (pValue == nullptr)
			return;

		std::lock_guard<std::mutex> guard(m_Mutex);

		auto item = reinterpret_cast<Item*>(pValue);

		item->m_pPrev->m_pNext = item->m_pNext;
		item->m_pNext->m_pPrev = item->m_pPrev;

		item->m_pNext = m_Free.m_pNext;
		item->m_pPrev = &m_Free;

		m_Free.m_pNext->m_pPrev = item;
		m_Free.m_pNext = item;

		--m_Count;
	}

	uint32_t GetSize() const {
		return m_Capacity;
	}

	uint32_t GetUsedCount()const {
		return m_Count;
	}

	uint32_t GetAvailableCount()const {
		return m_Capacity - m_Count;
	}


private:

	Item* GetItem(uint32_t i)
	{
		assert(i >= 0 && i < m_Capacity);
		return reinterpret_cast<Item*>(m_pBuffer + sizeof(Item) * i);
	}

	//多分使ってない
	Item* AssignItem(uint32_t i)
	{
		assert(i >= 0 && i < m_Capacity);
		auto buf = (m_pBuffer + sizeof(Item) * i);
		return new (buf) Item;
	}

private:

	uint8_t* m_pBuffer;
	Item m_Active;
	Item m_Free;
	uint32_t m_Capacity;
	uint32_t m_Count;
	std::mutex m_Mutex;

	Pool(const Pool&) = delete;
	void operator = (const Pool&) = delete;

};

}//namespace Graphics
}//namespace Lib