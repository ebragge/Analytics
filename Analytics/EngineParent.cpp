#include "pch.h"
#include "EngineParent.h"


EngineParent::EngineParent()
{

}

void EngineParent::SetConsumer(IMultiRingBufferConsumer *pRingBufferConsumer)
{
	m_pRingBufferConsumer = pRingBufferConsumer;
}