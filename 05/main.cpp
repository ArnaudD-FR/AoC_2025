#include <algorithm>
#include <cstring>
#include <iostream>
#include <sstream>
#include <vector>

typedef long long int Value;

class Range
{
  public:
    bool contains(Value value) const { return value >= start && value <= end; }
    bool overlaps(const Range &other) const
    {
        return !(end < other.start || start > other.end);
    }
    Range merge(const Range &other) const
    {
        return Range { std::min(start, other.start), std::max(end, other.end) };
    }
    size_t size() const { return end - start + 1; }

  public:
    Value start;
    Value end;
};

class Ranges: public std::vector<Range>
{
  public:
    bool contains(Value value) const
    {
        for (const auto &range: *this)
        {
            if (range.contains(value))
                return true;
        }
        return false;
    }

    Ranges merge_overlaps() const
    {
        Ranges merged;
        if (empty())
            return merged;

        Ranges sorted = *this;
        std::sort(
                sorted.begin(),
                sorted.end(),
                [](const Range &a, const Range &b)
                { return a.start < b.start; });

        Range current = sorted[0];
        for (size_t i = 1; i < sorted.size(); ++i)
        {
            const Range &next = sorted[i];
            if (current.overlaps(next))
            {
                current = current.merge(next);
            }
            else
            {
                merged.push_back(current);
                current = next;
            }
        }
        merged.push_back(current);
        return merged;
    }

    size_t total_covered() const
    {
        size_t total = 0;
        for (const auto &range: *this)
            total += range.size();
        return total;
    }
};

std::istream &operator>>(std::istream &in, Range &range)
{
    char dash;
    in >> range.start >> dash >> range.end;
    return in;
}

std::istream &operator>>(std::istream &in, Ranges &ranges)
{
    char line[256];
    Range range;
    while (in.getline(line, sizeof(line)))
    {
        std::cout << "Read line: " << strlen(line) << " " << line << std::endl;
        if (strlen(line) == 0)
            break;
        std::istringstream lineStream(line);
        lineStream >> range;
        ranges.push_back(range);
    }
    return in;
}

std::ostream &operator<<(std::ostream &out, const Range &range)
{
    out << range.start << "-" << range.end;
    return out;
}

std::ostream &operator<<(std::ostream &out, const Ranges &ranges)
{
    for (const auto &range: ranges)
        out << range << std::endl;
    return out;
}

int main()
{
    Ranges ranges;
    Value value;
    int total_fresh = 0;
    std::cin >> ranges;
    std::cout << "Loaded ranges:\n" << ranges << std::endl;

    while (std::cin >> value)
    {
        if (ranges.contains(value))
            total_fresh++;
    }

    std::cout << "Total fresh values: " << total_fresh << std::endl;
    Ranges merged = ranges.merge_overlaps();
    std::cout << "Merged ranges:\n" << merged << std::endl;
    std::cout << "Total covered values: " << merged.total_covered()
              << std::endl;
    return 0;
}
