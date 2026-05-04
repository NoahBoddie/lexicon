#pragma once

#include "Lexicon/Engine/TemplateType.h"

namespace LEX
{
    struct TemplateContainer
    {
        static constexpr uint32_t uninitialized = -1;

        size_t size() const
        {
            return GetParentSize() + entries.size();
        }

        size_t index()
        {
            return entries.size();
        }

        TemplateType* at(size_t i) const
        {
            if (auto size = GetParentSize(); i < size) {
                return parent->at(i);
            }
            else {
                i -= size;
            }

            return unconst(std::addressof(entries[i]));
        }

        void Observe() noexcept
        {
            _ref++;
        }
        void Destroy() noexcept
        {
            if (--_ref == 0) {
                TemplateContainer::Destroy();
            }
        }

        uint32_t GetParentSize() const
        {
            if (!parent)
                return 0;


            if (_cachedSize == uninitialized) {
                _cachedSize = parent->size();
            }
            return _cachedSize;
        }

        void SetParent(TemplateContainer* other)
        {
            //If the new thing has more parents than the previous thing, the previous thing must
            // be a child of the current thing. I should make an assert to assure that it doesn't
            // have the thing have 2 parents that arent replated.
            auto parent_size = GetParentSize();
            auto other_size = parent->size();

            if (parent_size < other_size) {
                parent = other;
                _cachedSize = other_size;
            }
        }

    public:
        //The parent will be what handles
        //I'm going to move the parent here.
        TemplateContainer* parent = nullptr;
        std::vector<TemplateType> entries;
    private:
        std::atomic<int32_t> _ref = 1;
        mutable uint32_t _cachedSize = uninitialized;
    };

}