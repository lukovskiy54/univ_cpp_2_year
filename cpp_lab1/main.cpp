#include <iostream>
#include <random>
#include <vector>

using namespace std;

const int CARDS_PER_SUIT = 13;

class Card{
    public:
        Card(int suit, int value):suit(suit), value(value){}

        friend ostream& operator <<(std::ostream& os,const Card& card)
        {
            os<<"("<<card.suit<<","<<card.value<<")"<<" ";
        }

        auto operator<=>(Card other)
        {
            return this->value <=> other.value;
        }
    private:
        int suit;
        int value;
};

class Deck{
    public:
        Deck(int suit_number):suit(suit_number)
        {
            vector<Card>unrandomized;
            for (int i = 0; i < suit_number; ++i)
            {
                for (int j = 0; j < CARDS_PER_SUIT; ++j)
                {
                    deck.push_back(Card(i,j));
                }
            }
        }
        void shuffle()
        {
            random_device rd;
            mt19937 rng(rd());
            std::shuffle(deck.begin(), deck.end(), rng);
        }

        Card operator()()
        {
            if (deck.empty())
            {
                Deck temp(suit);
                temp.shuffle();
                deck = temp.deck;
            }
            Card lastCard = deck.back();
            deck.pop_back();
            return lastCard;
        }
    private:
        int suit;
        vector<Card> deck;
};

bool findValue(vector<int>myVector, int valueToFind)
{
    auto it = std::find(myVector.begin(), myVector.end(), valueToFind);
    if (it != myVector.end())
    {
        return true;
    } else
    {
        return false;
    }
}
vector <vector<Card>> put_in_stacks(Deck deck,  vector <vector<Card>> all_stacks, int num_of_iterations)
{
    int current_stack = 0;
    Card previous{0, 0};
    Card current{0, 0};
    all_stacks.push_back(vector<Card>());
    for (int i = 0; i < num_of_iterations; ++i) {
        current = deck();
        if (current < previous){
            all_stacks.push_back(vector<Card>());
            current_stack++;
        }
        all_stacks[current_stack].push_back(current);
        previous = current;
    }
    return all_stacks;
}

int main() {
    int num_of_iterations;
    int num_of_suits;
    try {
        cout<<"Enter number of suits"<<endl;
        cin >> num_of_suits;
        if (cin.fail()) {
            throw invalid_argument("Invalid input");
        }
        cout<<"Enter number of cards"<<endl;
        cin >> num_of_iterations;
        if (cin.fail()) {
            throw invalid_argument("Invalid input");
        }
    }
    catch (invalid_argument& e){
        cout<<"Wrong input"<<endl;
        return 1;
    }

    Deck deck(num_of_suits);
    deck.shuffle();
    vector <vector<Card>> all_stacks;
    all_stacks = put_in_stacks(deck,all_stacks,num_of_iterations);

    vector<int>stack_sizes;
    for (auto& stack : all_stacks){
        stack_sizes.push_back(stack.size());
    }
    cout << "Num of stacks: " << size(stack_sizes) << endl;


    vector<int>checked;
    vector<double>percentages;
    double biggest_percentage = 0;
    int most_frequent_count = 0;
    int sum = 0;


    for (auto value : stack_sizes){
        sum += value;
        if (find(checked.begin(),checked.end(),value) == checked.end()){
            double percentage = ((double)count(stack_sizes.begin(), stack_sizes.end(), value) / stack_sizes.size()) * 100;
            cout << "Percentage of stacks with size " << value << ": " << percentage <<" %"<< endl;
            checked.push_back(value);
            if(percentage > biggest_percentage){
                biggest_percentage = percentage;
                most_frequent_count = value;
            }
        }
    }

    cout << "Most frequent value: " << most_frequent_count << endl;
    cout << "Average value: " <<(double)sum/size(stack_sizes) << endl;


    sort(stack_sizes.begin(), stack_sizes.end());
    int mediana;
    if (size(stack_sizes) % 2 == 0)
    {
        mediana = (stack_sizes[(size(stack_sizes)) / 2 - 1] + stack_sizes[(size(stack_sizes)) / 2]) / 2;
    }
    else mediana = stack_sizes[(size(stack_sizes)) / 2];

    cout<<"Mediana is: "<<mediana<<endl;

}

