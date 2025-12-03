#include <cstring>
#include <iostream>
#include <vector>

class Bank
{
    typedef std::vector<int> BatteryList;

  public:
    long long int maxjoltage(int digits) const
    {
        const size_t count = batteries.size();
        size_t best = 0;
        long long int out = 0;
        for (size_t d = 0; d < digits; ++d, ++best)
        {
            for (size_t i = best + 1; i < count - (digits - 1) + d; ++i)
            {
                if (batteries[best] < batteries[i])
                    best = i;
            }
            out = out * 10 + batteries[best];
        }
        return out;
    }

  private:
    BatteryList batteries;

    friend std::istream &operator>>(std::istream &in, Bank &bank);
};

std::istream &operator>>(std::istream &in, Bank &bank)
{
    Bank::BatteryList &batteries = bank.batteries;
    char line[128];
    in.getline(line, sizeof(line));
    if (in.eof())
    {
        return in;
    }
    if (in.rdstate() & std::ios::failbit)
    {
        std::cerr << "Error reading line, buffer too small" << std::endl;
        return in;
    }
    int len = strlen(line);
    batteries.reserve(len);
    for (int i = 0; i < len; ++i)
    {
        char c = line[i];
        if (c == '\0')
        {
            break;
        }
        if (c < '0' && c > '9')
        {
            std::cerr << "Invalid character in input: " << c << std::endl;
            in.setstate(std::ios::failbit);
            break;
        }
        batteries.push_back(c - '0');
    }

    return in;
}

int main()
{
    long long int part1 = 0;
    long long int part2 = 0;
    while (true)
    {
        Bank bank;
        std::cin >> bank;
        if (std::cin.eof())
            break;
        if (std::cin.rdstate() & std::ios::failbit)
        {
            std::cerr << "Failed to read battery joltages" << std::endl;
            return 1;
        }
        long long int p1 = bank.maxjoltage(2);
        long long int p2 = bank.maxjoltage(12);
        part1 += p1;
        part2 += p2;
    }
    std::cout << "Total of maximum combined joltages: \n\t2:" << part1
              << "\n\t12:" << part2 << std::endl;
    return 0;
}
