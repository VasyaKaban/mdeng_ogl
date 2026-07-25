#include "PathCommon.h"

namespace Core
{
    namespace Detail
    {
        static Void AdvancePathIterators(const StringView& data, StringView::Iterator& begin, StringView::Iterator& end) noexcept
        {
            if(begin != data.GetSentinel()) //not end
            {
                for(; end != data.GetSentinel(); end++)
                {
                    if((*end).utf32 == U'/')
                    {
                        end++;
                        break;
                    }
                }
            }
        }

        PathPartIterator::PathPartIterator(StringView data, StringView::Iterator begin) noexcept
            : data(data),
              begin(begin),
              end(begin)
        {}

        PathPartIterator PathPartIterator::operator++(int) noexcept
        {
            PathPartIterator out(*this);

            ++(*this);

            return out;
        }

        PathPartIterator& PathPartIterator::operator++() noexcept
        {
            if(this->begin == this->end) //advance
                AdvancePathIterators(this->data, this->begin, this->end);

            this->begin = this->end;

            return *this;
        }

        Bool PathPartIterator::operator==(const PathPartIterator& it) const noexcept
        {
            return this->begin == it.begin;
        }

        StringView PathPartIterator::operator*() const noexcept
        {
            if(this->begin == this->end) //advance
                AdvancePathIterators(this->data, this->begin, this->end);

            return StringView(this->begin, this->end);
        }

        StringView::Iterator PathPartIterator::GetDataIterator() const noexcept
        {
            return this->begin;
        }

        StringView::Iterator PathPartIterator::GetDataSentinel() const noexcept
        {
            return this->end;
        }
    };
};