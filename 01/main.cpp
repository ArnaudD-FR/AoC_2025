#include <iomanip>
#include <iostream>

class Dial
{
  public:
    Dial &operator-=(int distance)
    {
        increment(-distance);
        return *this;
    }

    Dial &operator+=(int distance)
    {
        increment(distance);
        return *this;
    }

  private:
    void increment(int distance)
    {
        int old_position = position;
        int new_position = position + distance;
        int tick_zero = 0;

        if (new_position > 0)
        {
            position = new_position % 100;
            tick_zero = new_position / 100;
        }
        else if (new_position < 0)
        {
            position = 100 + (new_position % 100);
            if (position == 100)
                position = 0;
            tick_zero = -new_position / 100;
            if (old_position != 0)
                ++tick_zero;
        }
        else
        {
            position = 0;
            ++points_at_zero;
            tick_zero = 1;
        }

        cross_zero += tick_zero;
    }

  private:
    int position = 50;
    int points_at_zero = 0;
    int cross_zero = 0;

    friend std::ostream &operator<<(std::ostream &os, const Dial &dial);
};

std::ostream &operator<<(std::ostream &os, const Dial &dial)
{
    // Assuming we have access to the position for output
    // In a real implementation, we would have a getter method
    return os << "position: " << std::setw(2) << std::setfill(' ')
              << dial.position << "; points at 0: " << std::setw(5)
              << dial.points_at_zero
              << "; method 0x434C49434B: " << std::setw(5) << dial.cross_zero;
}

int main(int argc, char *argv[])
{
    char rotation;
    int distance;
    Dial dial;
    int line = 0;

    std::cout << "Initial: " << dial << std::endl;
    while (true)
    {
        std::cin >> rotation >> distance;
        if (std::cin.eof())
        {
            break;
        }

        ++line;

        if (distance <= 0)
        {
            std::cerr << "Error line " << line
                      << ": Count must be between 1 and 99 but got " << distance
                      << std::endl;
            return 1;
        }

        switch (rotation)
        {
            case 'L': dial -= distance; break;
            case 'R': dial += distance; break;
            default:
                // This case should never be reached due to prior validation
                std::cerr << "Error line " << line
                          << ": Invalid rotation input. Expecting 'L' or 'R' "
                             "but got '"
                          << rotation << "'." << std::endl;
                break;
        }
        // std::cout << "Step " << rotation << std::setw(5) << distance
        // 	  << ": " << dial << std::endl;
    }

    std::cout << "Final: " << dial << std::endl;
    return 0;
}
