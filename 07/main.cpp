#include <cassert>
#include <iostream>
#include <limits>
#include <numeric>
#include <string>
#include <tuple>
#include <vector>

template <typename T>
class Matrix
{
  public:
    T get(size_t col, size_t row) const
    {
        return m_data[col + row * m_columns];
    }
    T &get(size_t col, size_t row) { return m_data[col + row * m_columns]; }

    void set(size_t col, size_t row, T value)
    {
        m_data[col + row * m_columns] = value;
    }

    void columns(size_t size) { m_columns = size; }
    size_t columns() const { return m_columns; }
    void add_row() { m_data.resize(m_data.size() + m_columns); }
    size_t rows() const { return m_data.size() / m_columns; }

  protected:
    std::vector<T> m_data;
    int m_columns = 0;
};

template <typename T>
std::ostream &operator<<(std::ostream &os, const Matrix<T> &m)
{
    for (size_t row = 0; row < m.rows(); ++row)
    {
        for (size_t col = 0; col < m.columns(); ++col)
            os << m.get(col, row);
        os << '\n';
    }
    return os;
}

enum Cell
{
    FREE_SPACE,
    BEAM_ENTRY,
    SPLITTER,
    BEAM,
};

std::ostream &operator<<(std::ostream &os, const Cell &cell)
{
    switch (cell)
    {
        case FREE_SPACE: os << '.'; break;
        case BEAM_ENTRY: os << 'S'; break;
        case SPLITTER: os << '^'; break;
        case BEAM: os << '|'; break;
    }
    return os;
}

typedef std::tuple<size_t, size_t> Location;
static const Location invalid_location = std::make_tuple(
        std::numeric_limits<size_t>::max(),
        std::numeric_limits<size_t>::max());

class Lab: public Matrix<Cell>
{
  public:
    Location beam_entry() const
    {
        for (size_t i = 0, count = m_data.size(); i < count; ++i)
        {
            if (m_data[i] == BEAM_ENTRY)
                return std::make_tuple(i % columns(), i / columns());
        }
        return invalid_location;
    }

    Cell get(const Location &l) const
    {
        return get(std::get<0>(l), std::get<1>(l));
    }

    Cell get(const size_t col, const size_t row) const
    {
        return Matrix<Cell>::get(col, row);
    }

    void set(const Location &l, Cell value)
    {
        set(std::get<0>(l), std::get<1>(l), value);
    }

    void set(const size_t col, const size_t row, Cell value)
    {
        Matrix<Cell>::set(col, row, value);
    }

    std::tuple<Lab, size_t> propagete_beam() const
    {
        Lab out = *this;
        size_t split_count = 0;

        Location beam_start = beam_entry();

        for (int row = std::get<1>(beam_start) + 1; row < rows(); ++row)
        {
            for (int col = 0; col < columns(); ++col)
            {
                Cell prev = out.get(col, row - 1);
                if (prev == BEAM || prev == BEAM_ENTRY)
                {
                    switch (out.get(col, row))
                    {
                        case SPLITTER:
                            ++split_count;
                            if (col > 0)
                                out.set(col - 1, row, BEAM);
                            if (col + 1 < columns())
                                out.set(col + 1, row, BEAM);
                            break;
                        case FREE_SPACE: out.set(col, row, BEAM); break;
                        default: break;
                    }
                }
            }
        }

        return std::make_tuple(out, split_count);
    }

    size_t timelines(const size_t max_depth, size_t col, size_t row) const
    {
        size_t count = 0;

        if (row >= max_depth)
            return 1;

        switch (get(col, row))
        {
            case BEAM_ENTRY:
            case BEAM:
                // BEAM_ENTRY and BEAM should not be present for timelines
                assert(false);
                break;
            case SPLITTER:
                if (col > 0)
                    count += timelines(max_depth, col - 1, row + 1);
                if (col + 1 < columns())
                    count += timelines(max_depth, col + 1, row + 1);
                break;
            case FREE_SPACE: count = timelines(max_depth, col, row + 1);
            default: break;
        }

        return count;
    }

    size_t timelines(const size_t max_depth) const
    {
        size_t col;
        size_t row;
        std::tie(col, row) = beam_entry();
        return timelines(max_depth, col, row + 1);
    }

    size_t timelines2(const size_t max_depth) const
    {
        std::vector<size_t> paths(columns(), 0);
        std::vector<size_t> next;
        size_t col;
        size_t row;
        std::tie(col, row) = beam_entry();
        paths[col] = 1;
        for (++row; row < max_depth; ++row)
        {
            next.clear();
            next.resize(columns(), 0);
            for (col = 0; col < columns(); ++col)
            {
                if (paths[col] == 0)
                    continue;

                switch (get(col, row))
                {
                    case SPLITTER:
                        if (col > 0)
                            next[col - 1] += paths[col];
                        if (col + 1 < columns())
                            next[col + 1] += paths[col];
                        break;
                    case FREE_SPACE: next[col] += paths[col]; break;
                    default: break;
                }
            }
            paths = std::move(next);
        }
        return std::accumulate(paths.begin(), paths.end(), size_t(0));
    }
};

std::istream &operator>>(std::istream &is, Lab &lab)
{
    std::string line;
    size_t row = 0;
    while (std::getline(is, line))
    {
        if (is.eof())
            break;

        if (line.empty())
            continue;

        if (row == 0)
            lab.columns(line.size());

        size_t col = 0;
        lab.add_row();
        for (char c: line)
        {
            switch (c)
            {
                case '.': lab.set(col, row, FREE_SPACE); break;
                case '^': lab.set(col, row, SPLITTER); break;
                case 'S': lab.set(col, row, BEAM_ENTRY); break;
            }
            ++col;
        }
        ++row;
    }
    return is;
}

int main()
{
    Lab lab;
    std::cin >> lab;
    std::cout << lab;

    auto beam_start = lab.beam_entry();
    if (beam_start == invalid_location)
    {
        std::cerr << "No beam entry found!" << std::endl;
        return 1;
    }

    std::cerr << "Beam entry at (" << std::get<0>(beam_start) << ", "
              << std::get<1>(beam_start) << ")" << std::endl;

    std::cout << "Propagating beam..." << std::endl;
    Lab result;
    size_t split_count = 0;
    std::tie(result, split_count) = lab.propagete_beam();
    std::cout << "Resulting lab:\n"
              << result << "Number of splitters activated: " << split_count
              << std::endl;

    std::cout << "Calculating timelines..." << std::endl;
    size_t timeline_count = lab.timelines2(lab.rows());
    std::cout << "Number of timelines: " << timeline_count << std::endl;

    return 0;
}
