#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include <regex>
#include <queue>

using namespace std;

bool is_valid(const std::string &str)
{
    return str.find_first_not_of("0123456789:,") == std::string::npos;
}

void findAllOccurances(std::vector<size_t> & vec, std::queue<char> & q, std::string data, std::string toSearch)
{
    size_t pos = data.find_first_of(toSearch);
    while( pos != std::string::npos)
    {
        vec.push_back(pos);
        q.push(data.at(pos));
        pos = data.find_first_of(toSearch, pos + 1);
    }
}

int main()
{

    bool inRange;
    int startRange;
    int step;
    int endRange;

    std::vector<size_t> idxs;
    std::queue<char> vals;

    std::string test = "213";

    std::regex re("[\\|,:]");
    std::sregex_token_iterator first{test.begin(), test.end(), re, -1}, last;//the '-1' is what makes the regex split (-1 := what was not matched)
    std::vector<std::string> tokens{first, last};

    std::queue<int> nums;
    for (auto token : tokens)
    {
        if (token.size())
            nums.push(std::stoi(token));
    }

    findAllOccurances(idxs, vals, test, ",:"); 

    for (auto pos : idxs)
    {
        std::cout << "Found '" << test.at(pos) << "' " << "@" << pos << std::endl;
    }


    if (!nums.size())
    {
        if (vals.size() == 1 && vals.front() == ':')
            std::cout << "Select all channels" << std::endl;
        else
        {
            std::cout << "Invalid input!" << std::endl;
        }
        return 1;
    }
    else if (!vals.size())
    {
        std::cout << "Selecting channel: " << nums.front() << std::endl;
    }
    else
    {
        while (nums.size())
        {

            if (vals.front() == ':')
            {
                //Got a : but already started a range...means next num in queue should be a step
                if (inRange)
                {
                    step = nums.front();
                }
                else 
                {
                    startRange = nums.front();
                    step = 0;
                    inRange = true;
                }
                nums.pop();
            }
            else if (!vals.size() || vals.front() == ',')
            {
                if (inRange)
                {
                    std::cout << "Set the range" << std::endl;
                    inRange = false;
                }
                else
                {
                    std::cout << "Set the channel" << std::endl;
                }
                
            }
            else
            {
                std::cout << "Should never reach here" << std::endl;
            }

            if (!vals.size() && nums.size() == 1)
            {
                if (inRange)
                {
                    endRange = nums.pop();
                }
                
                {
                    std::cout << "Add single channel" << std::endl;
                    nums.pop();
                }

            }
            
        }
    }
    

    

    fflush(stdout);
    
}
