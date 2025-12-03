#include <cstring>
#include <iostream>

class Debugger
{
  public:
    int check_ranges(const char *ranges)
    {
        int ret;
        while (true)
        {
            const char *comma = strchr(ranges, ',');
            if (comma == nullptr)
            {
                comma = strchr(ranges, '\0');
            }
            // std::cout << "Checking range: "
            // 	  << std::string(ranges, comma - ranges) << std::endl;
            ret = check_range(ranges, comma);
            if (ret != 0)
            {
                return ret;
            }
            if (*comma == ',')
            {
                ++comma;
            }
            if (*comma == '\0')
            {
                break;
            }
            ranges = comma;
        }
        return 0;
    }

  private:
    void check_range(long long start, long long end)
    {
        char buffer[50];
        // std::cerr << "Validating range from " << start << " to " << end
        // 	  << std::endl;
        for (long long i = start; i <= end; ++i)
        {
            int len = snprintf(buffer, sizeof(buffer), "%lld", i);
            if (len == sizeof(buffer) || len < 0)
            {
                std::cerr << "Number too large to process: " << i << std::endl;
                exit(1);
            }
            for (int pattern_len = 1, pattern_len_max = len / 2;
                 pattern_len <= pattern_len_max;
                 ++pattern_len)
            {
                if (len % pattern_len != 0)
                    continue;

                int repeat = 1;
                int repeat_count = len / pattern_len;
                for (; repeat < repeat_count; ++repeat)
                {
                    if (strncmp(buffer,
                                buffer + repeat * pattern_len,
                                pattern_len)
                        != 0)
                        break;
                }
                if (repeat == repeat_count)
                {
                    // std::cout << "Invalid: " << i << std::endl;
                    if (repeat_count == 2)
                        part1 += i;
                    part2 += i;
                    break;
                }
            }
        }
    }

    int check_range(const char *begin, const char *last)
    {
        const char *dash;
        char *endptr;

        dash = strchr(begin, '-');
        if (dash == nullptr || dash > last)
        {
            std::cerr << "Missing dash in range: "
                      << std::string(begin, last - begin) << std::endl;
            return 1;
        }
        long long start = strtoll(begin, &endptr, 10);
        if (endptr != dash)
        {
            std::cerr << "Invalid start of range: "
                      << std::string(begin, dash ? dash - begin : last - begin)
                      << std::endl;
            return 1;
        }
        long long end = strtol(dash + 1, &endptr, 10);
        if (endptr != last)
        {
            std::cerr << "Invalid end of range: "
                      << std::string(dash + 1, last - (dash + 1)) << std::endl;
            return 1;
        }

        check_range(start, end);
        return 0;
    }

  public:
    long long part1 = 0;
    long long part2 = 0;
};

int main(int argc, char **argv)
{
    std::string ranges;
    int line = 0;
    int ret = 0;
    Debugger debugger;
    while (ret == 0)
    {
        std::cin >> ranges;
        if (std::cin.eof())
        {
            break;
        }

        // std::cerr << "Processing line " << line + 1 << ": " << ranges
        // 	  << std::endl;

        if (ranges.empty())
        {
            continue;
        }

        ++line;

        ret = debugger.check_ranges(ranges.c_str());
    }
    if (ret == 0)
        std::cout << debugger.part1 << ' ' << debugger.part2 << std::endl;
    return ret;
}
