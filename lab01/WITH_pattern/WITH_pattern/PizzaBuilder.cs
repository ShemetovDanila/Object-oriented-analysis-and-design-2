using System.Collections.Generic;

namespace WITH_pattern
{
    public class PizzaBuilder : IPizzaBuilder
    {
        private Pizza _pizza;

        public IPizzaBuilder Reset()
        {
            _pizza = new Pizza();
            _pizza.Toppings = new List<Topping>();
            return this;
        }

        public IPizzaBuilder SetName(string name)
        {
            _pizza.name = name;
            return this;
        }

        public IPizzaBuilder SetBasePrice(int price)
        {
            _pizza.price = price;
            return this;
        }

        public IPizzaBuilder SetIsCustom(bool custom)
        {
            _pizza.custom = custom;
            return this;
        }

        public IPizzaBuilder SetDough(string dough)
        {
            _pizza.Dough = ToppingCatalog.GetIngredient(dough) as Dough;
            return this;
        }

        public IPizzaBuilder SetSauce(string sauce)
        {
            _pizza.Sauce = ToppingCatalog.GetIngredient(sauce) as Sauce;
            return this;
        }

        public IPizzaBuilder AddTopping(string topping)
        {
            var t = ToppingCatalog.GetIngredient(topping) as Topping;
            if (t != null)
            {
                _pizza.Toppings.Add(t);
            }
            return this;
        }

        public Pizza GetResult()
        {
            return _pizza;
        }
    }
}