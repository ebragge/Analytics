#pragma once
#include "AudioControlData.h"
#include "IMultiRingBufferConsumer.h"

private ref class EngineParent abstract
{
protected:
internal:

	EngineParent();

	virtual void SetConsumer(IMultiRingBufferConsumer *pRingBufferConsumer);
	virtual void StopAsync() = 0;

	IMultiRingBufferConsumer *m_pRingBufferConsumer;
};

