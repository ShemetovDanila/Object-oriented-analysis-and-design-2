using System.Collections.Generic;

namespace WITH_pattern
{
    public interface IPizzaBuilder
    {
        IPizzaBuilder SetName(string name);
        IPizzaBuilder SetDough(Dough dough);
        IPizzaBuilder SetSauce(Sauce sauce);
        IPizzaBuilder SetBasePrice(int price);
        IPizzaBuilder SetIsCustomPizza(bool isCustom);
        IPizzaBuilder AddTopping(Topping topping, int portions = 1, bool isBase = false);
        IPizzaBuilder SetToppings(Dictionary<Topping, int> toppings, bool isBase = false);
        Pizza Build();
        void Reset();
    }
}