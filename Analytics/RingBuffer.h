#pragma once
#include <assert.h>

class RingBuffer
{
public:
	class Window
	{
	public:
		Window(RingBuffer &parent, UINT size) :
			m_rParent(parent),
			m_size(size)
		{

		}

		const INT16& operator[](UINT idx) const
		{
			return m_rParent.GetValueFromWindowStart(idx);
		};

		const std::vector<INT16> getWindow() const
		{
			std::vector<INT16> window;

			for (size_t i = 0; i < m_size; i++)
			{
				window.push_back(m_rParent.GetValueFromWindowStart(i));
			}
			return window;
		};

	private:
		RingBuffer &m_rParent;
		size_t m_size;
	};

public:
	RingBuffer();
	RingBuffer(UINT cBufferSize);
	~RingBuffer();

	Window GetWindow(UINT size)
	{
		return Window(*this, size);
	};

	UINT ReadPos();
	UINT WritePos();

	INT16 GetValueFromWindowStart(UINT iShift);
	void WriteNext(INT16);
	void MoveWindow(INT nShift);
	UINT GetMaxWindowSize();
	void ClearBuffer();

private:
	std::vector<INT16> m_vBuffer;	//Holds data 
	UINT m_iReadNextBufferItem;		//Next item to read
	UINT m_iWriteNextBufferItem;	//Next item to write
};







