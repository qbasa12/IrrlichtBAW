#ifndef __IRR_C_DOUBLE_BUFFERING_ALLOCATOR_H__
#define __IRR_C_DOUBLE_BUFFERING_ALLOCATOR_H__


#include "IGPUBuffer.h"
#include "IDriverMemoryAllocation.h"
#include "IVideoDriver.h"

namespace irr
{
namespace video
{

template<class AddressAllocator, bool onlySwapRangesMarkedDirty>
class CDoubleBufferingAllocatorBase;
template<class AddressAllocator>
class CDoubleBufferingAllocatorBase<AddressAllocator,false>
{
    protected:
        std::pair<typename AddressAllocator::size_type,typename AddressAllocator::size_type> dirtyRange;

        CDoubleBufferingAllocatorBase() {}
        virtual ~CDoubleBufferingAllocatorBase() {}

        inline void resetDirtyRange() {}

        static constexpr bool alwaysSwapEntireRange = true;
    public:
        inline decltype(dirtyRange) getDirtyRange() const {return dirtyRange;}
};
template<class AddressAllocator>
class CDoubleBufferingAllocatorBase<AddressAllocator,true>
{
    protected:
        std::pair<typename AddressAllocator::size_type,typename AddressAllocator::size_type> dirtyRange;

        CDoubleBufferingAllocatorBase() {resetDirtyRange();}
        virtual ~CDoubleBufferingAllocatorBase() {}

        inline void resetDirtyRange() {dirtyRange.first = 0x7fffFFFFu; dirtyRange.second = 0;}

        static constexpr bool alwaysSwapEntireRange = false;
    public:
        inline decltype(dirtyRange) getDirtyRange() const {return dirtyRange;}

        inline void markRangeDirty(typename AddressAllocator::size_type begin, typename AddressAllocator::size_type end)
        {
            if (begin<dirtyRange.first) dirtyRange.first = begin;
            if (end>dirtyRange.second) dirtyRange.second = end;
        }
};


template<class AddressAllocator, bool onlySwapRangesMarkedDirty = false, class CPUAllocator=core::allocator<uint8_t> >
class CDoubleBufferingAllocator : public CDoubleBufferingAllocatorBase<AddressAllocator,onlySwapRangesMarkedDirty>, public virtual core::IReferenceCounted
{
    private:
        typedef CDoubleBufferingAllocatorBase<AddressAllocator,onlySwapRangesMarkedDirty> Base;
    protected:
        IVideoDriver*       mDriver;
        IGPUBuffer*         mBackBuffer;
        IGPUBuffer*         mFrontBuffer;
        void*               mStagingPointer;
        size_t              mRangeLength;

        size_t              mRangeStartRelToFB;
        size_t              mDestBuffOff;

        size_t              mReservedSize; // for allocator external state
        CPUAllocator        mCPUAllocator;
        void*               mAllocatorState;
        AddressAllocator    mAllocator;

        virtual ~CDoubleBufferingAllocator()
        {
            mCPUAllocator.deallocate(mAllocatorState,mReservedSize);
            mBackBuffer->drop();
            mFrontBuffer->drop();
        }
    public:
		//! Creates a double buffering allocator from two GPU Buffers, where the staging buffer must have its bound memory already mapped with an appropriate range.
        /** Both buffers need to have already had their memory bound, i.e. IGPUBuffer::getBoundMemory cannot return nullptr.
        MEMORY CANNOT BE UNMAPPED OR REMAPPED FOR THE LIFETIME OF THE CDoubleBufferingAllocator OBJECT !!!
		@param rangeToUse is a memory range relative to the staging buffer's bound memory start, not the buffer start or the mapped IDriverMemoryAllocation offset. */
        template<typename... Args>
        CDoubleBufferingAllocator(IVideoDriver* driver, const IDriverMemoryAllocation::MemoryRange& rangeToUse, IGPUBuffer* stagingBuff, size_t destBuffOffset, IGPUBuffer* destBuff, Args&&... args) :
                        mDriver(driver), mBackBuffer(stagingBuff), mFrontBuffer(destBuff), mStagingPointer(mBackBuffer->getBoundMemory()->getMappedPointer()),
                        mRangeLength(rangeToUse.length), mRangeStartRelToFB(stagingBuff->getBoundMemoryOffset()-rangeToUse.offset), mDestBuffOff(destBuffOffset),
                        mReservedSize(AddressAllocator::reserved_size(mRangeLength,std::forward<Args>(args)...)),
                        mCPUAllocator(), mAllocatorState(mCPUAllocator.allocate(mReservedSize,_IRR_SIMD_ALIGNMENT)),
                        mAllocator(mAllocatorState,mStagingPointer,mRangeLength)
        {
#ifdef _DEBUG
            assert(stagingBuff->getBoundMemoryOffset()>=rangeToUse.offset);
#endif // _DEBUG
            mBackBuffer->grab();
            mFrontBuffer->grab();
        }


        inline void* getBackBufferPointer() {return mStagingPointer;}

        inline IGPUBuffer* getFrontBuffer() {return mFrontBuffer;}


        template<typename... Args>
        inline typename AddressAllocator::size_type alloc_addr(Args&&... args) {return mAllocator.alloc_addr(std::forward<Args>(args)...);}

        template<typename... Args>
        inline typename AddressAllocator::size_type free_addr(Args&&... args) {return mAllocator.free_addr(std::forward<Args>(args)...);}


        //! Makes Writes visible
        inline void swapBuffers(void (*StuffToDoToBackBuffer)(IGPUBuffer*,void*)=NULL, void (*StuffToDoToFrontBuffer)(IGPUBuffer*,void*)=NULL,void* userData=NULL)
        {
            if (StuffToDoToBackBuffer)
                StuffToDoToBackBuffer(mBackBuffer,userData);

            if (Base::alwaysSwapEntireRange)
            {
                IDriverMemoryAllocation::MappedMemoryRange range;
                range.memory = mBackBuffer->getBoundMemory();
                range.offset = mDestBuffOff+mBackBuffer->getBoundMemoryOffset();
                range.length = mRangeLength;
                mDriver->flushMappedMemoryRanges(1,&range);

                mDriver->copyBuffer(mBackBuffer,mFrontBuffer,mRangeStartRelToFB,mDestBuffOff,mRangeLength);
            }
            else if (Base::dirtyRange.first<Base::dirtyRange.second)
            {
                size_t tmpBackOffset = mDestBuffOff+Base::dirtyRange.first;

                IDriverMemoryAllocation::MappedMemoryRange range;
                range.memory = mBackBuffer->getBoundMemory();
                range.offset = tmpBackOffset+mBackBuffer->getBoundMemoryOffset();
                range.length = Base::dirtyRange.second-Base::dirtyRange.first;
                mDriver->flushMappedMemoryRanges(1,&range);

                mDriver->copyBuffer(mBackBuffer,mFrontBuffer,tmpBackOffset,
                                    mDestBuffOff+Base::dirtyRange.first,range.length);
                Base::resetDirtyRange();
            }

            if (StuffToDoToFrontBuffer)
                StuffToDoToFrontBuffer(mFrontBuffer,userData);
        }
};


template<class AddressAllocator, bool onlySwapRangesMarkedDirty = false, class CPUAllocator=core::allocator<uint8_t> >
class CDoubleBufferingAllocatorExt : public CDoubleBufferingAllocator<AddressAllocator,onlySwapRangesMarkedDirty,CPUAllocator>
{
    public:
};

}
}

#endif // __IRR_C_DOUBLE_BUFFERING_ALLOCATOR_H__

