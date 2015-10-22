#include "pch.h"
#include "RingBuffer.h"


RingBuffer::RingBuffer(UINT cBufferSize)
{
	m_vBuffer = std::vector<INT16>(cBufferSize, 0);
	m_iReadNextBufferItem = 0;
	m_iWriteNextBufferItem = 0;
}
RingBuffer::RingBuffer()
{
	m_iReadNextBufferItem = 0;
	m_iWriteNextBufferItem = 0;
}

RingBuffer::~RingBuffer()
{

}

UINT RingBuffer::ReadPos()
{
	return m_iReadNextBufferItem;
}

UINT RingBuffer::WritePos()
{
	return m_iWriteNextBufferItem;
}

void RingBuffer::ClearBuffer()
{
	m_vBuffer = std::vector<INT16>(m_vBuffer.size(), 0);
	m_iReadNextBufferItem = 0;
	m_iWriteNextBufferItem = 0;
}

void RingBuffer::WriteNext(INT16 val)
{
	m_iWriteNextBufferItem = ++m_iWriteNextBufferItem < m_vBuffer.size() ?
		m_iWriteNextBufferItem :
		m_iWriteNextBufferItem = 0;

	m_vBuffer[m_iWriteNextBufferItem] = val;
}

INT16 RingBuffer::GetValueFromWindowStart(UINT iShift)
{
	UINT indx = m_iReadNextBufferItem + iShift < m_vBuffer.size() ?
		m_iReadNextBufferItem + iShift :
		m_iReadNextBufferItem + iShift - m_vBuffer.size();

	return m_vBuffer[indx];
}

void RingBuffer::MoveWindow(INT nShift)
{
	m_iReadNextBufferItem = m_iReadNextBufferItem + nShift  < m_vBuffer.size() ?
		m_iReadNextBufferItem + nShift :
		m_iReadNextBufferItem + nShift - m_vBuffer.size();
}

UINT RingBuffer::GetMaxWindowSize()
{
	UINT smallest = m_vBuffer.size();

	if (m_iReadNextBufferItem < m_iWriteNextBufferItem)
	{
		smallest = (m_iWriteNextBufferItem - m_iReadNextBufferItem) < smallest ?
			m_iWriteNextBufferItem - m_iReadNextBufferItem :
			smallest;
	}
	else if (m_iReadNextBufferItem > m_iWriteNextBufferItem)
	{
		smallest = (m_iWriteNextBufferItem + (m_vBuffer.size() - m_iReadNextBufferItem)) < smallest ?
			(m_iWriteNextBufferItem + (m_vBuffer.size() - m_iReadNextBufferItem)) :
			smallest;
	}
	else
	{
		smallest = 0;
	}

	return smallest;
}
