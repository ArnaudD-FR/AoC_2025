#include <cstring>
#include <iostream>
#include <vector>

enum Cell
{
    Empty,
    Roll,
    Removed,
};

class RollMatrix
{
  public:
    void push_back(bool roll) { content.push_back(roll ? Roll : Empty); }
    size_t get_cols() const { return row_size; }
    void set_cols(size_t size) { row_size = size; }
    size_t get_rows() const { return content.size() / row_size; }
    size_t get_rolls(size_t less)
    {
        size_t ret = 0;
        for (size_t r = 0, rcount = get_rows(); r < rcount; ++r)
        {
            for (size_t c = 0; c < row_size; ++c)
            {
                Cell cell = get(r, c);
                if (cell != Roll)
                    continue;
                size_t adj = get_adj_rolls(r, c);
                if (adj < less)
                {
                    std::cerr << "Roll at " << r << "," << c << " has " << adj
                              << " adjacent rolls" << std::endl;
                    ++ret;
                    set(r, c, Removed);
                }
            }
        }
        return ret;
    }
    void clear_removed()
    {
        for (size_t r = 0, rcount = get_rows(); r < rcount; ++r)
        {
            for (size_t c = 0; c < row_size; ++c)
            {
                Cell cell = get(r, c);
                if (cell == Removed)
                {
                    set(r, c, Empty);
                }
            }
        }
    }
    size_t get_adj_rolls(size_t row, size_t col) const
    {
        size_t ret = 0;
        size_t rows = get_rows();
        for (size_t r = (row > 0 ? row - 1 : row),
                    rmax = (row + 1 < rows ? row + 1 : row);
             r <= rmax;
             ++r)
        {
            for (size_t c = (col > 0 ? col - 1 : 0),
                        cmax = (col + 1 < row_size ? col + 1 : col);
                 c <= cmax;
                 ++c)
            {
                if (r == row && c == col)
                    continue;
                Cell cell = get(r, c);
                if (cell != Empty)
                    ++ret;
            }
        }
        return ret;
    }

    Cell get(size_t row, size_t col) const
    {
        return content[row * row_size + col];
    }

    void set(size_t row, size_t col, Cell value)
    {
        content[row * row_size + col] = value;
    }

  private:
    typedef std::vector<Cell> Content;

  private:
    Content content;
    size_t row_size = 0;
};

std::istream &operator>>(std::istream &in, RollMatrix &matrix)
{
    char line[1024];
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
    std::cerr << "Read line: " << line << std::endl;
    int len = strlen(line);
    if (matrix.get_cols() == 0)
    {
        matrix.set_cols(len);
    }

    for (int i = 0; i < len; ++i)
    {
        char c = line[i];
        switch (c)
        {
            case '.': matrix.push_back(false); break;
            case '@': matrix.push_back(true); break;
            default:
                std::cerr << "Invalid character in input: " << c << std::endl;
        }
    }
    return in;
}

std::ostream &operator<<(std::ostream &out, const RollMatrix &matrix)
{
    size_t rows = matrix.get_rows();
    size_t cols = matrix.get_cols();
    for (size_t r = 0; r < rows; ++r)
    {
        for (size_t c = 0; c < cols; ++c)
        {
            Cell cell = matrix.get(r, c);
            char ch;
            switch (cell)
            {
                case Empty: ch = '.'; break;
                case Roll: ch = '@'; break;
                case Removed: ch = 'x'; break;
            }
            out << ch;
        }
        out << std::endl;
    }
    return out;
}

int main()
{
    RollMatrix matrix;
    while (std::cin >> matrix)
        ;

    size_t total = 0;
    size_t count;
    int i = 0;
    do
    {
        count = matrix.get_rolls(4);
        if (i == 0)
        {
            std::cout << "Part1: Rolls with less than 4 adjacent: " << count
                      << std::endl;
        }
        total += count;
        ++i;
        std::cerr << "After iteration " << i << ", removed " << count
                  << " rolls" << std::endl;
        std::cerr << matrix;
        matrix.clear_removed();
    } while (count > 0);

    std::cout << "Part2: Total rolls removed: " << total << std::endl;

    return 0;
}
