using System.Collections.Generic;

namespace WITH_pattern
{
    public class PizzaBuilder : IPizzaBuilder
    {
        private Pizza _pizza;

        public PizzaBuilder()
        {
            Reset();
        }

        public void Reset()
        {
            _pizza = new Pizza();
        }

        public IPizzaBuilder SetName(string name)
        {
            _pizza.Name = name;
            return this;
        }

        public IPizzaBuilder SetDough(Dough dough)
        {
            _pizza.Dough = dough;
            return this;
        }

        public IPizzaBuilder SetSauce(Sauce sauce)
        {
            _pizza.Sauce = sauce;
            return this;
        }

        public IPizzaBuilder SetBasePrice(int price)
        {
            _pizza.BasePrice = price;
            return this;
        }

        public IPizzaBuilder SetIsCustomPizza(bool isCustom)
        {
            _pizza.IsCustomPizza = isCustom;
            return this;
        }

        public IPizzaBuilder AddTopping(Topping topping, int portions = 1, bool isBase = false)
        {
            _pizza.AddTopping(topping, portions, isBase);
            return this;
        }

        public IPizzaBuilder SetToppings(Dictionary<Topping, int> toppings, bool isBase = false)
        {
            foreach (var kvp in toppings)
            {
                _pizza.AddTopping(kvp.Key, kvp.Value, isBase);
            }
            return this;
        }

        public Pizza Build()
        {
            Pizza result = _pizza;
            Reset();
            return result;
        }
    }
}