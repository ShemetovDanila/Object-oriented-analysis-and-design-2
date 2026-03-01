using System.Collections.Generic;

namespace WITH_pattern
{
    public interface IPizzaBuilder
    {
        IPizzaBuilder Reset();
        IPizzaBuilder SetIsCustom(bool custom);
        IPizzaBuilder SetName(string name);
        IPizzaBuilder SetBasePrice(int price);
        IPizzaBuilder SetDough(string dough);
        IPizzaBuilder SetSauce(string sauce);
        IPizzaBuilder AddTopping(string topping);
        Pizza GetResult();
    }
}