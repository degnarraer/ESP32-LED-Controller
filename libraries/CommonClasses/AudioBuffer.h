/*
    Light Tower by Rob Shockency
    Copyright (C) 2021 Rob Shockency degnarraer@yahoo.com

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version of the License, or
    (at your option) any later version. 3

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef AudioBuffer_H
#define AudioBuffer_H


#include "Datatypes.h"
#include "CircularBuffer.h" 
#include "circle_buf.h"

template <uint32_t T>
class AudioBuffer
{
  public:
    AudioBuffer(){}
    virtual ~AudioBuffer()
    {
      FreeMemory();
    }
    void Initialize()
	{ 
		AllocateMemory();pthread_mutexattr_t Attr;
		pthread_mutexattr_init(&Attr);
		pthread_mutexattr_settype(&Attr, PTHREAD_MUTEX_RECURSIVE);	  
		if(0 != pthread_mutex_init(&m_Lock, &Attr))
		{   
			ESP_LOGE("TestClass", "Failed to Create Lock");
		}
	}

	void AllocateMemory()
	{
		size_t CircleBuffSize = sizeof(bfs::CircleBuf<Frame_t, T>);
		void *CircularBuffer_Raw = (bfs::CircleBuf<Frame_t, T>*)heap_caps_malloc(CircleBuffSize, MALLOC_CAP_SPIRAM);
		m_CircularAudioBuffer = new(CircularBuffer_Raw) bfs::CircleBuf<Frame_t, T>;
	}

	void FreeMemory()
	{
	 heap_caps_free(m_CircularAudioBuffer);
	}
	size_t GetFrameCapacity()
	{
		size_t Capacity = 0;
		pthread_mutex_lock(&m_Lock);
		Capacity = m_CircularAudioBuffer->capacity();
		pthread_mutex_unlock(&m_Lock);
		return Capacity;
	}

	bool ClearAudioBuffer()
	{
		bool Success = false;
		pthread_mutex_lock(&m_Lock);
		m_CircularAudioBuffer->Clear();
		Success = true;
		pthread_mutex_unlock(&m_Lock);
		return Success;
	}

	size_t GetFrameCount()
	{
		size_t size = 0;
		pthread_mutex_lock(&m_Lock);
		size = m_CircularAudioBuffer->size();
		pthread_mutex_unlock(&m_Lock);
		return size;
	}

	size_t GetFreeFrameCount()
	{
		return GetFrameCapacity() - GetFrameCount();
	}

	size_t WriteAudioFrames( Frame_t *FrameBuffer, size_t FrameCount )
	{
		size_t FramesWritten = 0;
		pthread_mutex_lock(&m_Lock);
		FramesWritten = m_CircularAudioBuffer->Write(FrameBuffer, FrameCount);
		pthread_mutex_unlock(&m_Lock);
		return FramesWritten;
	}

	bool WriteAudioFrame( Frame_t Frame )
	{
		bool Success = false;
		pthread_mutex_lock(&m_Lock);
		Success = m_CircularAudioBuffer->Write(Frame);
		pthread_mutex_unlock(&m_Lock);
		return Success;
	}

	size_t ReadAudioFrames(Frame_t *FrameBuffer, size_t FrameCount)
	{
		size_t FramesRead = 0;
		pthread_mutex_lock(&m_Lock);
		FramesRead = m_CircularAudioBuffer->Read(FrameBuffer, FrameCount);
		pthread_mutex_unlock(&m_Lock);
		return FramesRead;
	}

	bfs::optional<Frame_t> ReadAudioFrame()
	{
		bfs::optional<Frame_t> FrameRead;
		pthread_mutex_lock(&m_Lock);
		FrameRead = m_CircularAudioBuffer->Read();
		pthread_mutex_unlock(&m_Lock);
		return FrameRead;
	}
	
	private:
		bfs::CircleBuf<Frame_t, T> *m_CircularAudioBuffer;
		pthread_mutex_t m_Lock;
};


template <uint32_t T>
class ContinuousAudioBuffer
{
  public:
    ContinuousAudioBuffer(){}
    virtual ~ContinuousAudioBuffer()
    {
      FreeMemory();
    }
    void Initialize()
	{ 
		AllocateMemory();  
		pthread_mutexattr_t Attr;
		pthread_mutexattr_init(&Attr);
		pthread_mutexattr_settype(&Attr, PTHREAD_MUTEX_RECURSIVE);	  
		if(0 != pthread_mutex_init(&m_Lock, &Attr))
		{
			ESP_LOGE("TestClass", "Failed to Create Lock");
		}
	}

	void AllocateMemory()
	{
		size_t CircleBuffSize = sizeof(CircularBuffer<Frame_t, T>);
		void *CircularBuffer_Raw = (CircularBuffer<Frame_t, T>*)heap_caps_malloc(CircleBuffSize, MALLOC_CAP_SPIRAM);
		m_CircularAudioBuffer = new(CircularBuffer_Raw) CircularBuffer<Frame_t, T>;
	}

	void FreeMemory()
	{
		heap_caps_free(m_CircularAudioBuffer);
	}
	
	uint32_t GetAudioFrames(Frame_t *Buffer, uint32_t Count)
	{
		uint32_t ReturnCount = 0;
		pthread_mutex_lock(&m_Lock);
		for(int i = 0; i < Count && i < m_CircularAudioBuffer->size(); ++i)
		{
			Buffer[i] = ((Frame_t*)m_CircularAudioBuffer)[i];
			++ReturnCount;
		}
		pthread_mutex_unlock(&m_Lock);
		return ReturnCount;
	}
	
	bool Push(Frame_t Frame)
	{
		bool Result = false;
		pthread_mutex_lock(&m_Lock);
		Result = m_CircularAudioBuffer->push(Frame);
		pthread_mutex_unlock(&m_Lock);
		return Result;
	}

	size_t Push(Frame_t *Frame, size_t Count)
	{
		size_t Result = 0;
		pthread_mutex_lock(&m_Lock);
		for(int i = 0; i < Count; ++i)
		{
			m_CircularAudioBuffer->push(Frame[i]);
			++Result;
		}
		pthread_mutex_unlock(&m_Lock);
		return Result;
	}

	Frame_t Pop()
	{
		Frame_t Result;
		pthread_mutex_lock(&m_Lock);
		Result = m_CircularAudioBuffer->pop();
		pthread_mutex_unlock(&m_Lock);
		return Result;
	}

	size_t Pop(Frame_t *Frame, size_t Count)
	{
		size_t Result = 0;
		pthread_mutex_lock(&m_Lock);
		for(int i = 0; i < Count; ++i)
		{
			Frame[i] == m_CircularAudioBuffer->pop();
			++Result;
		}
		pthread_mutex_unlock(&m_Lock);
		return Result;
	}

	bool Unshift(Frame_t Frame)
	{
		bool Result = false;
		pthread_mutex_lock(&m_Lock);
		Result = m_CircularAudioBuffer->unshift(Frame);
		pthread_mutex_unlock(&m_Lock);
		return Result;
	}

	size_t Unshift(Frame_t *Frame, size_t Count)
	{
		size_t Result = 0;
		pthread_mutex_lock(&m_Lock);
		for(int i = 0; i < Count; ++i)
		{
			if( true == m_CircularAudioBuffer->unshift(Frame[i]) )
			{
				++Result;
			}
			else
			{
				break;
			}
		}
		pthread_mutex_unlock(&m_Lock);
		return Result;
	}

	Frame_t Shift()
	{
		Frame_t Result;
		pthread_mutex_lock(&m_Lock);
		Result = m_CircularAudioBuffer->shift();
		pthread_mutex_unlock(&m_Lock);
		return Result;
	}

	size_t Shift(Frame_t *Frame, size_t Count)
	{
		size_t Result = 0;
		pthread_mutex_lock(&m_Lock);
		for(int i = 0; i < Count; ++i)
		{
			Frame[i] = m_CircularAudioBuffer->shift();
			++Result;
		}
		pthread_mutex_unlock(&m_Lock);
		return Result;
	}

	bool IsEmpty()
	{
		bool Result = false;
		pthread_mutex_lock(&m_Lock);
		Result = m_CircularAudioBuffer->isEmpty();
		pthread_mutex_unlock(&m_Lock);
		return Result;
	}

	bool IsFull()
	{
		bool Result = false;
		pthread_mutex_lock(&m_Lock);
		Result = m_CircularAudioBuffer->isFull();
		pthread_mutex_unlock(&m_Lock);
		return Result;
	}

	size_t Size()
	{
		size_t Result = 0;
		pthread_mutex_lock(&m_Lock);
		Result = m_CircularAudioBuffer->size();
		pthread_mutex_unlock(&m_Lock);
		return Result;
	}

	size_t Available()
	{
		size_t Result = 0;
		pthread_mutex_lock(&m_Lock);
		Result = m_CircularAudioBuffer->available();
		pthread_mutex_unlock(&m_Lock);
		return Result;
	}

	void Clear()
	{
		pthread_mutex_lock(&m_Lock);
		m_CircularAudioBuffer->clear();
		pthread_mutex_unlock(&m_Lock);
	}
	  private:
		CircularBuffer<Frame_t, T> *m_CircularAudioBuffer;
		pthread_mutex_t m_Lock;
};

#endif
