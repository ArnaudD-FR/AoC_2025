#include <algorithm>
#include <assert.h>
#include <cstring>
#include <iostream>
#include <sstream>
#include <vector>

typedef long long int Value;
typedef std::vector<Value> Values;
enum Operations
{
    OP_ADD,
    OP_MULTIPLY
};

class Problem: public Values
{
  public:
    Value solve() const
    {
        Value res = (*this)[0];
        for (size_t i = 1; i < size(); i++)
        {
            switch (operation)
            {
                case OP_ADD: res += (*this)[i]; break;
                case OP_MULTIPLY: res *= (*this)[i]; break;
            }
        }
        return res;
    }

  public:
    Operations operation;
};

class Problems: public std::vector<Problem>
{
  public:
    Value grand_total() const
    {
        Value total = 0;
        for (const auto &problem: *this)
            total += problem.solve();
        return total;
    }
};

std::ostream &operator<<(std::ostream &out, const Problems &problems)
{
    for (size_t i = 0; i < problems.size(); i++)
    {
        const Problem &problem = problems[i];
        for (size_t j = 0; j < problem.size(); j++)
        {
            out << problem[j];
            if (j < problem.size() - 1)
            {
                out << (problem.operation == OP_ADD ? " + " : " * ");
            }
        }
        out << " = " << problem.solve() << std::endl;
    }
    return out;
}

#if 0
std::istream &operator>>(std::istream &in, Problems &problems)
{
    std::string line;
    std::string word;
    bool first_line = true;
    while (std::getline(in, line))
    {
        std::cout << "Reading line: " << line << std::endl;
        std::istringstream lineStream(line);
        size_t problem_id = 0;
        while (std::getline(lineStream, word, ' '))
        {
            if (word.empty())
                continue;
            std::cout << "  Word: " << word << std::endl;
            if (first_line)
                problems.push_back(Problem());
            Problem &problem = problems[problem_id++];
            if (word == "+")
            {
                problem.operation = OP_ADD;
            }
            else if (word == "*")
            {
                problem.operation = OP_MULTIPLY;
            }
            else
            {
                problem.push_back(std::stoll(word));
            }
            std::cout << "    Value added: " << problem.back() << std::endl;
        }
        first_line = false;
    }
    return in;
}
#else
std::istream &operator>>(std::istream &in, Problems &problems)
{
    // Read all lines
    std::vector<std::string> lines;
    size_t line_size = 0;
    while (true)
    {
        std::string line;
        std::getline(in, line);
        if (in.eof())
            break;
        if (line_size == 0)
            line_size = line.length();
        else if (line.length() != line_size)
        {
            std::cerr << "Inconsistent line sizes: " << line_size << " vs "
                      << line.length() << std::endl;
            in.setstate(std::ios::failbit);
            return in;
        }
        lines.emplace_back(line);
    }

    std::cerr << "Read " << lines.size() << " lines of size " << line_size
              << std::endl;

    // Identify empty columns
    std::vector<bool> empty_cols;
    empty_cols.resize(line_size, true);
    for (size_t col = 0; col < line_size; ++col)
    {
        for (const auto &line: lines)
        {
            if (line[col] != ' ')
            {
                empty_cols[col] = false;
                break;
            }
        }
    }

    // Parse problems
    problems.clear();
    problems.emplace_back();

    size_t value_idx = 0;
    for (size_t col = 0; col < line_size; ++col)
    {
        if (empty_cols[col])
        {
            assert(col != 0);
            // Skip consecutive empty columns
            if (!empty_cols[col - 1])
            {
                ++value_idx;
                problems.emplace_back();
            }
            continue;
        }

        Problem &problem = problems.back();
        Value value = 0;

        for (size_t line_idx = 0; line_idx < lines.size(); ++line_idx)
        {
            const auto &line = lines[line_idx];
            const char c = line[col];
            if (c == ' ')
                continue;
            if (c == '+')
                problem.operation = OP_ADD;
            else if (c == '*')
                problem.operation = OP_MULTIPLY;
            else if (std::isdigit(c))
                value = value * 10 + (c - '0');
            else
            {
                std::cerr << "Unexpected character: " << c << std::endl;
                in.setstate(std::ios::failbit);
                return in;
            }
        }
        problem.push_back(value);
    }

    return in;
}
#endif

int main()
{
    Problems problems;
    std::cin >> problems;
    std::cout << problems;
    std::cout << "Grand Total: " << problems.grand_total() << std::endl;
    return 0;
}
