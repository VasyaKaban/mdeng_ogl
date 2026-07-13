#include "PathCommon.h"

namespace Core
{
    namespace Detail
    {
        static void AdvancePathIterators(const StringView& data, StringView::Iterator& begin, StringView::Iterator& end) noexcept
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
        {
            AdvancePathIterators(this->data, this->begin, this->end);
        }

        PathPartIterator PathPartIterator::operator++(int) noexcept
        {
            PathPartIterator out(*this);

            ++(*this);

            return out;
        }

        PathPartIterator& PathPartIterator::operator++() noexcept
        {
            this->begin = this->end;

            AdvancePathIterators(this->data, this->begin, this->end);

            return *this;
        }

        Bool PathPartIterator::operator==(const PathPartIterator& it) const noexcept
        {
            return this->data == it.data && this->begin == it.begin && this->end == it.end;
        }

        StringView PathPartIterator::operator*() const noexcept
        {
            return StringView(this->begin, this->end);
        }
    };
};